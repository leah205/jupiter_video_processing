
# run with: cmake --build build -j

cmake_minimum_required(VERSION 3.16)

project(Jupiter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Dependencies
find_package(OpenCV REQUIRED)

include(FetchContent)

FetchContent_Declare(
    plog
    GIT_REPOSITORY https://github.com/SergiusTheBest/plog.git
    GIT_TAG 1.1.11
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(plog)

# Sources
file(GLOB SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_executable(jupiter ${SOURCES})

target_include_directories(jupiter PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/external/wavelib/header"
    ${OpenCV_INCLUDE_DIRS}
)

target_link_directories(jupiter PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/external/wavelib/Bin"
)

target_link_libraries(jupiter PRIVATE
    ${OpenCV_LIBS}
    wavelib
    wauxlib
    plog::plog
)

target_compile_options(jupiter PRIVATE
    -Wall
    -Wextra
    -g
)

option(ENABLE_PROFILING "Enable gprof profiling" OFF)

if(ENABLE_PROFILING)
    target_compile_options(jupiter PRIVATE -pg)
    target_link_options(jupiter PRIVATE -pg)
endif()
