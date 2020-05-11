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
set(MODEL_FILE_PREFIX
    "${PROJECT_SOURCE_DIR}/resources"
    CACHE STRING "Dir for model file")
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
    https://quvision.oss-cn-beijing.aliyuncs.com/qufacesdk/deps/prebuild-3rd-0.1.0.tar.gz
    ${DOWNLOAD_DIR}/prebuild-3rd-0.1.0.tar.gz
    SHA256
    65bbd66b356eb2886568e6851bb6b6709d25bfad92147f9276f3324313c55c39
    ${THIRD_PARTY_PREFIX})

  download_and_extract(
    https://quvision.oss-cn-beijing.aliyuncs.com/qufacesdk/hisi/rp-dv300-sdk.tgz
    ${DOWNLOAD_DIR}/rp-dv300-sdk.tgz
    SHA256
    f9075af18adbb95a21789f6d79ca98fa1f92fee0696a2d156a88425fa4efdbcb
    ${HISI_SDK_PREFIX})

  download_and_extract(
    https://quvision.oss-cn-beijing.aliyuncs.com/qufacesdk/releases/QuFaceSDK-0.9.2-hisi-rp-dv300.tar.gz
    ${DOWNLOAD_DIR}/QuFaceSDK-0.9.2-hisi-rp-dv300.tar.gz
    SHA256
    196a33e92866bd0d2abd2e18fe5ad3f0574f38eb9cd09bc7227cc546a975bbd8
    ${QUFACE_SDK_PREFIX})

  download_and_extract(
    https://quvision.oss-cn-beijing.aliyuncs.com/qufacesdk/hisi/hisi-model-0.1.0.tar.gz
    ${DOWNLOAD_DIR}/hisi-model-0.1.0.tar.gz
    SHA256
    e3b9e9c7f9f62f1f6c20a70b2c5220ae9303caaf610d5c61e2a40a4ac41e3bf4
    ${MODEL_FILE_PREFIX})
endif()

find_package(QuFaceSDK REQUIRED)
find_package(freetype REQUIRED)

aux_source_directory(src/lib _LIB_FILES)
aux_source_directory(src/lib/io _LIB_IO_FILES)
aux_source_directory(src/app _APP_FILES)

add_executable(main src/main.cpp ${_LIB_FILES} ${_LIB_IO_FILES} ${_APP_FILES})

target_include_directories(main PRIVATE quface/include src src/app src/lib)
target_link_libraries(main QuFaceSDK::faceServer freetype)
