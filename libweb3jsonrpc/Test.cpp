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
/** @file Test.cpp
 * @authors:
 *   Dimitry Khokhlov <dimitry@aquadev.com>
 * @date 2016
 */

#include "Test.h"
#include <jsonrpccpp/common/errors.h>
#include <jsonrpccpp/common/exception.h>
#include <libaquachain/ClientTest.h>
#include <libaquachain/ChainParams.h>

using namespace std;
using namespace dev;
using namespace dev::rpc;
using namespace jsonrpc;

Test::Test(aqua::Client& _aqua): m_aqua(_aqua) {}

bool Test::test_setChainParams(Json::Value const& param1)
{
	try
	{
		Json::FastWriter fastWriter;
		std::string output = fastWriter.write(param1);
		asClientTest(m_aqua).setChainParams(output);
	}
	catch (std::exception const&)
	{
		BOOST_THROW_EXCEPTION(JsonRpcException(Errors::ERROR_RPC_INTERNAL_ERROR));
	}

	return true;
}

bool Test::test_mineBlocks(int _number)
{
	try
	{
		asClientTest(m_aqua).mineBlocks(_number);
	}
	catch (std::exception const&)
	{
		BOOST_THROW_EXCEPTION(JsonRpcException(Errors::ERROR_RPC_INTERNAL_ERROR));
	}

	return true;
}

bool Test::test_modifyTimestamp(int _timestamp)
{
	// FIXME: Fix year 2038 issue.
	try
	{
		asClientTest(m_aqua).modifyTimestamp(_timestamp);
	}
	catch (std::exception const&)
	{
		BOOST_THROW_EXCEPTION(JsonRpcException(Errors::ERROR_RPC_INTERNAL_ERROR));
	}
	return true;
}

bool Test::test_addBlock(std::string const& _rlp)
{
	try
	{
		asClientTest(m_aqua).addBlock(_rlp);
	}
	catch (std::exception const&)
	{
		BOOST_THROW_EXCEPTION(JsonRpcException(Errors::ERROR_RPC_INTERNAL_ERROR));
	}
	return true;
}

bool Test::test_rewindToBlock(int _number)
{
	try
	{
		m_aqua.rewind(_number);
		asClientTest(m_aqua).completeSync();
	}
	catch (std::exception const&)
	{
		BOOST_THROW_EXCEPTION(JsonRpcException(Errors::ERROR_RPC_INTERNAL_ERROR));
	}
	return true;
}
