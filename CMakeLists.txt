cmake_minimum_required(VERSION 3.10)

project(socks5-proxy)
set (CMAKE_CXX_STANDARD 11)

#set(CMAKE_SYSTEM_NAME Windows)

set(executable "main")
###############################################################################################################

set(modules
"LogConfigReader.cpp"
"Logger.cpp"
"session.cpp"
"server.cpp"
"statWriter.cpp"
"sock5_impl.cpp"
"stat_db_service.cpp"
)

set(module_dependencies
""
"LogConfigReader"
"statWriter Logger sock5_impl"
"session"
""
"session"
"sqlite3"
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

  list(TRANSFORM module_dependencies APPEND " ws2_32 wsock32" AT 2 3)

  add_compile_options("-static" "-lstdc++" "-lgcc")
endif()

add_executable(${executable} ${sourcedir}/${executable}.cpp)

foreach (src dependencies IN ZIP_LISTS modules module_dependencies)
    if(EXISTS ${sourcedir}/${src})
        string(REGEX REPLACE "\.[a-z]+$" "" lib "${src}")
        string(PREPEND src "${sourcedir}/")
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
