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
/** @file FixedClient.h
 * @author Marek Kotewicz <marek@aquadev.com>
 * @date 2015
 */

#pragma once

#include <libaquachain/ClientBase.h>
#include <libaquachain/BlockChain.h>
#include <libaquacore/Common.h>

namespace dev
{
namespace test
{

/**
 * @brief mvp implementation of ClientBase
 * Doesn't support mining interface
 */
class FixedClient: public dev::aqua::ClientBase
{
public:
	FixedClient(aqua::BlockChain const& _bc, aqua::Block const& _block) :  m_bc(_bc), m_block(_block) {}

	// stub
	void flushTransactions() override {}
	aqua::BlockChain& bc() override { BOOST_THROW_EXCEPTION(InterfaceNotSupported("FixedClient::bc()")); }
	aqua::BlockChain const& bc() const override { return m_bc; }
	using ClientBase::block;
	aqua::Block block(h256 const& _h) const override;
	aqua::Block preSeal() const override { ReadGuard l(x_stateDB); return m_block; }
	aqua::Block postSeal() const override { ReadGuard l(x_stateDB); return m_block; }
	void setAuthor(Address const& _us) override { WriteGuard l(x_stateDB); m_block.setAuthor(_us); }
	void prepareForTransaction() override {}
	std::pair<h256, Address> submitTransaction(aqua::TransactionSkeleton const&, Secret const&) override { return {}; };
	aqua::ImportResult injectTransaction(bytes const&, aqua::IfDropped) override { return {}; }
	aqua::ExecutionResult call(Address const&, u256, Address, bytes const&, u256, u256, aqua::BlockNumber, aqua::FudgeFactor) override { return {}; };

private:
	aqua::BlockChain const& m_bc;
	aqua::Block m_block;
	mutable SharedMutex x_stateDB;			///< Lock on the state DB, effectively a lock on m_postSeal.
};

}
}
