file(GLOB sources "*.cpp" "*.h")

add_library(aquahashseal ${sources})
target_include_directories(aquahashseal PRIVATE ../utils)
target_link_libraries(aquahashseal PUBLIC aquachain aquacore aquahash)
