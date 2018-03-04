#!/usr/bin/env bash

#------------------------------------------------------------------------------
# Bash script to build cpp-aquachain within TravisCI.
#
# The documentation for cpp-aquachain is hosted at http://cpp-aquachain.org
#
# ------------------------------------------------------------------------------
# This file is part of cpp-aquachain.
#
# cpp-aquachain is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# cpp-aquachain is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with cpp-aquachain.  If not, see <http://www.gnu.org/licenses/>
#
# (c) 2016 cpp-aquachain contributors.
#------------------------------------------------------------------------------

set -e -x

# There is an implicit assumption here that we HAVE to run from repo root

mkdir -p build
cd build
if [ $(uname -s) == "Linux" ]; then
#	Disabling while llvm-3.9 package is broken
#    cmake .. -DCMAKE_BUILD_TYPE=$1 -DCOVERAGE=ON -DEVMJIT=ON -DLLVM_DIR=/usr/lib/llvm-3.9/lib/cmake/llvm
    cmake .. -DCMAKE_BUILD_TYPE=$1 -DCOVERAGE=ON
else
    cmake .. -DCMAKE_BUILD_TYPE=$1 -DCOVERAGE=ON
fi

make -j2
