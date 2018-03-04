#!/usr/bin/env bash
# author: Lefteris Karapetsas <lefteris@refu.co>
#
# Just upload the generated .rb file to homebrew aquachain

echo ">>> Starting the script to upload .rb file to homebrew aquachain"
rm -rf homebrew-aquachain
git clone git@github.com:aquachain/homebrew-aquachain.git
cp webthree-umbrella/build/cpp-aquachain.rb homebrew-aquachain
cd homebrew-aquachain
git add . -u
git commit -m "update cpp-aquachain.rb"
git push origin
cd ..
rm -rf homebrew-aquachain
echo ">>> Succesfully uploaded the .rb file to homebrew aquachain"
