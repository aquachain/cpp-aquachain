
*****************************************************
Using Testaqua
*****************************************************

Aquachain cpp-client testaqua tool for creation and execution of aquachain tests.

To run tests you should open folder (see also `Installing and building <https://github.com/aquachain/cpp-aquachain#building-from-source>`_)

   ``/build/test``

and execute a command:
 
   ``./testaqua`` 
   
This will run all test cases automatically.
By default `testaqua` will look for the test repository cloned in cpp-aquachain submodule ``cpp-aquachain/test/jsontests`` assuming that the build folder is ``cpp-aquachain/build``

If environment variable ``ETHEREUM_TEST_PATH`` is set in /etc/environment file, ``testaqua`` will use path to the test repo from that variable. Example:

|    ``nano /etc/environment``
|    ``ETHEREUM_TEST_PATH="/home/user/aquachain/tests"``

You could always override the test path for testaqua using an option:

   ``./testaqua -- --testpath "/path/to/the/tests"``
   
Note that --testpath option argument should be an absolute path.
For a brief help on testaqua command options make sure to run 

   ``./testaqua --help``


Running a specific test case
--------------------------------------------------------------------------------

To run a specific test case you could use parameter ``-t`` in the command line option:

    ``./testaqua -t <TEST_SUITE>/<TEST_CASE>``

Or just the test suite:

   ``./testaqua -t <TEST_SUITE>``
   
To run a specific test from the test case:

   ``./testaqua -t <TEST_SUITE>/<TEST_CASE> -- --singletest <TEST_NAME>``
   
Tests has cases designed for different network rules. Such as initial frontier rules, homestead rules and other fork updates. That is to make sure that your client could sync up from the very begining to the recent top block. Block fork numbers are declared in genesis config in the file:

https://github.com/aquachain/cpp-aquachain/blob/develop/libaquahashseal/genesis/mainNetwork.cpp

If you need to debug a specific test on a specific network rules use this command:

   ``./testaqua -t <TEST_SUITE>/<TEST_CASE> -- --singletest <TEST_NAME> --singlenet <NET_NAME>``
   
Currently network names <NET_NAME> are following: Frontier, Homestead, EIP150, EIP158, Byzantine, Constantinople

The main test suites are <TEST_SUITE>: GeneralStateTests, BlockchainTests, TransitionTests, TransactionTests, VMTests

<TEST_CASE> correspond to a folder name in the tests repo. And <TEST_NAME> correspond to the filename in that folder describing a specific test. 

GeneralStateTests has a single transaction being executed on a given pre state to produce a post state. 
This transaction has arrays <data>, <value>, <gasLimit>. So a single test execute transaction with different arguments taken from those arrays to test different conditions on the same pre state. To run a transaction from the GeneralStateTests with the specified arguments use: 

   ``./testaqua -t <TEST_SUITE>/<TEST_CASE> -- --singletest <TEST_NAME> --singlenet <NET_NAME> -d <DATA_INDEX> -g <GASLIMIT_INDEX> -v <VALUE_INDEX>``
   
This will run a transaction with given data, gasLimit, and value as described in the test on a given network rules. Note that parameters here are indexes. The actual values described in the test file itself. This is only valid when <TEST_SUITE> is GeneralStateTests. 
 
Debugging and tracing a state test
--------------------------------------------------------------------------------

``testaqua`` has debugging options for getting a step by step execution log from the EVM. 
Use following options:

   ``./testaqua -t <TEST_SUITE>/<TEST_CASE> -- --vmtrace --verbosity 5``
   
``--vmtrace`` prints a step by step execution log from the EVM. Make sure that you've run cmake with -DVMTRACE=1 flag.

   ``./testaqua -t <TEST_SUITE>/<TEST_CASE> -- --jsontrace <CONFIG>``
   
An rpc maquaod like, providing step by step debug in json format. The <CONFIG> is in json format like following: 

   ``./testaqua -t <TEST_SUITE>/<TEST_CASE> -- --jsontrace '{ "disableStorage" : false, "disableMemory" : false, "disableStack" : false, "fullStorage" : true }' ``
   
Or just empty string to load default options.

    ``./testaqua -t <TEST_SUITE>/<TEST_CASE> -- --jsontrace '' ``
   
You could specify some of the options in this json line or use an empty argument to load default options. Sometimes you might want to disable just the memory logs or the storage logs or both cause it could provide a lot lines to the final log.

    ``./testaqua -t <TEST_SUITE>/<TEST_CASE> -- --statediff``
    
Get a statediff of a pre -> post state in general state test. Use this to see what accounts has changed after executing a transaction. This options should better be used in combination with ``--singletest <>`` ``--singlenet <>`` and ``-d -v -g`` (if any)
   
The option --exectimelog will print the stats on how much time was spend on a specific test suite/case. It will also sort the most time consuming test at the end of the execution. 
   
   ``./testaqua -- --exectimelog``
   
Note that some tests are disabled by default. Such as Frontier, Homestead rules tests, some time consuming tests. If you want to run a full test suite with all tests available use option --all:
   
   ``./testaqua -- --all``


Generating(Filling) the tests
--------------------------------------------------------------------------------

Tests are generated from test filler files located in the src folder of the test repo. Testaqua will run the execution of a *Filler.json file and produce a final test in the repo.
``--filltests`` option will rerun test creation. .json files will be overwritten, hashes recalculated and a fresh build info will be added to the tests.  

    ``./testaqua -t <TEST_SUITE>/<TEST_CASE> --filltests --checkstate --all``

If some test case has different results than it is expected to have (an expect section is specified in the *Filler.json file) then you will see an error in the cmd log. This error indicates that somaquaing went different and the post state does not match the expect section. So you should check the test and it's expect section. Generating a test case and creating new tests is rather a whole new topic and it's described in more detail here: 

   https://github.com/aquachain/cpp-aquachain/blob/develop/doc/generating_tests.rst
