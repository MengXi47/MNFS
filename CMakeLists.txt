cmake_minimum_required(VERSION 3.12)
project(MNFS VERSION 0.0.1 LANGUAGES C CXX)

set(CMAKE_CONFIGURATION_TYPES "RelWithDebInfo;Debug;Release;MinSizeRel" CACHE STRING "" FORCE)
if (NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "None")
    set (CMAKE_BUILD_TYPE "RelWithDebInfo")
    message (STATUS "CMAKE_BUILD_TYPE is not set, set to default = ${CMAKE_BUILD_TYPE}")
endif ()
message (STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

string (TOUPPER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE_UC)

if(CMAKE_BUILD_TYPE_UC STREQUAL "DEBUG" )
    option(ENABLE_ASSERTIONS "Enable assertions" ON)
else()
    option(ENABLE_ASSERTIONS "Enable assertions" OFF)
endif()
message (STATUS "ENABLE_ASSERTIONS: ${ENABLE_ASSERTIONS}")

if(ENABLE_ASSERTIONS)
    add_definitions(-D_DEBUG)
    # On non-Debug builds cmake automatically defines NDEBUG, so we explicitly undefine it:
    if(NOT CMAKE_BUILD_TYPE_UC STREQUAL "DEBUG")
        # NOTE: use `add_compile_options` rather than `add_definitions` since
        # `add_definitions` does not support generator expressions.
        add_compile_options($<$<OR:$<COMPILE_LANGUAGE:C>,$<COMPILE_LANGUAGE:CXX>>:-UNDEBUG>)
    endif()
endif()

set(SHUFFLE_METHODS "g++10" "g++11" "stdshuffle")
set(SHUFFLE_METHOD "" CACHE STRING "Choose the shuffle method (REQUIRED)")
set_property(CACHE SHUFFLE_METHOD PROPERTY STRINGS ${SHUFFLE_METHODS})

if("${SHUFFLE_METHOD}" STREQUAL "")
    message(FATAL_ERROR
        "\n [ERROR]: SHUFFLE_METHOD is not set!\n"
        " You must specify it using: cmake -DSHUFFLE_METHOD=<value>\n"
        " Valid options are: ${SHUFFLE_METHODS}"
    )
elseif(NOT SHUFFLE_METHOD IN_LIST SHUFFLE_METHODS)
    message(FATAL_ERROR
        "\n [ERROR]: Invalid SHUFFLE_METHOD: '${SHUFFLE_METHOD}'\n"
        " Valid options are: ${SHUFFLE_METHODS}"
    )
endif()

message(STATUS "Use Shuffle Method: ${SHUFFLE_METHOD}")
if(SHUFFLE_METHOD STREQUAL "stdshuffle")
    add_compile_definitions(USE_STD_SHUFFLE)
elseif(SHUFFLE_METHOD STREQUAL "g++10")
    add_compile_definitions(USE_GCC10_SHUFFLE)
elseif(SHUFFLE_METHOD STREQUAL "g++11")
    add_compile_definitions(USE_GCC11_SHUFFLE)
endif()

option(OVERRIDE_CXX_NEW_DELETE "Override C++ new/delete operator" OFF)
option(SAVE_ALLOCATE_SIZE "Use more memory to save allocate size" OFF)

option(ENABLE_FUSE_APPLICATION "" ON)

if (DEFINED SANITIZER AND SANITIZER)
    set(OVERRIDE_CXX_NEW_DELETE OFF)
endif()
message (STATUS "OVERRIDE_CXX_NEW_DELETE: ${OVERRIDE_CXX_NEW_DELETE}")

if (OVERRIDE_CXX_NEW_DELETE)
    add_definitions(-DOVERRIDE_CXX_NEW_DELETE)
    if (SAVE_ALLOCATE_SIZE)
        add_definitions(-DSAVE_ALLOCATE_SIZE)
    endif()
endif()
message (STATUS "SAVE_ALLOCATE_SIZE: ${SAVE_ALLOCATE_SIZE}")

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED OFF)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcoroutines-ts")
    set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -latomic")
    add_link_options(-fuse-ld=lld)
    # Do not build with libc++ (LLVM's implementation of the C++ standard library) in fdb
    set(USE_LIBCXX OFF)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcoroutines")
endif()

if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
    add_compile_options(-msse4.2 -mavx2)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
    # ARM architecture detected:
    # Clang on ARM uses built-in runtime library (compiler-rt) to provide symbols like '__muloti4'.
    # Without explicitly specifying '-rtlib=compiler-rt', linking may fail due to missing '__muloti4'.
    # '-unwindlib=libgcc' ensures proper exception unwinding compatibility.
    add_compile_options(-march=armv8-a+crc)
    message(STATUS "ARM architecture detected, linking with compiler-rt and libgcc.")
    add_link_options("-rtlib=compiler-rt" "-unwindlib=libgcc")
endif()

include(cmake/CompileFlags.cmake)

store_compile_flags()
add_subdirectory("third_party/fmt" EXCLUDE_FROM_ALL)
restore_compile_flags()
set(FOLLY_NO_EXCEPTION_TRACER ON)
add_subdirectory("third_party/folly" EXCLUDE_FROM_ALL)
restore_compile_flags()
