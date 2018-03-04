add_executable(aquavm main.cpp)

target_link_libraries(aquavm PRIVATE aquachain evm aquahashseal devcore Boost::program_options)

target_include_directories(aquavm PRIVATE ../utils)

install(TARGETS aquavm DESTINATION bin OPTIONAL)
