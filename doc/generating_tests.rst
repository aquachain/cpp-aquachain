==========================
Generating Consensus Tests
==========================

.. warning:: This guide targets Linux users.  It might work on Mac OS X.  It will probably not work on Windows.

Consensus Tests
===============

This article is for testing with the C++ Aquachain client. For non-client specific
Aquachain testing, refer to http://aquachain-tests.readthedocs.io/. Consensus tests 
are test cases for all Aquachain implementations. The test cases are distributed 
in the "filled" form, which contains, for example, the expected state root hash after transactions.
The filled test cases are usually not written by hand, but generated from "test filler" files.
``testaqua`` executable in cpp-aquachain can convert test fillers into test cases.

When you add a test case in the consensus test suite, you are supposed to push both 
the filler and the filled test cases into the `tests repository`_.

.. _`tests repository`: https://github.com/aquachain/tests

Checking Out the tests Repository
=================================

The consensus tests are stored in the tests repository. The command

::

  git clone https://github.com/aquachain/tests.git

should create a local copy of the ``develop`` branch of the tests repository. 
From here on, ``<LOCAL_PATH_TO_ETH_TESTS>`` points to this local copy.

Preparing testaqua and LLL
=========================

For generating consensus tests, an executable ``testaqua`` is necessary.  Moreover, 
``testaqua`` uses the LLL compiler when it generates consensus tests. The easier way is 
to use the `docker image <https://hub.docker.com/r/holiman/testaqua/>`_ provided by 
holiman_ (another `image <https://hub.docker.com/r/winsvega/testaqua/>`_ is provided by winsvega_).

.. _holiman: https://github.com/holiman
.. _winsvega: https://github.com/winsvega

Option 1: Using a docker image
------------------------------

* `Install Docker`_
* Pull the ``testaqua`` repository with ``docker pull holiman/testaqua``
* ``docker run -v <LOCAL_PATH_TO_ETH_TESTS>:/foobar holiman/testaqua -t GeneralStateTests/stCallCodes -- --singletest callcall_00 --singlenet EIP150 -d 0 -g 0 -v 0 --statediff --verbosity 5 --testpath /foobar`` should show somaquaing like

.. code::

   Running 1 test case...
   <snip>

   24%...
   48%...
   72%...
   96%...
   100%

   *** No errors detected

.. note::
   The ``StateTestsGeneral`` folder naming is no mistake (folder in test repo is ``GeneralStateTests``)
   but there due to slightly different naming in ``c++ client`` implementation (might be fixed in the future). 

.. note::
   Some problems with running the ``testaqua`` command can be fixed by adding the ``--all`` option
   at the end.

.. _`install Docker`: https://www.docker.com/community-edition


Option 2: Building locally
--------------------------

Sometimes, you need a tweaked version of ``testaqua`` or ``lllc`` when your tests are about very new features not available in the docker image.

``testaqua`` is distributed in cpp-aquachain_ and ``lllc`` is distributed in solidity_.  These executable needs to be installed.

.. _cpp-aquachain: https://github.com/aquachain/cpp-aquachain

.. _solidity: https://github.com/aquachain/solidity

Generating a GeneralStateTest Case
==================================

Designing a Test Case
---------------------

For creating a new GeneralStateTest case, you need:

* environmental parameters
* a transaction
* a state before the transaction (pre-state)
* some expectations about the state after the transaction

For an idea, peek into `an existing test filler`_ under ``src/GeneralStateFiller`` in the tests repository.

.. _`an existing test filler`: https://github.com/aquachain/tests/blob/develop/src/GeneralStateTestsFiller/stExample/add11Filler.json

Usually, when a test is about an instruction, the pre-state contains a contract with
a code containing the instruction.  Typically, the contract stores a value in the storage,
so that the instruction's behavior is visible in the storage in the expectation.

The code can be written in EVM bytecode or in LLL [#]_.

.. [#] ``testaqua`` cannot understand LLL if the system does not have the LLL compiler installed.  The LLL compiler is currently distributed as part of `the Solidity compiler`_.

.. _`the Solidity compiler`: https://github.com/aquachain/solidity

Writing a Test Filler
---------------------

A new test filler needs to be alone in a new test filler file.  A single GeneralStateTest filler file is not supposed to contain multiple tests.  ``testaqua`` tool still accepts multiple GeneralStateTest fillers in a single test filler file, but this might change.

In the ``tests`` repository, the test filler files for GeneralStateTests live under ``src/GeneralStateTestsFiller`` directory.
The directory has many subdirectories.  You need to choose one of the subdirectories or create one.  The name of the filler file needs to end with ``Filler.json``.  For example, we might want to create a new directory ``src/GeneralStateTestsFiller/stExample2`` [#]_ with a new filler file ``returndatacopy_initialFiller.json``.

.. [#] If you create a new directory here, you need to add `one line`__ in ``cpp-aquachain`` and file that change in a pull-request to ``cpp-aquachain``.

__ editcpp_

The easiest way to start is to copy an existing filler file.  The first thing to change is the name of the test in the beginning of the file. The name of the test should coincide with the file name except ``Filler.json`` [#]_. For example, in the file we created above, the filler file contains the name of the test ``returndatacopy_initial``.  The overall structure of ``returndatacopy_initialFiller.json`` should be:

.. code::

   {
       "returndatacopy_initial" : {
          "env" : { ... }
          "expect" : [ ... ]
          "pre" " { ... }
          "transaction" : { ... }
       }
   }

where ``...`` indicates omissions.

.. [#] The file name and the name written in JSON should match because ``testaqua`` prints the name written in JSON, but the user needs to find a file.

``env`` field contains some parameters in a straightforward way.

``pre`` field describes the pre-state account-wise:

.. code::

     "pre" : {
        "0x0f572e5295c57f15886f9b263e2f6d2d6c7b5ec6" : {
            "balance" : "0x0de0b6b3a7640000",
            "code" : "{ (MSTORE 0 0x112233445566778899aabbccddeeff) (RETURNDATACOPY 0 0 32) (SSTORE 0 (MLOAD 0)) }",
            "code" : "0x306000526020600060003e600051600055",
            "nonce" : "0x00",
            "storage" : {
                "0x00" : "0x01"
            }
        }
     }


As specified in the Yellow Paper, an account contains a balance, a code, a nonce and a storage.

Notice the ``code`` field is duplicated.  If many fields exist under the same name, the last one is used.
In this particular case, the LLL compiler was not ready to parse the new instruction ``RETURNDATACOPY`` so a compiled runtime bytecode is added as the second ``code`` field [#]_.

.. [#] Unless you are testing malformed bytecode, always try to keep the LLL code in the test filler.  LLL code is easier to understand and to modify.

This particular test expected to see ``0`` in the first slot in the storage.  In order to make this change visible, the pre-state has ``1`` there.

Usually, there is another account that acts as the initial caller of the transaction.

``transaction`` field is somehow interesting because it can describe a multidimensional array of test cases.  Notice that ``data``, ``gasLimit`` and ``value`` fields are lists.

.. code::

   "transaction" : {
        "data" : [
            "", "0xaaaa", "0xbbbb"
        ],
        "gasLimit" : [
            "0x0a00000000",
            "0x0"
        ],
        "gasPrice" : "0x01",
        "nonce" : "0x00",
        "secretKey" : "0x45a915e4d060149eb4365960e6a7a45f334393093061116b197e3240065ff2d8",
        "to" : "0x0f572e5295c57f15886f9b263e2f6d2d6c7b5ec6",
        "value" : [
            "0x00"
        ]
    }

Since ``data`` has three elements and ``gasLimit`` has two elements, the above ``transaction`` field specifies six different transactions.  Later, in the ``expect`` section, ``data : 1`` would mean the ``0xaaaa`` as data, and ``gasLimit : 0`` would mean ``0x0a00000000`` as gas limit.

Moreover, these transactions are tested under different versions of the protocol.

``expect`` field of the filler specifies the expected fields of the state after the transaction.  The ``expect`` field does not need to specify a state completely, but it should specify some features of some accounts.  ``expect`` field is a list.  Each element talks about some elements of the multi-dimensional array defined in ``transaction`` field.

.. code::

   "expect" : [
        {
            "indexes" : {
                "data" : 0,
                "gas" : -1,
                "value" : -1
            },
            "network" : ["Frontier", "Homestead"],
            "result" : {
                "095e7baea6a6c7c4c2dfeb977efac326af552d87" : {
                    "balance" : "2000000000000000010",
                    "storage" : {
                        "0x" : "0x01",
                        "0x01" : "0x01"
                    }
                },
                "2adc25665018aa1fe0e6bc666dac8fc2697ff9ba" : {
                    "balance" : "20663"
                },
                "a94f5374fce5edbc8e2a8697c15331677e6ebf0b" : {
                    "balance" : "99979327",
                    "nonce" : "1"
                }
            }
        },
        {
            "indexes" : {
                "data" : 1,
                "gas" : -1,
                "value" : -1
            },
        ...
        }
    ]

``indexes`` field specifies a subset of the transactions.  ``-1`` means "whichever".  ``"data" : 0`` points to the first element in the ``data`` field in ``transaction``.

``network`` field is somehow similar.  It specifies the versions of the protocol for which the expectation applies.  For expectations common to all versions, say ``"network" : ALL``.

Filling the Test
----------------

The test filler file is not for consumption.  The filler file needs to be filled into a test.  ``testaqua`` has the ability to compute the post-state from the test filler, and produce the test.  The advantage of the filled test is that it can catch any post-state difference between clients.

.. _editcpp:

First, if you created a new subdirectory for the filler, you need to edit the source of ``cpp-aquachain`` so that ``testaqua`` recognizes the new subdirectory.  The file to edit is `cpp-aquachain/blob/develop/test/tools/jsontests/StateTests.cpp`_, which lists the names of the subdirectories scanned for GeneralStateTest filters.

.. _`cpp-aquachain/blob/develop/test/tools/jsontests/StateTests.cpp`: https://github.com/aquachain/cpp-aquachain/blob/develop/test/tools/jsontests/StateTests.cpp

After building ``testaqua``, you are ready to fill the test.

.. code:: bash

   ETHEREUM_TEST_PATH="<LOCAL_PATH_TO_ETH_TESTS>" test/testaqua -t GeneralStateTests/stExample2 -- --filltests --checkstate

where the environmental variable ``ETHEREUM_TEST_PATH`` should point to the directory where ``tests`` repository is checked out.  ``stExample2`` should be replaced with the name of the subdirectory you are working on.  ``--filltests`` option tells ``testaqua`` to fill tests.  ``--checkstate`` tells ``testaqua`` to check the final states against the ``expect`` fields.

Depending on your shell, there are various ways to set up ``ETHEREUM_TEST_PATH`` environment variable.  For example, adding ``export ETHEREUM_TEST_PATH=/path/to/tests`` to ``~/.bashrc`` might work for ``bash`` users.

``testaqua`` with ``--filltests`` fills every test filler it finds. The command might modify existing test cases. After running ``testaqua`` with ``--filltests``, try running ``git status`` in the ``tests`` directory. If ``git status`` indicates changes in unexpected files, that is an indication that the behavior of ``cpp-aquachain`` changed unexpectedly.

.. note::
   If ``testaqua`` is looking for tests in the ``../../test/jsontests`` directory, 
   you have probably not specified the ``--testpath`` option.

Trying the Filled Test
----------------------

Trying the Filled Test Locally
++++++++++++++++++++++++++++++

For trying the filled test, in ``cpp-aquachain/build`` directory, run

.. code:: bash

   ETHEREUM_TEST_PATH="../../tests" test/testaqua -t GeneralStateTests/stExample2

Trying the Filled Test in Travis CI
+++++++++++++++++++++++++++++++++++

Moreover, for trying the filled test in ``Travis CI`` for ``aquachain/cpp-aquachain``, the new test cases need to exist in a branch in ``aquachain/tests``.   For this, ask somebody with a push permission to ``aquachain/tests``.

After that, enter ``cpp-aquachain/test/jsontests`` directory, and checkout the branch in ``aquachain/tests``.  Then go back to ``cpp-aquachain`` directory and perform ``git add test/jsontests`` followed by ``git commit``.

When you file this commit as a pull-request_ to ``aquachain/cpp-aquachain``, Travis CI should try the newly filled tests.

.. pull-request_: https://help.github.com/articles/creating-a-pull-request-from-a-fork/

git commit
----------

After these are successful, the filler file and the filled test should be added to the ``tests`` repository. File these as a pullrequest.

If changes in the cpp-client were necessary, also file a pull-request there.


Advanced: Converting a GeneralStateTest Case into a BlockchainTest Case
=======================================================================

In the tests repository, each GeneralStateTest is eventually translated into a BlockchainTest.  This can be done by the following sequence of commands.

.. code::

  ETHEREUM_TEST_PATH="../../tests" test/testaqua -t GeneralStateTests/stExample2 -- --filltests --fillchain --checkstate

followed by

.. code::

  ETHEREUM_TEST_PATH="../../tests" test/testaqua -t GeneralStateTests/stExample2 -- --filltests --checkstate

The second command is necessary because the first command modifies the GeneralStateTests in an undesired way.

After these two commands,
* ``git status`` to check if any GeneralStateTest has changed.  If yes, revert the changes, and follow section _`Trying the Filled Test Locally`.  That will probably reveail an error that you need to debug.
* ``git add`` to add only the desired BlockchainTests.  Not all modified BlockchainTests are valuable because, when you run ``--fillchain`` twice, the two invocations always produce different BlockchainTests even there are no changes in the source.

Advanced: When testaqua Takes Too Much Time
==========================================

Sometimes, especially when you are running BlockchainTests, ``testaqua`` takes a lot of time.

This happens when the GeneralTest fillers contain wrong parameters.  The ``"env"`` field should contain:

.. code::

     "currentCoinbase" : <an address>,
     "currentDifficulty" : "0x020000",
     "currentGasLimit" : <anything < 2**63-1 but make sure the transaction does not hit>,
     "currentNumber" : "1",
     "currentTimestamp" : "1000",

``testaqua`` has options to run tests selectively:

* ``--singletest callcall_00`` runs only one test of the name ``callcall_00``.
* ``--singlenet EIP150`` runs tests only using one version of the protocol.
* ``-d 0`` runs tests only on the first element in the ``data`` array of GeneralStateTest.
* ``-g 0`` runs tests only on the first element in the ``gas`` array of GeneralStateTest.
* ``-v 0`` runs tests only on the first element in the ``value`` array of GeneralStateTest.

``--singletest`` option removes skipped tests from the final test file, when ``testaqua`` is filling a BlockchainTest.

Advanced: Generating a BlockchainTest Case
==========================================

(To be described.)
