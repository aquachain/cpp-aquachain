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
/** @file EthashCPUMiner.h
 * @author Gav Wood <i@gavwood.com>
 * @date 2014
 *
 * Determines the PoW algorithm.
 */

#pragma once

#include "libdevcore/Worker.h"
#include <libaquachain/GenericMiner.h>
#include "EthashAux.h"

namespace dev
{
namespace aqua
{

class EthashCPUMiner: public GenericMiner<EthashProofOfWork>, Worker
{
public:
	EthashCPUMiner(GenericMiner<EthashProofOfWork>::ConstructionInfo const& _ci);
	~EthashCPUMiner();

	static unsigned instances() { return s_numInstances > 0 ? s_numInstances : std::thread::hardware_concurrency(); }
	static std::string platformInfo();
	static void setNumInstances(unsigned _instances) { s_numInstances = std::min<unsigned>(_instances, std::thread::hardware_concurrency()); }

protected:
	void kickOff() override;
	void pause() override;

private:
	void workLoop() override;
	static unsigned s_numInstances;
};

}
}
