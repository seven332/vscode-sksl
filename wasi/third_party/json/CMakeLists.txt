cmake_path(SET SKSL_JSON_SOURCE_DIR NORMALIZE ${CMAKE_CURRENT_SOURCE_DIR}/../externals/json)
add_subdirectory(${SKSL_JSON_SOURCE_DIR} json)
target_compile_definitions(nlohmann_json INTERFACE JSON_HAS_FILESYSTEM=0)
