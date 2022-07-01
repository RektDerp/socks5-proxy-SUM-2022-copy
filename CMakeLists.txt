cmake_minimum_required(VERSION 3.10)

project(socks5-proxy)
set (CMAKE_CXX_STANDARD 11)

set(executable "server")
#2D list of modules and their dependencies
set(modules "LogConfigReader.cpp\;" "Logger.cpp\;LogConfigReader" "proxy_test.cpp\;" "tcp_connection.cpp\;" "stdc++\;")
set (libraries)

set(sourcedir ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

foreach (elem IN LISTS modules)
    list(GET elem 0 src)
    if(src MATCHES ".*\.[a-z]+")
        message("matched, build and link")
        string(REGEX REPLACE "\.[a-z]+$" "" lib "${src}")
        string(PREPEND src "${sourcedir}/")
        list(GET elem 1 dependencies)
        list(APPEND libraries ${lib})
        message("\nlib " ${lib}: "\nsrc: " ${src} "\ndependencies: \n" ${dependencies})

        add_library(${lib} ${src})
        target_link_libraries (${lib} ${dependencies})
    else()
        message("not matched, it is external lib")
    endif()
endforeach()

message("libraries: " ${libraries})
list(LENGTH libraries len)
message("len: " ${len})

add_executable(${executable} ${sourcedir}/${executable}.cpp)
target_link_libraries(${executable} ${libraries})
