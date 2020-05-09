aux_source_directory(. _APP_FILES)
add_library(app STATIC ${_APP_FILES})

target_link_libraries(app PUBLIC lib)
