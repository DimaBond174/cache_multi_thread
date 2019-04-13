# This is the source code of SpecNet project
# It is licensed under MIT License.
#
#  Copyright (c) Dmitriy Bondarenko
#  feel free to contact me: specnet.messenger@gmail.com

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set(SPEC_BUILD DDEBUG)
else()
    set(SPEC_BUILD DRELEASE)
endif()

# Inject Components:
# warn: label name must be differen from class name (Error: declaration of anonymous class must be a definition)


# Configuration of the assembly
#   according to the selected components:
set(SPEC_DEFINITIONS
	${SPEC_DEFINITIONS}
    ${CMAKE_SYSTEM_NAME}
    ${SPEC_FILE}
    ${SPEC_CONFIG}    
    ${CMAKE_BUILD_TYPE}
    ${SPEC_BUILD}        
)

set(SPEC_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

set(SPEC_INCLUDE    
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    )


if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
   message(STATUS "Linux was chosen ")
   file(GLOB_RECURSE LOC_SRC
       ${CMAKE_CURRENT_SOURCE_DIR}/src/system/linux/*.h
       ${CMAKE_CURRENT_SOURCE_DIR}/src/system/linux/*.cpp
   )
   set(SPEC_SRC ${SPEC_SRC}  ${LOC_SRC})

   add_library( filesys
                STATIC
                IMPORTED )

   set(SPEC_LINK_LIBS
             ${SPEC_LINK_LIBS}
			 pthread
#       stdc++fs
       filesys
         )

   set_target_properties( # Specifies the target library.
           filesys
           PROPERTIES IMPORTED_LOCATION
           ${CLANG_PATH}/lib/libc++fs.a )

elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
   message(STATUS "Windows was chosen ")
   file(GLOB_RECURSE LOC_SRC
       ${CMAKE_CURRENT_SOURCE_DIR}/src/system/windows/*.h
       ${CMAKE_CURRENT_SOURCE_DIR}/src/system/windows/*.cpp
   )
   set(SPEC_SRC ${SPEC_SRC}  ${LOC_SRC})
   	set(SPEC_DEFINITIONS
	${SPEC_DEFINITIONS}
	"WIN32_LEAN_AND_MEAN")
endif()


file(GLOB_RECURSE DEF_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/i/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/i/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/spec/*.h    
    ${CMAKE_CURRENT_SOURCE_DIR}/src/configs/commandline/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/configs/commandline/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/configs/hardcode/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/configs/hardcode/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/configs/json/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/configs/json/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/system/filesystem/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/system/filesystem/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/system/linux/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/system/linux/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cases/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cases/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/testers/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/testers/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/caches/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/caches/*.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/caches/facebook/*.h
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/caches/facebook/*.cpp
)


set(SPEC_SRC
    ${SPEC_SRC}
    ${DEF_SRC}
    )
message("SPEC_SRC: ${SPEC_SRC}")

# Output folder for binaries
set(SPEC_BUILD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${CUR_EXEC_FOLDER})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${SPEC_BUILD_DIR}/libs)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${SPEC_BUILD_DIR}/libs)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${SPEC_BUILD_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG  ${SPEC_BUILD_DIR})


# Current OS name:
message("CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")


set(SPEC_PROPERTIES
    DEBUG_POSTFIX             "d"
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
