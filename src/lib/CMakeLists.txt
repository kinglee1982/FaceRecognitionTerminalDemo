aux_source_directory(. _LIB_FILES)
add_library(lib STATIC ${_LIB_FILES})

target_include_directories(lib
                           PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>)
target_link_libraries(lib PUBLIC faceServerShared freetype)
