add_executable(aquakey KeyAux.h main.cpp)
target_link_libraries(aquakey PRIVATE aquacore devcore Boost::program_options)
