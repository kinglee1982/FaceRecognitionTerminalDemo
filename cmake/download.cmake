function(download_file url filename)
  message(STATUS "Download ${url} to ${filename} ...")
  file(DOWNLOAD ${url} ${filename} TIMEOUT 600)
endfunction()

function(download_file_with_hash url filename hash_type hash)
  message(STATUS "Download ${url} to ${filename} ...")
  file(
    DOWNLOAD ${url} ${filename}
    TIMEOUT 600 # seconds
    EXPECTED_HASH ${hash_type}=${hash})
endfunction()

function(extrat_file filename extract_dir)
  message(STATUS "Extract ${filename} to ${extract_dir} ...")

  set(temp_dir ${CMAKE_BINARY_DIR}/tmp_for_extract.dir)
  if(EXISTS ${temp_dir})
    file(REMOVE_RECURSE ${temp_dir})
  endif()
  file(MAKE_DIRECTORY ${temp_dir})
  execute_process(COMMAND ${CMAKE_COMMAND} -E tar -xf ${filename}
                  WORKING_DIRECTORY ${temp_dir})

  file(GLOB contents "${temp_dir}/*")
  list(LENGTH contents n)
  if(NOT n EQUAL 1 OR NOT IS_DIRECTORY "${contents}")
    set(contents "${temp_dir}")
  endif()

  get_filename_component(contents ${contents} ABSOLUTE)
  file(INSTALL "${contents}/" DESTINATION ${extract_dir})
  file(REMOVE_RECURSE ${temp_dir})
endfunction()

function(download_and_extract)
  set(options RM_EXTRAT_DIR_IF_EXISTS)
  set(oneValueArgs DESTINATION RENAME)
  set(multiValueArgs)
  set(oneValueArgs URL FILENAME HASH_TYPE HASH EXTRACT_DIR)
  cmake_parse_arguments(DAE "${options}" "${oneValueArgs}" "${multiValueArgs}"
                        ${ARGN})
  if(NOT DEFINED DAE_URL)
    message(FATAL_ERROR "Missing URL")
  endif()
  if(NOT DEFINED DAE_FILENAME)
    message(FATAL_ERROR "Missing FILENAME")
  endif()
  if(NOT DEFINED DAE_HASH_TYPE)
    message(FATAL_ERROR "Missing HASH_TYPE")
  endif()
  if(NOT DEFINED DAE_EXTRACT_DIR)
    message(FATAL_ERROR "Missing EXTRACT_DIR")
  endif()

  if(EXISTS ${DAE_EXTRACT_DIR})
    if(DAE_RM_EXTRAT_DIR_IF_EXISTS)
      message(STATUS "${DAE_EXTRACT_DIR} already exists, removing...")
      file(REMOVE_RECURSE ${DAE_EXTRACT_DIR})
    else()
      message(
        STATUS "${DAE_EXTRACT_DIR} already exists, skip download & extract")
      return()
    endif()
  endif()

  if(NOT EXISTS ${DAE_FILENAME})
    download_file(${DAE_URL} ${DAE_FILENAME})
    if(NOT DAE_HASH)
      download_file(${DAE_URL}.sig ${DAE_FILENAME}.sig)
    endif()
  endif()

  file(${DAE_HASH_TYPE} ${DAE_FILENAME} _ACTUAL_CHKSUM)

  if(NOT DAE_HASH)
    file(READ ${DAE_FILENAME}.sig _SIG_CONTENT)
    if(_SIG_CONTENT MATCHES "^([0-9a-z]+) ")
      set(_EXPECT_HASH ${CMAKE_MATCH_1})
    endif()
  else()
    set(_EXPECT_HASH ${DAE_HASH})
  endif()

  if(NOT (${_EXPECT_HASH} STREQUAL ${_ACTUAL_CHKSUM}))
    message(STATUS "Expect ${_EXPECT_HASH}")
    message(STATUS "Actual ${_ACTUAL_CHKSUM}")
    message(FATAL_ERROR "File hash miss match ..., please delete and retry")
  endif()

  extrat_file(${DAE_FILENAME} ${DAE_EXTRACT_DIR})
endfunction()
