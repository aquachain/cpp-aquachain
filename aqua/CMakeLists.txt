set(
    sources
    AccountManager.cpp AccountManager.h
    main.cpp
    MinerAux.h
)

add_executable(aqua ${sources})
target_link_libraries(
    aqua
    PRIVATE aquachain aquahashseal evm web3jsonrpc webthree devcore Boost::program_options
)

target_include_directories(aqua PRIVATE ../utils)

if(MINIUPNPC)
    target_compile_definitions(aqua PRIVATE ETH_MINIUPNPC)
endif()

install(TARGETS aqua DESTINATION bin)
