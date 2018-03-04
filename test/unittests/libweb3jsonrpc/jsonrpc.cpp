/*
	This file is part of cpp-aquachain.

	cpp-aquachain is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	cpp-aquachain is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with cpp-aquachain.  If not, see <http://www.gnu.org/licenses/>.
*/
/** @file jsonrpc.cpp
 * @author Marek Kotewicz <marek@aquadev.com>
 * @date 2014
 */

//FIXME:
// @debris disabled as tests fail with:
// unknown location(0): fatal error in "jsonrpc_setMining": std::exception: Exception -32003 : Client connector error: : libcurl error: 28
// /home/gav/Eth/cpp-aquachain/test/jsonrpc.cpp(169): last checkpoint
#if ETH_JSONRPC && 0

#include <boost/test/unit_test.hpp>
#include <boost/lexical_cast.hpp>
#include <libdevcore/Log.h>
#include <libdevcore/CommonIO.h>
#include <libaquacore/CommonJS.h>
#include <libwebthree/WebThree.h>
#include <libweb3jsonrpc/WebThreeStubServer.h>
#include <jsonrpccpp/server/connectors/httpserver.h>
#include <set>
#include "../JsonSpiritHeaders.h"
#include <test/tools/libtestaqua/TestOutputHelper.h>
#include "WebThreeStubClient.h"

BOOST_FIXTURE_TEST_SUITE(jsonrpc, TestOutputHelper)

using namespace std;
using namespace dev;
using namespace dev::aqua;
namespace js = json_spirit;

WebThreeDirect* web3;
unique_ptr<WebThreeStubServer> jsonrpcServer;
unique_ptr<WebThreeStubClient> jsonrpcClient;

struct Setup
{
	Setup()
	{
		static bool setup = false;
		if (setup)
			return;
		setup = true;

		dev::p2p::NetworkPreferences nprefs(30303, std::string(), false);
		web3 = new WebThreeDirect("aqua tests", "", true, {"aqua", "shh"}, nprefs);
		
		web3->setIdealPeerCount(5);
		web3->aquachain()->setForceMining(true);
		auto server = new jsonrpc::HttpServer(8080);
		jsonrpcServer = unique_ptr<WebThreeStubServer>(new WebThreeStubServer(*server, *web3, {}));
		jsonrpcServer->setIdentities({});
		jsonrpcServer->StartListening();
		auto client = new jsonrpc::HttpClient("http://localhost:8080");
		jsonrpcClient = unique_ptr<WebThreeStubClient>(new WebThreeStubClient(*client));
	}
};

string fromAscii(string _s)
{
	bytes b = asBytes(_s);
	return toHexPrefixed(b);
}

BOOST_FIXTURE_TEST_SUITE(environment, Setup)

BOOST_AUTO_TEST_CASE(jsonrpc_defaultBlock)
{
	cnote << "Testing jsonrpc defaultBlock...";
	int defaultBlock = jsonrpcClient->aqua_defaultBlock();
	BOOST_CHECK_EQUAL(defaultBlock, web3->aquachain()->getDefault());
}

BOOST_AUTO_TEST_CASE(jsonrpc_gasPrice)
{
	cnote << "Testing jsonrpc gasPrice...";
	string gasPrice = jsonrpcClient->aqua_gasPrice();
	BOOST_CHECK_EQUAL(gasPrice, toJS(10 * dev::aqua::szabo));
}

BOOST_AUTO_TEST_CASE(jsonrpc_isListening)
{
	cnote << "Testing jsonrpc isListening...";

	web3->startNetwork();
	bool listeningOn = jsonrpcClient->aqua_listening();
	BOOST_CHECK_EQUAL(listeningOn, web3->isNetworkStarted());
	
	web3->stopNetwork();
	bool listeningOff = jsonrpcClient->aqua_listening();
	BOOST_CHECK_EQUAL(listeningOff, web3->isNetworkStarted());
}

BOOST_AUTO_TEST_CASE(jsonrpc_isMining)
{
	cnote << "Testing jsonrpc isMining...";

	web3->aquachain()->startSealing();
	bool miningOn = jsonrpcClient->aqua_mining();
	BOOST_CHECK_EQUAL(miningOn, web3->aquachain()->isMining());

	web3->aquachain()->stopSealing();
	bool miningOff = jsonrpcClient->aqua_mining();
	BOOST_CHECK_EQUAL(miningOff, web3->aquachain()->isMining());
}

BOOST_AUTO_TEST_CASE(jsonrpc_accounts)
{
	cnote << "Testing jsonrpc accounts...";
	std::vector <dev::KeyPair> keys = {KeyPair::create(), KeyPair::create()};
	jsonrpcServer->setAccounts(keys);
	Json::Value k = jsonrpcClient->aqua_accounts();
	jsonrpcServer->setAccounts({});
	BOOST_CHECK_EQUAL(k.isArray(), true);
	BOOST_CHECK_EQUAL(k.size(),  keys.size());
	for (auto &i:k)
	{
		auto it = std::find_if(keys.begin(), keys.end(), [i](dev::KeyPair const& keyPair)
		{
			return jsToAddress(i.asString()) == keyPair.address();
		});
		BOOST_CHECK_EQUAL(it != keys.end(), true);
	}
}

BOOST_AUTO_TEST_CASE(jsonrpc_number)
{
	cnote << "Testing jsonrpc number2...";
	int number = jsonrpcClient->aqua_number();
	BOOST_CHECK_EQUAL(number, web3->aquachain()->number() + 1);
	dev::aqua::mine(*(web3->aquachain()), 1);
	int numberAfter = jsonrpcClient->aqua_number();
	BOOST_CHECK_EQUAL(number + 1, numberAfter);
	BOOST_CHECK_EQUAL(numberAfter, web3->aquachain()->number() + 1);
}

BOOST_AUTO_TEST_CASE(jsonrpc_peerCount)
{
	cnote << "Testing jsonrpc peerCount...";
	int peerCount = jsonrpcClient->aqua_peerCount();
	BOOST_CHECK_EQUAL(web3->peerCount(), peerCount);
}

BOOST_AUTO_TEST_CASE(jsonrpc_setListening)
{
	cnote << "Testing jsonrpc setListening...";

	jsonrpcClient->aqua_setListening(true);
	BOOST_CHECK_EQUAL(web3->isNetworkStarted(), true);
	
	jsonrpcClient->aqua_setListening(false);
	BOOST_CHECK_EQUAL(web3->isNetworkStarted(), false);
}

BOOST_AUTO_TEST_CASE(jsonrpc_setMining)
{
	cnote << "Testing jsonrpc setMining...";

	jsonrpcClient->aqua_setMining(true);
	BOOST_CHECK_EQUAL(web3->aquachain()->isMining(), true);

	jsonrpcClient->aqua_setMining(false);
	BOOST_CHECK_EQUAL(web3->aquachain()->isMining(), false);
}

BOOST_AUTO_TEST_CASE(jsonrpc_stateAt)
{
	cnote << "Testing jsonrpc stateAt...";
	dev::KeyPair key = KeyPair::create();
	auto address = key.address();
	string stateAt = jsonrpcClient->aqua_stateAt(toJS(address), "0");
	BOOST_CHECK_EQUAL(toJS(web3->aquachain()->stateAt(address, jsToU256("0"), 0)), stateAt);
}

BOOST_AUTO_TEST_CASE(jsonrpc_transact)
{
	cnote << "Testing jsonrpc transact...";
	string coinbase = jsonrpcClient->aqua_coinbase();
	BOOST_CHECK_EQUAL(jsToAddress(coinbase), web3->aquachain()->author());
	
	dev::KeyPair key = KeyPair::create();
	auto address = key.address();
	auto receiver = KeyPair::create();
	web3->aquachain()->setAuthor(address);

	coinbase = jsonrpcClient->aqua_coinbase();
	BOOST_CHECK_EQUAL(jsToAddress(coinbase), web3->aquachain()->author());
	BOOST_CHECK_EQUAL(jsToAddress(coinbase), address);
	
	jsonrpcServer->setAccounts({key});
	auto balance = web3->aquachain()->balanceAt(address, 0);
	string balanceString = jsonrpcClient->aqua_balanceAt(toJS(address));
	double countAt = jsonrpcClient->aqua_countAt(toJS(address));
	
	BOOST_CHECK_EQUAL(countAt, (double)(uint64_t)web3->aquachain()->countAt(address));
	BOOST_CHECK_EQUAL(countAt, 0);
	BOOST_CHECK_EQUAL(toJS(balance), balanceString);
	BOOST_CHECK_EQUAL(jsToDecimal(balanceString), "0");
	
	dev::aqua::mine(*(web3->aquachain()), 1);
	balance = web3->aquachain()->balanceAt(address, 0);
	balanceString = jsonrpcClient->aqua_balanceAt(toJS(address));
	
	BOOST_CHECK_EQUAL(toJS(balance), balanceString);
	BOOST_CHECK_EQUAL(jsToDecimal(balanceString), "1500000000000000000");
	
	auto txAmount = balance / 2u;
	auto gasPrice = 10 * dev::aqua::szabo;
	auto gas = EVMSchedule().txGas;
	
	Json::Value t;
	t["from"] = toJS(address);
	t["value"] = jsToDecimal(toJS(txAmount));
	t["to"] = toJS(receiver.address());
	t["data"] = toJS(bytes());
	t["gas"] = toJS(gas);
	t["gasPrice"] = toJS(gasPrice);
	
	jsonrpcClient->aqua_transact(t);
	jsonrpcServer->setAccounts({});
	dev::aqua::mine(*(web3->aquachain()), 1);
	
	countAt = jsonrpcClient->aqua_countAt(toJS(address));
	auto balance2 = web3->aquachain()->balanceAt(receiver.address());
	string balanceString2 = jsonrpcClient->aqua_balanceAt(toJS(receiver.address()));
	
	BOOST_CHECK_EQUAL(countAt, (double)(uint64_t)web3->aquachain()->countAt(address));
	BOOST_CHECK_EQUAL(countAt, 1);
	BOOST_CHECK_EQUAL(toJS(balance2), balanceString2);
	BOOST_CHECK_EQUAL(jsToDecimal(balanceString2), "750000000000000000");
	BOOST_CHECK_EQUAL(txAmount, balance2);
}


BOOST_AUTO_TEST_CASE(simple_contract)
{
	cnote << "Testing jsonrpc contract...";
	KeyPair kp = KeyPair::create();
	web3->aquachain()->setAuthor(kp.address());
	jsonrpcServer->setAccounts({kp});

	dev::aqua::mine(*(web3->aquachain()), 1);
	
	char const* sourceCode = "contract test {\n"
	"  function f(uint a) returns(uint d) { return a * 7; }\n"
	"}\n";

	string compiled = jsonrpcClient->aqua_solidity(sourceCode);

	Json::Value create;
	create["code"] = compiled;
	string contractAddress = jsonrpcClient->aqua_transact(create);
	dev::aqua::mine(*(web3->aquachain()), 1);
	
	Json::Value call;
	call["to"] = contractAddress;
	call["data"] = "0x00000000000000000000000000000000000000000000000000000000000000001";
	string result = jsonrpcClient->aqua_call(call);
	BOOST_CHECK_EQUAL(result, "0x0000000000000000000000000000000000000000000000000000000000000007");
}

BOOST_AUTO_TEST_CASE(contract_storage)
{
	cnote << "Testing jsonrpc contract storage...";
	KeyPair kp = KeyPair::create();
	web3->aquachain()->setAuthor(kp.address());
	jsonrpcServer->setAccounts({kp});

	dev::aqua::mine(*(web3->aquachain()), 1);
	
	char const* sourceCode = R"(
		contract test {
			uint hello;
			function writeHello(uint value)  returns(bool d){
				hello = value;
				return true;
			}
		}
	)";
	
	string compiled = jsonrpcClient->aqua_solidity(sourceCode);
	
	Json::Value create;
	create["code"] = compiled;
	string contractAddress = jsonrpcClient->aqua_transact(create);
	dev::aqua::mine(*(web3->aquachain()), 1);
	
	Json::Value transact;
	transact["to"] = contractAddress;
	transact["data"] = "0x00000000000000000000000000000000000000000000000000000000000000003";
	jsonrpcClient->aqua_transact(transact);
	dev::aqua::mine(*(web3->aquachain()), 1);
	
	Json::Value storage = jsonrpcClient->aqua_storageAt(contractAddress);
	BOOST_CHECK_EQUAL(storage.getMemberNames().size(), 1);
	// bracers are required, cause msvc couldnt handle this macro in for statement
	for (auto name: storage.getMemberNames())
	{
		BOOST_CHECK_EQUAL(storage[name].asString(), "0x03");
	}
}

BOOST_AUTO_TEST_CASE(sha3)
{
	cnote << "Testing jsonrpc sha3...";
	string testString = "multiply(uint256)";
	h256 expected = dev::sha3(testString);

	auto hexValue = fromAscii(testString);
	string result = jsonrpcClient->web3_sha3(hexValue);
	BOOST_CHECK_EQUAL(toJS(expected), result);
	BOOST_CHECK_EQUAL("0xc6888fa159d67f77c2f3d7a402e199802766bd7e8d4d1ecd2274fc920265d56a", result);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

#endif
