cmake_minimum_required(VERSION 3.10)

project(socks5-proxy)
set (CMAKE_CXX_STANDARD 11)

#set(CMAKE_SYSTEM_NAME Windows)

set(executable "main")
###############################################################################################################

set(modules "LogConfigReader.cpp\;" "Logger.cpp\;LogConfigReader" "session.cpp\;statWriter Logger sock5_impl" "server.cpp\;session" "statWriter.cpp\;" "sock5_impl.cpp\;session")

###############################################################################################################
set (libraries)
set(sourcedir ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(libdir ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${libdir})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${libdir})
link_directories(${libdir})

message(${CMAKE_SYSTEM_NAME})
if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows" AND UNIX)
  message("windows on unix")
  set(CMAKE_C_COMPILER   x86_64-w64-mingw32-gcc)
  set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
  include_directories(/usr/x86_64-w64-mingw32/include/)
  link_directories(/usr/x86_64-w64-mingw32/lib/ ${libdir})
  list(APPEND modules "ws2_32\;${libdir}/ws2_32.dll")
endif()

add_executable(${executable} ${sourcedir}/${executable}.cpp)

foreach (elem IN LISTS modules)
    list(GET elem 0 src)
    if(EXISTS ${sourcedir}/${src})
        string(REGEX REPLACE "\.[a-z]+$" "" lib "${src}")
        string(PREPEND src "${sourcedir}/")
        list(GET elem 1 dependencies)
        list(APPEND libraries ${lib})
        message("\nlib " ${lib}: "\nsrc: " ${src} "\ndependencies:")
        add_library(${lib} ${src})

        string(REPLACE " " ";" dependencies "${dependencies}")

        foreach(dep ${dependencies})
          message(${dep})
          target_link_libraries (${lib} ${dep})
        endforeach()
    else()
        string(REGEX REPLACE "\.[a-z]+$" "" lib "${src}")
        list(GET elem 1 path)
        message(${path})
        target_sources(${executable} PRIVATE ${path})
    endif()
endforeach()

message("libraries: " ${libraries})
list(LENGTH libraries len)
message("len: " ${len})

target_link_libraries(${executable} ${libraries})
