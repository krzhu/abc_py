cmake_minimum_required(VERSION 2.8)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON )
set(CMAKE_VERBOSE_MAKEFILE ON)

set(PROJECT_NAME "abc_py")
project(${PROJECT_NAME})


#The project version number
set(VERSION_MAJOR   0   CACHE STRING "Project major version number.")
set(VERSION_MINOR   2   CACHE STRING "Project minor version number.")
set(VERSION_PATCH   0   CACHE STRING "Project patch version number.")

# output path
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(DOCUMENT_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/docs)
set(PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)


#Check if intall prefix flag is found, if not, set a default path
if (CMAKE_INSTALL_PREFIX)
    set(CMAKE_INSTALL_PREFIX_DIR ${CMAKE_INSTALL_PREFIX})
else()
    set(CMAKE_INSTALL_PREFIX_DIR ${CMAKE_CURRENT_SOURCE_DIR}/install)
endif()
unset(CMAKE_INSTALL_PREFIX)


message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")



# Find the directory for ABC. If cache variable ABC_DIR is defined, use it as the path to ABC, otherwise use the system ABC_DIR
if (ABC_DIR)
    set(ABC_ROOT_DIR ${ABC_DIR})
    message(STATUS "Use ABC: ${ABC_ROOT_DIR}")
else()
    set(ABC_ROOT_DIR $ENV{ABC_DIR})
    message(STATUS "Use system ABC ${ABC_ROOT_DIR}")
endif()
unset (ABC_DIR CACHE)

find_package(Boost 1.6 COMPONENTS system graph iostreams)

# add a target to generate API documentation with Doxygen
find_package(Doxygen)
option(BUILD_DOCUMENTATION "Create and install the HTML based API documentation (requires Doxygen)" ${DOXYGEN_FOUND})

if(BUILD_DOCUMENTATION)
    if(NOT DOXYGEN_FOUND)
        message(FATAL_ERROR "Doxygen is needed to build the documentation.")
    endif()

    set(doxy_main_page ${CMAKE_CURRENT_SOURCE_DIR}/Readme.md)
    set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)
    set(doxyfile ${DOCUMENT_OUTPUT_PATH}/Doxyfile)

    #request to configure the file
    configure_file(${doxyfile_in} ${doxyfile} @ONLY)

    add_custom_target(doc
        COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
        WORKING_DIRECTORY ${DOCUMENT_OUTPUT_PATH}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM)

    #install(DIRECTORY ${DOCUMENT_OUTPUT_PATH}/html DESTINATION ${CMAKE_INSTALL_PREFIX_DIR}/doc)
endif()
mark_as_advanced(VERSION_MAJOR VERSION_MINOR VERSION_PATCH)



file(GLOB SOURCES src/global/*.h    src/global/*.cpp
                  src/interface/*.h      src/interface/*.cpp
                  src/util/*.h      src/util/*.cpp
                  src/util/thirdparty/*.h      src/util/thirdparty/*.cpp
                  )

file(GLOB EXE_SOURCES src/main/main.cpp)
file(GLOB PY_API_SOURCES src/api/*.cpp)
file(GLOB UNITTEST_SOURCES 
    unittest/main/*.cpp
    unittest/db/*.cpp
    unittest/parser/*.cpp
    ${SOURCES})

#pybind11
if (PYBIND11_DIR)
    set(PYBIND11_ROOT_DIR ${PYBIND11_DIR})
    message(STATUS "Use pybind11: ${PYBIND11_ROOT_DIR}")
else()
    set(PYBIND11_ROOT_DIR $ENV{PYBIND11_DIR})
    message(STATUS "Use system pybind11: ${PYBIND11_ROOT_DIR}")
endif()
unset(PYBIND11_DIR CACHE)

add_subdirectory(${PYBIND11_ROOT_DIR} "./pybind11")

# Python
if (NOT PYTHON_INCLUDE_DIR OR NOT PYTHON_LIBRARIES)
find_package(PythonLibs REQUIRED)
endif()
#set(PYTHON_INCLUDE_DIR "/home/local/eda09/keren/libs/python-3.7.5-shared/include/python3.7m")
#set(PYTHON_LIBRARIES "/home/local/eda09/keren/libs/python-3.7.5-shared/lib/libpython3.7m.a")
message(STATUS "python include directory: ${PYTHON_INCLUDE_DIR}")
message(STATUS "python libraries: ${PYTHON_LIBRARIES}")

#Print out the used build type
IF(CMAKE_BUILD_TYPE MATCHES Debug)
    message(STATUS "Using build type DEBUG")
    #ENDIF(CMAKE_BUILD_TYPE MATCHES Debug)
ELSEIF(CMAKE_BUILD_TYPE MATCHES Release)
    message(STATUS "Using build type RELEASE")
    #ENDIF(CMAKE_BUILD_TYPE MATCHES Release)
ELSEIF(CMAKE_BUILD_TYPE MATCHES Profile)
    message(STATUS "Using build type PROFILE")
    #ENDIF(CMAKE_BUILD_TYPE MATCHES Profile)
ELSE()
    message(STATUS "Using build type DEFAULT: using Release flag")
    set(CMAKE_BUILD_TYPE Release)
ENDIF()
#set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(CMAKE_CXX_FLAGS "-std=c++14 -Wall -march=native -fopenmp -fPIC")
#set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -march=native -fopenmp -fext-numeric-literals")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -fno-inline ")
#set(CMAKE_CXX_FLAGS_RELEASE "-O3 -fno-inline") 
#set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS_RELEASE "-Ofast -frename-registers -fprofile-use -fprofile-correction") 
set(CMAKE_CXX_FLAGS_PROFILE "-Ofast -pg -Winline")
set(CMAKE_EXE_LINKER_FLAGS "-pthread -static-libgcc -static-libstdc++ -static")


#add_executable(${PROJECT_NAME} ${SOURCES} ${EXE_SOURCES})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/unittest
    ${Boost_INCLUDE_DIR}
    ${ZLIB_INCLUDE_DIRS}
    ${ABC_ROOT_DIR}
    ${ABC_ROOT_DIR}/src
)



link_libraries (
    ${GTEST_MAIN_LIB}
    ${PYTHON_LIBRARIES}
)

# linking libraries
file(GLOB STATIC_LIB 
    ${ABC_ROOT_DIR}/libabc.a
    )



# Add modules to pybind
pybind11_add_module("abc_py" ${PY_API_SOURCES} ${SOURCES})
target_link_libraries("abc_py" PUBLIC ${STATIC_LIB} ${Boost_LIBRARIES} )

