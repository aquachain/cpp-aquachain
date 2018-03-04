#include <jsonrpccpp/common/exception.h>
#include <libdevcore/CommonJS.h>
#include <libaquacore/KeyManager.h>
#include <libaquachain/Client.h>
#include <libaquachain/Executive.h>
#include <libaquahashseal/EthashClient.h>
#include "AdminEth.h"
#include "SessionManager.h"
#include "JsonHelper.h"
using namespace std;
using namespace dev;
using namespace dev::rpc;
using namespace dev::aqua;

AdminEth::AdminEth(aqua::Client& _aqua, aqua::TrivialGasPricer& _gp, aqua::KeyManager& _keyManager, SessionManager& _sm):
	m_aqua(_aqua),
	m_gp(_gp),
	m_keyManager(_keyManager),
	m_sm(_sm)
{}

bool AdminEth::admin_aqua_setMining(bool _on, string const& _session)
{
	RPC_ADMIN;
	if (_on)
		m_aqua.startSealing();
	else
		m_aqua.stopSealing();
	return true;
}

Json::Value AdminEth::admin_aqua_blockQueueStatus(string const& _session)
{
	RPC_ADMIN;
	Json::Value ret;
	BlockQueueStatus bqs = m_aqua.blockQueue().status();
	ret["importing"] = (int)bqs.importing;
	ret["verified"] = (int)bqs.verified;
	ret["verifying"] = (int)bqs.verifying;
	ret["unverified"] = (int)bqs.unverified;
	ret["future"] = (int)bqs.future;
	ret["unknown"] = (int)bqs.unknown;
	ret["bad"] = (int)bqs.bad;
	return ret;
}

bool AdminEth::admin_aqua_setAskPrice(string const& _wei, string const& _session)
{
	RPC_ADMIN;
	m_gp.setAsk(jsToU256(_wei));
	return true;
}

bool AdminEth::admin_aqua_setBidPrice(string const& _wei, string const& _session)
{
	RPC_ADMIN;
	m_gp.setBid(jsToU256(_wei));
	return true;
}

Json::Value AdminEth::admin_aqua_findBlock(string const& _blockHash, string const& _session)
{
	RPC_ADMIN;
	h256 h(_blockHash);
	if (m_aqua.blockChain().isKnown(h))
		return toJson(m_aqua.blockChain().info(h));
	switch(m_aqua.blockQueue().blockStatus(h))
	{
		case QueueStatus::Ready:
			return "ready";
		case QueueStatus::Importing:
			return "importing";
		case QueueStatus::UnknownParent:
			return "unknown parent";
		case QueueStatus::Bad:
			return "bad";
		default:
			return "unknown";
	}
}

string AdminEth::admin_aqua_blockQueueFirstUnknown(string const& _session)
{
	RPC_ADMIN;
	return m_aqua.blockQueue().firstUnknown().hex();
}

bool AdminEth::admin_aqua_blockQueueRetryUnknown(string const& _session)
{
	RPC_ADMIN;
	m_aqua.retryUnknown();
	return true;
}

Json::Value AdminEth::admin_aqua_allAccounts(string const& _session)
{
	RPC_ADMIN;
	Json::Value ret;
	u256 total = 0;
	u256 pendingtotal = 0;
	Address beneficiary;
	for (auto const& address: m_keyManager.accounts())
	{
		auto pending = m_aqua.balanceAt(address, PendingBlock);
		auto latest = m_aqua.balanceAt(address, LatestBlock);
		Json::Value a;
		if (address == beneficiary)
			a["beneficiary"] = true;
		a["address"] = toJS(address);
		a["balance"] = toJS(latest);
		a["nicebalance"] = formatBalance(latest);
		a["pending"] = toJS(pending);
		a["nicepending"] = formatBalance(pending);
		ret["accounts"][m_keyManager.accountName(address)] = a;
		total += latest;
		pendingtotal += pending;
	}
	ret["total"] = toJS(total);
	ret["nicetotal"] = formatBalance(total);
	ret["pendingtotal"] = toJS(pendingtotal);
	ret["nicependingtotal"] = formatBalance(pendingtotal);
	return ret;
}

Json::Value AdminEth::admin_aqua_newAccount(Json::Value const& _info, string const& _session)
{
	RPC_ADMIN;
	if (!_info.isMember("name"))
		throw jsonrpc::JsonRpcException("No member found: name");
	string name = _info["name"].asString();
	KeyPair kp = KeyPair::create();
	h128 uuid;
	if (_info.isMember("password"))
	{
		string password = _info["password"].asString();
		string hint = _info["passwordHint"].asString();
		uuid = m_keyManager.import(kp.secret(), name, password, hint);
	}
	else
		uuid = m_keyManager.import(kp.secret(), name);
	Json::Value ret;
	ret["account"] = toJS(kp.pub());
	ret["uuid"] = toUUID(uuid);
	return ret;
}

bool AdminEth::admin_aqua_setMiningBenefactor(string const& _uuidOrAddress, string const& _session)
{
	RPC_ADMIN;
	return miner_setEtherbase(_uuidOrAddress);
}

Json::Value AdminEth::admin_aqua_inspect(string const& _address, string const& _session)
{
	RPC_ADMIN;
	if (!isHash<Address>(_address))
		throw jsonrpc::JsonRpcException("Invalid address given.");
	
	Json::Value ret;
	auto h = Address(fromHex(_address));
	ret["storage"] = toJson(m_aqua.storageAt(h, PendingBlock));
	ret["balance"] = toJS(m_aqua.balanceAt(h, PendingBlock));
	ret["nonce"] = toJS(m_aqua.countAt(h, PendingBlock));
	ret["code"] = toJS(m_aqua.codeAt(h, PendingBlock));
	return ret;
}

h256 AdminEth::blockHash(string const& _blockNumberOrHash) const
{
	if (isHash<h256>(_blockNumberOrHash))
		return h256(_blockNumberOrHash.substr(_blockNumberOrHash.size() - 64, 64));
	try
	{
		return m_aqua.blockChain().numberHash(stoul(_blockNumberOrHash));
	}
	catch (...)
	{
		throw jsonrpc::JsonRpcException("Invalid argument");
	}
}

Json::Value AdminEth::admin_aqua_reprocess(string const& _blockNumberOrHash, string const& _session)
{
	RPC_ADMIN;
	Json::Value ret;
	PopulationStatistics ps;
	m_aqua.block(blockHash(_blockNumberOrHash), &ps);
	ret["enact"] = ps.enact;
	ret["verify"] = ps.verify;
	ret["total"] = ps.verify + ps.enact;
	return ret;
}

Json::Value AdminEth::admin_aqua_vmTrace(string const& _blockNumberOrHash, int _txIndex, string const& _session)
{
	RPC_ADMIN;
	
	Json::Value ret;
	
	if (_txIndex < 0)
		throw jsonrpc::JsonRpcException("Negative index");
	Block block = m_aqua.block(blockHash(_blockNumberOrHash));
	if ((unsigned)_txIndex < block.pending().size())
	{
		Transaction t = block.pending()[_txIndex];
		State s(State::Null);
		Executive e(s, block, _txIndex, m_aqua.blockChain());
		try
		{
			StandardTrace st;
			st.setShowMnemonics();
			e.initialize(t);
			if (!e.execute())
				e.go(st.onOp());
			e.finalize();
			Json::Reader().parse(st.json(), ret);
		}
		catch(Exception const& _e)
		{
			cwarn << diagnostic_information(_e);
		}
	}
	
	return ret;
}

Json::Value AdminEth::admin_aqua_getReceiptByHashAndIndex(string const& _blockNumberOrHash, int _txIndex, string const& _session)
{
	RPC_ADMIN;
	if (_txIndex < 0)
		throw jsonrpc::JsonRpcException("Negative index");
	auto h = blockHash(_blockNumberOrHash);
	if (!m_aqua.blockChain().isKnown(h))
		throw jsonrpc::JsonRpcException("Invalid/unknown block.");
	auto rs = m_aqua.blockChain().receipts(h);
	if ((unsigned)_txIndex >= rs.receipts.size())
		throw jsonrpc::JsonRpcException("Index too large.");
	return toJson(rs.receipts[_txIndex]);
}

bool AdminEth::miner_start(int)
{
	m_aqua.startSealing();
	return true;
}

bool AdminEth::miner_stop()
{
	m_aqua.stopSealing();
	return true;
}

bool AdminEth::miner_setEtherbase(string const& _uuidOrAddress)
{
	Address a;
	h128 uuid = fromUUID(_uuidOrAddress);
	if (uuid)
		a = m_keyManager.address(uuid);
	else if (isHash<Address>(_uuidOrAddress))
		a = Address(_uuidOrAddress);
	else
		throw jsonrpc::JsonRpcException("Invalid UUID or address");

	if (m_setMiningBenefactor)
		m_setMiningBenefactor(a);
	else
		m_aqua.setAuthor(a);
	return true;
}

bool AdminEth::miner_setExtra(string const& _extraData)
{
	m_aqua.setExtraData(asBytes(_extraData));
	return true;
}

bool AdminEth::miner_setGasPrice(string const& _gasPrice)
{
	m_gp.setAsk(jsToU256(_gasPrice));
	return true;
}

string AdminEth::miner_hashrate()
{
	EthashClient const* client = nullptr;
	try
	{
		client = asEthashClient(&m_aqua);
	}
	catch (...)
	{
		throw jsonrpc::JsonRpcException("Hashrate not available - blockchain does not support mining.");
	}
	return toJS(client->hashrate());
}
