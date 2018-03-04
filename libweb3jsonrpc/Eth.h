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
/** @file Eth.h
 * @authors:
 *   Gav Wood <i@gavwood.com>
 *   Marek Kotewicz <marek@aquadev.com>
 * @date 2014
 */

#pragma once

#include <memory>
#include <iosfwd>
#include <jsonrpccpp/server.h>
#include <jsonrpccpp/common/exception.h>
#include <libdevcore/Common.h>
#include "SessionManager.h"
#include "EthFace.h"


namespace dev
{
class NetworkFace;
class KeyPair;
namespace aqua
{
class AccountHolder;
struct TransactionSkeleton;
class Interface;
}

}

namespace dev
{

namespace rpc
{

/**
 * @brief JSON-RPC api implementation
 */
class Eth: public dev::rpc::EthFace
{
public:
	Eth(aqua::Interface& _aqua, aqua::AccountHolder& _aquaAccounts);

	virtual RPCModules implementedModules() const override
	{
		return RPCModules{RPCModule{"aqua", "1.0"}};
	}

	aqua::AccountHolder const& aquaAccounts() const { return m_aquaAccounts; }

	virtual std::string aqua_protocolVersion() override;
	virtual std::string aqua_hashrate() override;
	virtual std::string aqua_coinbase() override;
	virtual bool aqua_mining() override;
	virtual std::string aqua_gasPrice() override;
	virtual Json::Value aqua_accounts() override;
	virtual std::string aqua_blockNumber()override;
	virtual std::string aqua_getBalance(std::string const& _address, std::string const& _blockNumber) override;
	virtual std::string aqua_getStorageAt(std::string const& _address, std::string const& _position, std::string const& _blockNumber) override;
	virtual std::string aqua_getStorageRoot(std::string const& _address, std::string const& _blockNumber) override;
	virtual std::string aqua_getTransactionCount(std::string const& _address, std::string const& _blockNumber) override;
	virtual std::string aqua_pendingTransactions() override;
	virtual Json::Value aqua_getBlockTransactionCountByHash(std::string const& _blockHash) override;
	virtual Json::Value aqua_getBlockTransactionCountByNumber(std::string const& _blockNumber) override;
	virtual Json::Value aqua_getUncleCountByBlockHash(std::string const& _blockHash) override;
	virtual Json::Value aqua_getUncleCountByBlockNumber(std::string const& _blockNumber) override;
	virtual std::string aqua_getCode(std::string const& _address, std::string const& _blockNumber) override;
	virtual std::string aqua_sendTransaction(Json::Value const& _json) override;
	virtual std::string aqua_call(Json::Value const& _json, std::string const& _blockNumber) override;
	virtual std::string aqua_estimateGas(Json::Value const& _json) override;
	virtual bool aqua_flush() override;
	virtual Json::Value aqua_getBlockByHash(std::string const& _blockHash, bool _includeTransactions) override;
	virtual Json::Value aqua_getBlockByNumber(std::string const& _blockNumber, bool _includeTransactions) override;
	virtual Json::Value aqua_getTransactionByHash(std::string const& _transactionHash) override;
	virtual Json::Value aqua_getTransactionByBlockHashAndIndex(std::string const& _blockHash, std::string const& _transactionIndex) override;
	virtual Json::Value aqua_getTransactionByBlockNumberAndIndex(std::string const& _blockNumber, std::string const& _transactionIndex) override;
	virtual Json::Value aqua_getTransactionReceipt(std::string const& _transactionHash) override;
	virtual Json::Value aqua_getUncleByBlockHashAndIndex(std::string const& _blockHash, std::string const& _uncleIndex) override;
	virtual Json::Value aqua_getUncleByBlockNumberAndIndex(std::string const& _blockNumber, std::string const& _uncleIndex) override;
	virtual std::string aqua_newFilter(Json::Value const& _json) override;
	virtual std::string aqua_newFilterEx(Json::Value const& _json) override;
	virtual std::string aqua_newBlockFilter() override;
	virtual std::string aqua_newPendingTransactionFilter() override;
	virtual bool aqua_uninstallFilter(std::string const& _filterId) override;
	virtual Json::Value aqua_getFilterChanges(std::string const& _filterId) override;
	virtual Json::Value aqua_getFilterChangesEx(std::string const& _filterId) override;
	virtual Json::Value aqua_getFilterLogs(std::string const& _filterId) override;
	virtual Json::Value aqua_getFilterLogsEx(std::string const& _filterId) override;
	virtual Json::Value aqua_getLogs(Json::Value const& _json) override;
	virtual Json::Value aqua_getLogsEx(Json::Value const& _json) override;
	virtual Json::Value aqua_getWork() override;
	virtual bool aqua_submitWork(std::string const& _nonce, std::string const&, std::string const& _mixHash) override;
	virtual bool aqua_submitHashrate(std::string const& _hashes, std::string const& _id) override;
	virtual std::string aqua_register(std::string const& _address) override;
	virtual bool aqua_unregister(std::string const& _accountId) override;
	virtual Json::Value aqua_fetchQueuedTransactions(std::string const& _accountId) override;
	virtual std::string aqua_signTransaction(Json::Value const& _transaction) override;
	virtual Json::Value aqua_inspectTransaction(std::string const& _rlp) override;
	virtual std::string aqua_sendRawTransaction(std::string const& _rlp) override;
	virtual bool aqua_notePassword(std::string const&) override { return false; }
	virtual Json::Value aqua_syncing() override;
	
	void setTransactionDefaults(aqua::TransactionSkeleton& _t);
protected:

	aqua::Interface* client() { return &m_aqua; }
	
	aqua::Interface& m_aqua;
	aqua::AccountHolder& m_aquaAccounts;

};

}
} //namespace dev
