aux_source_directory(. _LIB_IO_FILES)
add_library(lib_io STATIC ${_LIB_IO_FILES})

target_include_directories(
  lib_io PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
target_link_libraries(lib_io PUBLIC lib)
