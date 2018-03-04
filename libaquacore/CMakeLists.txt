file(GLOB sources "*.cpp" "*.h")

add_library(aquacore ${sources})

target_include_directories(aquacore PRIVATE "${UTILS_INCLUDE_DIR}")
target_link_libraries(aquacore PUBLIC devcrypto devcore)
