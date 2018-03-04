
REM ---------------------------------------------------------------------------
REM Batch file for implementing release flow for cpp-aquachain for Windows.
REM
REM The documentation for cpp-aquachain is hosted at http://cpp-aquachain.org
REM
REM ---------------------------------------------------------------------------
REM This file is part of cpp-aquachain.
REM
REM cpp-aquachain is free software: you can redistribute it and/or modify
REM it under the terms of the GNU General Public License as published by
REM the Free Software Foundation, either version 3 of the License, or
REM (at your option) any later version.
REM
REM cpp-aquachain is distributed in the hope that it will be useful,
REM but WITHOUT ANY WARRANTY; without even the implied warranty of
REM MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
REM GNU General Public License for more details.
REM
REM You should have received a copy of the GNU General Public License
REM along with cpp-aquachain.  If not, see <http://www.gnu.org/licenses/>
REM
REM Copyright (c) 2016 cpp-aquachain contributors.
REM ---------------------------------------------------------------------------

set CONFIGURATION=%1
set ETHEREUM_DEPS_PATH=%2
set TESTS=%3

7z a cpp-aquachain-windows.zip ^
    .\build\aqua\%CONFIGURATION%\aqua.exe ^
    .\build\aquakey\%CONFIGURATION%\aquakey.exe ^
    .\build\aquavm\%CONFIGURATION%\aquavm.exe ^
    .\build\rlp\%CONFIGURATION%\rlp.exe ^
    .\build\test\%CONFIGURATION%\testaqua.exe ^
    "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\redist\x86\Microsoft.VC140.CRT\msvc*.dll"
