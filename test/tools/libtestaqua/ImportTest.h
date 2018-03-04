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
/** @file
 * Helper class for managing data when running state tests
 */

#pragma once
#include <test/tools/libtestutils/Common.h>
#include <libaquahashseal/GenesisInfo.h>
#include <test/tools/libtestaqua/JsonSpiritHeaders.h>
#include <libaquachain/State.h>

namespace dev
{
namespace test
{

class ImportTest
{
public:
	ImportTest(json_spirit::mObject const& _input, json_spirit::mObject& _output);

	// imports
	void importEnv(json_spirit::mObject const& _o);
	static void importState(json_spirit::mObject const& _o, aqua::State& _state);
	static void importState(json_spirit::mObject const& _o, aqua::State& _state, aqua::AccountMaskMap& o_mask);
	static void importTransaction (json_spirit::mObject const& _o, aqua::Transaction& o_tr);
	void importTransaction(json_spirit::mObject const& _o);
	static json_spirit::mObject makeAllFieldsHex(json_spirit::mObject const& _o, bool _isHeader = false);
    static void parseJsonStrValueIntoSet(
        json_spirit::mValue const& _json, std::set<std::string>& _out);

    enum testType
    {
        StateTest,
        BlockchainTest
    };
    static std::set<aqua::Network> getAllNetworksFromExpectSections(
        json_spirit::mArray const& _expects, testType _testType);


    //check functions
	//check that networks in the vector are allowed
    static void checkAllowedNetwork(std::string const& _network);
    static void checkAllowedNetwork(std::set<std::string> const& _networks);
    static void checkBalance(aqua::State const& _pre, aqua::State const& _post, bigint _miningReward = 0);

    bytes executeTest(bool _isFilling);
    int exportTest();
	static int compareStates(aqua::State const& _stateExpect, aqua::State const& _statePost, aqua::AccountMaskMap const _expectedStateOptions = aqua::AccountMaskMap(), WhenError _throw = WhenError::Throw);
	bool checkGeneralTestSection(json_spirit::mObject const& _expects, std::vector<size_t>& _errorTransactions, std::string const& _network="") const;
	void traceStateDiff();

	aqua::State m_statePre;
	aqua::State m_statePost;

private:
	using ExecOutput = std::pair<aqua::ExecutionResult, aqua::TransactionReceipt>;
	std::tuple<aqua::State, ExecOutput, aqua::ChangeLog> executeTransaction(aqua::Network const _sealEngineNetwork, aqua::EnvInfo const& _env, aqua::State const& _preState, aqua::Transaction const& _tr);

	std::unique_ptr<aqua::LastBlockHashesFace const> m_lastBlockHashes;
	std::unique_ptr<aqua::EnvInfo> m_envInfo;
	aqua::Transaction m_transaction;

	//General State Tests
	struct transactionToExecute
	{
		transactionToExecute(int d, int g, int v, aqua::Transaction const& t):
			dataInd(d), gasInd(g), valInd(v), transaction(t), postState(0), netId(aqua::Network::MainNetwork),
			output(std::make_pair(aqua::ExecutionResult(), aqua::TransactionReceipt(h256(), u256(), aqua::LogEntries()))) {}
		int dataInd;
		int gasInd;
		int valInd;
		aqua::Transaction transaction;
		aqua::State postState;
		aqua::ChangeLog changeLog;
		aqua::Network netId;
		ExecOutput output;
	};
	std::vector<transactionToExecute> m_transactions;
	using StateAndMap = std::pair<aqua::State, aqua::AccountMaskMap>;
	using TrExpectSection = std::pair<transactionToExecute, StateAndMap>;
	bool checkGeneralTestSectionSearch(json_spirit::mObject const& _expects, std::vector<size_t>& _errorTransactions, std::string const& _network = "", TrExpectSection* _search = NULL) const;

    /// Create blockchain test fillers for specified _networks and test information (env, pre, txs)
    /// of Importtest then fill blockchain fillers into tests.
    void makeBlockchainTestFromStateTest(std::set<aqua::Network> const& _networks) const;

    json_spirit::mObject const& m_testInputObject;
	json_spirit::mObject& m_testOutputObject;
};

template<class T>
bool inArray(std::vector<T> const& _array, const T& _val)
{
	for (auto const& obj: _array)
		if (obj == _val)
			return true;
	return false;
}

} //namespace test
} //namespace dev
