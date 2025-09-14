cmake_minimum_required(VERSION 3.28)

if(MSVC)
    message(FATAL_ERROR "MSVC Compiler frontend is not supported, use GCC or Clang (not clang-cl it has MSVC frontend)")
endif()

if(CMAKE_CXX_COMPILER MATCHES "/msys[36][24]/")
    set(CLANG_VARIANT "MSYS2" CACHE STRING "Detected MSYS2 environment")
else()
    set(CLANG_VARIANT "OTHER" CACHE STRING "Not MSYS2 environment")
endif()

set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}" CACHE INTERNAL "")

if(CLANG_VARIANT STREQUAL "MSYS2")
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Bstatic -lc++ -lc++abi -Wl,-Bdynamic")
    endif()
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++ -static-libgcc -static")
        #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static")
    endif()
endif ()

project(VoidScape)
    if(CLANG_VARIANT STREQUAL "OTHER" AND CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_SYSTEM_NAME STREQUAL "Windows")
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:Debug>")
    endif ()

    if (NOT (
            (EXISTS "${PROJECT_SOURCE_DIR}/thirdparty/PerlinNoise/LICENSE") AND
            (EXISTS "${PROJECT_SOURCE_DIR}/CoriEngine/CMakeLists.txt")
            ))
        message(FATAL_ERROR "VoidScape thirdparty dependencies not found. Are you sure you cloned the repo recursively? Try running 'git submodule update --init --recursive' to download dependencies.")
    endif()

    set(CMAKE_CXX_STANDARD 23)

    add_compile_options(-march=native)

    get_filename_component(VOIDSCAPE_SOURCE_DIR ${PROJECT_SOURCE_DIR} ABSOLUTE)

    set(EXAMPLE_RUNTIME_OUTPUT_DIR ${PROJECT_SOURCE_DIR}/bin/Debug) # Or any default config

    # Calculate the relative path from the runtime output directory to the source directory.
    file(RELATIVE_PATH VOIDSCAPE_RELATIVE_PATH
        "${EXAMPLE_RUNTIME_OUTPUT_DIR}"
        "${VOIDSCAPE_SOURCE_DIR}")

    #cmake_path(CONVERT "${VOIDSCAPE_SOURCE_DIR}" TO_CMAKE_PATH_LIST VOIDSCAPE_ROOT_DIR)

    cmake_path(CONVERT "${VOIDSCAPE_RELATIVE_PATH}" TO_CMAKE_PATH_LIST VOIDSCAPE_ROOT_DIR)

    set(PATH_DEFINES_HEADER_IN "${VOIDSCAPE_SOURCE_DIR}/cmake/PathDefines.hpp.in")
    set(PATH_DEFINES_HEADER_OUT "${VOIDSCAPE_SOURCE_DIR}/src/Generated/PathDefinesGenerated.hpp")

    configure_file(${PATH_DEFINES_HEADER_IN} ${PATH_DEFINES_HEADER_OUT})

    add_subdirectory(${PROJECT_SOURCE_DIR}/CoriEngine)

    set(EXECUTABLE_NAME ${PROJECT_NAME})

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/$<CONFIG>)
    file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/src/*.cpp)

    add_executable(${EXECUTABLE_NAME} ${MY_SOURCES})

    target_include_directories(${EXECUTABLE_NAME} PRIVATE
            ${PROJECT_SOURCE_DIR}/thirdparty/PerlinNoise
            ${PROJECT_SOURCE_DIR}/src
            ${VOIDSCAPE_SOURCE_DIR}/src/Generated
        )


    #target_compile_options(${EXECUTABLE_NAME} PRIVATE -Wall -Wextra)
    target_compile_options(${EXECUTABLE_NAME} PRIVATE -w)

    target_compile_definitions(${EXECUTABLE_NAME} PRIVATE
        $<$<CONFIG:Debug>:DEBUG_BUILD>
        $<$<CONFIG:RelWithDebInfo>:DEBUG_BUILD>
        $<$<CONFIG:Release>:RELEASE_BUILD>
    )

    target_link_libraries(${EXECUTABLE_NAME} PRIVATE CoriEngine_static)