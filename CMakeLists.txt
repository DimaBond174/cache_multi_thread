cmake_minimum_required(VERSION 3.8 FATAL_ERROR)
project(cache_multi_thread)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_C_COMPILER clang)
# TODO Set Clang path:
set(CLANG_PATH "/usr/local/clang_8.0.0")

# TODO Set executable name:
set(CUR_EXEC_NAME "test_cache_multi_thread")

# TODO Set executable folder:
set(CUR_EXEC_FOLDER "../test_cache_multi_thread")

# TODO Enshure target build is what you want:
# set(CMAKE_BUILD_TYPE "Debug") for release just comment:
#set(CMAKE_BUILD_TYPE "Debug")

include(cmake/options.cmake)
include(cmake/functions.cmake)


set(CUR_BUILD_DIR ${SPEC_BUILD_DIR})
set(CUR_SRC ${SPEC_SRC})
set(CUR_INCLUDES ${SPEC_INCLUDE})
set(CUR_DEFINITIONS ${SPEC_DEFINITIONS})
set(CUR_LINK_LIBS ${SPEC_LINK_LIBS})
set(CUR_PROPERTIES ${SPEC_PROPERTIES})


set(CUR_INCLUDES
       ${SPEC_INCLUDE}
       ${CLANG_PATH}/include/c++/v1
       ${ADD_INCLUDES}
       ${CPP_INCLUDES}       
       /usr/include
       )


     # Coping assets (TODO any change&rerun CMake to copy):
     FILE(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets)
     FILE(GLOB_RECURSE SpecAssets
         ${CMAKE_CURRENT_SOURCE_DIR}/assets/*.*
         ${CMAKE_CURRENT_SOURCE_DIR}/assets/*
     )
     FOREACH(file ${SpecAssets})
         FILE(RELATIVE_PATH
             ITEM_PATH_REL
             ${CMAKE_CURRENT_SOURCE_DIR}/assets
             ${file}
         )
         GET_FILENAME_COMPONENT(dirname ${ITEM_PATH_REL} DIRECTORY)
         FILE(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/${dirname})
         FILE(COPY ${file} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/${dirname})
     ENDFOREACH()


custom_add_executable(${CUR_EXEC_NAME}
    "${CUR_BUILD_DIR}"
    "${CUR_SRC}"
    "${CUR_INCLUDES}"
    "${CUR_DEFINITIONS}"
    "${CUR_LINK_LIBS}"
    "${CUR_PROPERTIES}")


