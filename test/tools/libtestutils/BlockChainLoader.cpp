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
/** @file BlockChainLoader.cpp
 * @author Marek Kotewicz <marek@aquadev.com>
 * @date 2015
 */

#include <json/writer.h>
#include <libaquahashseal/Ethash.h>
#include <libaquachain/BlockChain.h>
#include "BlockChainLoader.h"
#include "Common.h"
using namespace std;
using namespace dev;
using namespace dev::test;
using namespace dev::aqua;

BlockChainLoader::BlockChainLoader(Json::Value const& _json, aqua::Network _sealEngineNetwork):
	m_block(Block::Null)
{
	// load genesisBlock
	bytes genesisBlock = fromHex(_json["genesisRLP"].asString());

	Json::FastWriter a;
	m_bc.reset(new BlockChain(ChainParams(genesisInfo(_sealEngineNetwork), genesisBlock, jsonToAccountMap(a.write(_json["pre"]))), m_dir.path(), WithExisting::Kill));

	// load pre state
	m_block = m_bc->genesisBlock(State::openDB(m_dir.path(), m_bc->genesisHash(), WithExisting::Kill));

	assert(m_block.rootHash() == m_bc->info().stateRoot());

	// load blocks
	for (auto const& block: _json["blocks"])
	{
		bytes rlp = fromHex(block["rlp"].asString());
		m_bc->import(rlp, state().db());
	}

	// sync state
	m_block.sync(*m_bc);
}
