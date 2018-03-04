#pragma once
#include "AdminEthFace.h"

namespace dev
{

namespace aqua
{
class Client;
class TrivialGasPricer;
class KeyManager;
}

namespace rpc
{

class SessionManager;

class AdminEth: public AdminEthFace
{
public:
	AdminEth(aqua::Client& _aqua, aqua::TrivialGasPricer& _gp, aqua::KeyManager& _keyManager, SessionManager& _sm);

	virtual RPCModules implementedModules() const override
	{
		return RPCModules{RPCModule{"admin", "1.0"}, RPCModule{"miner", "1.0"}};
	}

	virtual bool admin_aqua_setMining(bool _on, std::string const& _session) override;
	virtual Json::Value admin_aqua_blockQueueStatus(std::string const& _session) override;
	virtual bool admin_aqua_setAskPrice(std::string const& _wei, std::string const& _session) override;
	virtual bool admin_aqua_setBidPrice(std::string const& _wei, std::string const& _session) override;
	virtual Json::Value admin_aqua_findBlock(std::string const& _blockHash, std::string const& _session) override;
	virtual std::string admin_aqua_blockQueueFirstUnknown(std::string const& _session) override;
	virtual bool admin_aqua_blockQueueRetryUnknown(std::string const& _session) override;
	virtual Json::Value admin_aqua_allAccounts(std::string const& _session) override;
	virtual Json::Value admin_aqua_newAccount(const Json::Value& _info, std::string const& _session) override;
	virtual bool admin_aqua_setMiningBenefactor(std::string const& _uuidOrAddress, std::string const& _session) override;
	virtual Json::Value admin_aqua_inspect(std::string const& _address, std::string const& _session) override;
	virtual Json::Value admin_aqua_reprocess(std::string const& _blockNumberOrHash, std::string const& _session) override;
	virtual Json::Value admin_aqua_vmTrace(std::string const& _blockNumberOrHash, int _txIndex, std::string const& _session) override;
	virtual Json::Value admin_aqua_getReceiptByHashAndIndex(std::string const& _blockNumberOrHash, int _txIndex, std::string const& _session) override;
	virtual bool miner_start(int _threads) override;
	virtual bool miner_stop() override;
	virtual bool miner_setEtherbase(std::string const& _uuidOrAddress) override;
	virtual bool miner_setExtra(std::string const& _extraData) override;
	virtual bool miner_setGasPrice(std::string const& _gasPrice) override;
	virtual std::string miner_hashrate() override;

	virtual void setMiningBenefactorChanger(std::function<void(Address const&)> const& _f) { m_setMiningBenefactor = _f; }
private:
	aqua::Client& m_aqua;
	aqua::TrivialGasPricer& m_gp;
	aqua::KeyManager& m_keyManager;
	SessionManager& m_sm;
	std::function<void(Address const&)> m_setMiningBenefactor;

	h256 blockHash(std::string const& _blockNumberOrHash) const;
};

}
}
