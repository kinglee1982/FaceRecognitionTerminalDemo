cmake_minimum_required(VERSION 3.10)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE
      "${PROJECT_SOURCE_DIR}/cmake/himix200.toolchain.cmake"
      CACHE STRING "")
endif()

set(CMAKE_GENERATOR "Ninja")
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_DISABLE_SOURCE_CHANGES ON)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
  message(STATUS "Set up ccache ...")
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
  set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif()

project(
  FaceRecognitionTerminalDemo
  VERSION 0.1.0
  DESCRIPTION "FaceRecognitionTerminal Demo for Hi3516DV300")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

option(DOWNLOAD_DEPENDENCY "Download 3rd party dependencies from remote" ON)

set(PROJECT_DEPENDENCY_DIR
    "${PROJECT_SOURCE_DIR}/deps"
    CACHE STRING "Project dependencies dir")
set(RESOURCE_PREFIX
    "${PROJECT_SOURCE_DIR}/resources"
    CACHE STRING "Dir for model & font file")
set(HISI_SDK_PREFIX
    "${PROJECT_DEPENDENCY_DIR}/${HISI_SDK_PLATFORM}"
    CACHE STRING "Dir for hisi sdk")
set(THIRD_PARTY_PREFIX
    "${PROJECT_DEPENDENCY_DIR}/3rd"
    CACHE STRING "Dir for 3rd")
set(QUFACE_SDK_PREFIX
    "${PROJECT_DEPENDENCY_DIR}/qufacesdk"
    CACHE STRING "Dir for quface sdk")

message(STATUS "set HISI_SDK_PREFIX to: ${HISI_SDK_PREFIX}")
message(STATUS "set THIRD_PARTY_PREFIX to: ${THIRD_PARTY_PREFIX}")
message(STATUS "set QUFACE_SDK_PREFIX to: ${QUFACE_SDK_PREFIX}")

set(CMAKE_FIND_ROOT_PATH ${HISI_SDK_PREFIX} ${THIRD_PARTY_PREFIX}
                         ${QUFACE_SDK_PREFIX})
set(CMAKE_PREFIX_PATH ${HISI_SDK_PREFIX} ${THIRD_PARTY_PREFIX}
                      ${QUFACE_SDK_PREFIX})

if(DOWNLOAD_DEPENDENCY)
  # -- Download dependecies --
  include(cmake/download.cmake)

  set(DOWNLOAD_DIR "${PROJECT_DEPENDENCY_DIR}/download")

  download_and_extract(
    URL
    https://quvision.oss-cn-beijing.aliyuncs.com/qufacesdk/deps/prebuild-3rd-0.1.2.tar.gz
    FILENAME
    ${DOWNLOAD_DIR}/prebuild-3rd-0.1.2.tar.gz
    HASH_TYPE
    SHA256
    HASH
    2f129d529e599395b29b21b6d7d5b694b5f158f573433a2f911c834cb314e148
    EXTRACT_DIR
    ${THIRD_PARTY_PREFIX})

  download_and_extract(
    URL
    https://quvision.oss-cn-beijing.aliyuncs.com/qufacesdk/hisi/rp-dv300-sdk.tgz
    FILENAME
    ${DOWNLOAD_DIR}/rp-dv300-sdk.tgz
    HASH_TYPE
    SHA256
    HASH
    f9075af18adbb95a21789f6d79ca98fa1f92fee0696a2d156a88425fa4efdbcb
    EXTRACT_DIR
    ${HISI_SDK_PREFIX})

  download_and_extract(
    URL
    https://quvision.oss-cn-beijing.aliyuncs.com/qufacesdk/releases/QuFaceSDK-latest-hisi-rp-dv300.tar.gz
    FILENAME
    ${DOWNLOAD_DIR}/QuFaceSDK-latest-hisi-rp-dv300.tar.gz
    HASH_TYPE
    SHA256
    EXTRACT_DIR
    ${QUFACE_SDK_PREFIX})

  download_and_extract(
    URL
    https://quvision.oss-cn-beijing.aliyuncs.com/qufacesdk/hisi/hisi-model-2.0.0.zip
    FILENAME
    ${DOWNLOAD_DIR}/hisi-model-2.0.0.zip
    HASH_TYPE
    SHA256
    HASH
    4ccfeaf60be028ff0484e62e19ddd1f977710321390b98b36f543f1ab4c8c488
    EXTRACT_DIR
    ${RESOURCE_PREFIX}/models
    RM_EXTRAT_DIR_IF_EXISTS)
  download_and_extract(
    URL
    https://quvision.oss-cn-beijing.aliyuncs.com/qufacesdk/hisi/Adobe-SongTi-Std-L.zip
    FILENAME
    ${DOWNLOAD_DIR}/Adobe-SongTi-Std-L.zip
    HASH_TYPE
    SHA256
    HASH
    41a16d79b91ae10b712c116d49bb710101e74e51bbe68f50f932f8f2051ec528
    EXTRACT_DIR
    ${RESOURCE_PREFIX}/fonts
    RM_EXTRAT_DIR_IF_EXISTS)
endif()

find_package(QuFaceSDK REQUIRED)
message(STATUS "Found QuFaceSDK ${QuFaceSDK_VERSION}")

find_package(freetype REQUIRED)

aux_source_directory(src/lib _LIB_FILES)
aux_source_directory(src/lib/io _LIB_IO_FILES)
aux_source_directory(src/app _APP_FILES)

add_executable(main src/main.cpp ${_LIB_FILES} ${_LIB_IO_FILES} ${_APP_FILES})

target_include_directories(main PRIVATE quface/include src src/app src/lib)
target_link_libraries(main QuFaceSDK::faceServer freetype)

install(TARGETS main DESTINATION .)
install(DIRECTORY ${RESOURCE_PREFIX}/models/ ${RESOURCE_PREFIX}/fonts/ asserts/
        DESTINATION .)
