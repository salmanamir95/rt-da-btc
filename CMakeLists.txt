cmake_minimum_required(VERSION 3.10)
project(RT-DA)

# Set C++20 standard as requested
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find packages using pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(IXWEBSOCKET REQUIRED IMPORTED_TARGET ixwebsocket)

find_package(nlohmann_json QUIET)

# Find system libraries: libcurl, zlib, and pthreads
find_package(CURL REQUIRED)
find_package(ZLIB REQUIRED)
find_package(Threads REQUIRED)

# Use GLOB_RECURSE to find all cpp and header files in src and inc
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "inc/*.h" "inc/*.hpp")

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE 
    inc
    inc/ingestor/classes
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE 
    PkgConfig::IXWEBSOCKET
    CURL::libcurl
    ZLIB::ZLIB
    Threads::Threads
)

if(nlohmann_json_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)
endif()
