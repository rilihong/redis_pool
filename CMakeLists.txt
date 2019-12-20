cmake_minimum_required(VERSION 3.14)
project(redis_pool)

set(CMAKE_CXX_STANDARD 11)

set(FILEINCLUDE "/usr/local/include")

link_directories("/usr/local/lib")

include_directories(include ${FILEINCLUDE})

link_libraries(hiredis pthread jsoncpp)

add_executable(redis_pool main.cpp base_mutex.h base_mutex.cpp redis_connect.h redis_connect.cpp  redis_pool.h redis_pool.cpp)