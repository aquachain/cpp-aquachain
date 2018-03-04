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
/** @file BlockChainLoader.h
 * @author Marek Kotewicz <marek@aquadev.com>
 * @date 2015
 */

#pragma once
#include <json/json.h>
#include <libdevcore/TransientDirectory.h>
#include <libaquachain/BlockChain.h>
#include <libaquachain/Block.h>
#include <libaquahashseal/GenesisInfo.h>

namespace dev
{
namespace test
{

/**
 * @brief Should be used to load test blockchain from json file
 * Loads the blockchain from json, creates temporary directory to store it, removes the directory on dealloc
 */
class BlockChainLoader
{
public:
	explicit BlockChainLoader(Json::Value const& _json, aqua::Network _sealEngineNetwork = aqua::Network::TransitionnetTest);
	aqua::BlockChain const& bc() const { return *m_bc; }
	aqua::State const& state() const { return m_block.state(); }	// TODO remove?
	aqua::Block const& block() const { return m_block; }

private:
	TransientDirectory m_dir;
	std::unique_ptr<aqua::BlockChain> m_bc;
	aqua::Block m_block;
};

}
}
