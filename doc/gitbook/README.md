cpp-aquachain guide
======= 

This book is intended as a practical user guide for the cpp-aquachain software distribution.

cpp-aquachain is a distribution of software including a number of diverse tools. This book begins with the installation instructions, before proceeding to introductions, walk-throughs and references for the various tools that make up cpp-aquachain.

The full software suite of cpp-aquachain includes:

- **aqua** (`aqua`) The mainline CLI Aquachain client. Run it in the background and it will connect to the Aquachain network; you can mine, make transactions and inspect the blockchain.
- `aquakey` A key/wallet management tool for Aquachain keys. This lets you add, remove and change your keys as well as *cold wallet device*-friendly transaction inspection and signing.
- `aquaminer` A standalone miner. This can be used to check how fast you can mine and will mine for you in concert with `aqua`, `gaqua` and `pyaquachain`.
- `aquavm` The Aquachain virtual machine emulator. You can use this to run EVM code.
- `rlp` An serialisation/deserialisation tool for the Recursive Length Prefix format.
