file(GLOB sources "*.cpp" "*.h")

add_library(aquachain ${sources})

target_include_directories(aquachain PRIVATE "${UTILS_INCLUDE_DIR}")
target_link_libraries(aquachain PUBLIC evm aquacore p2p devcrypto devcore PRIVATE jsoncpp_lib_static Snappy::snappy Boost::fiber Boost::context)
