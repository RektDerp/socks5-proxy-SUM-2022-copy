cmake_minimum_required(VERSION 3.10)

project(socks5-proxy)
set (CMAKE_CXX_STANDARD 11)

set(CMAKE_SYSTEM_NAME Windows)

set(executable "main")
###############################################################################################################
#Format : <Library source>.cpp\;<dependencies>

set(modules
"LogConfigReader.cpp\;"
"Logger.cpp\;LogConfigReader"
"session.cpp\;statWriter Logger sock5_impl "
"server.cpp\;session "
"statWriter.cpp\;"
"sock5_impl.cpp\;session"
"stat_db_service.cpp\;sqlite3"
)

set(modules_windows
"LogConfigReader.cpp\;"
"Logger.cpp\;LogConfigReader"
"session.cpp\;statWriter Logger sock5_impl wsock32 ws2_32"
"server.cpp\;session wsock32 ws2_32"
"statWriter.cpp\;"
"sock5_impl.cpp\;session"
"stat_db_service.cpp\;sqlite3"
"stdc++\;"
"gcc\;"
)

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
  message("Build for windows on unix")
  set(CMAKE_C_COMPILER   x86_64-w64-mingw32-gcc)
  set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
  include_directories(/usr/x86_64-w64-mingw32/include/)
  link_directories(/usr/x86_64-w64-mingw32/lib/ /usr/x86_64-w64-mingw32/bin/)

  set(modules ${modules_windows})

  add_compile_options("-static" "-lstdc++" "-lgcc")
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
        message("\nexternal lib " ${src}:)
        string(REGEX REPLACE "\.[a-z]+$" "" lib "${src}")
        list(GET elem 1 path)
        #add_library(${src} STATIC)
        #set_target_properties(${src} PROPERTIES IMPORTED_LOCATION ${path})
        list(APPEND libraries ${src})
    endif()
endforeach()

message("\nmodules: ")

foreach (elem IN LISTS modules)
    message(${elem})
endforeach()

message("\nlibraries: " ${libraries})
list(LENGTH libraries len)
message("len: " ${len})

target_link_libraries(${executable} ${libraries})
