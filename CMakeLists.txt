cmake_minimum_required(VERSION 3.10)

project(socks5-proxy)
set (CMAKE_CXX_STANDARD 11)
set (libraries)
set(sourcedir ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(libdir ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${libdir})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${libdir})
set(LINK_DIRECTORIES ${libdir})
link_directories(${LINK_DIRECTORIES})
#set(CMAKE_VERBOSE_MAKEFILE ON)

###############################################################################################################
set(executable "main")

set(modules
"LogConfigReader.cpp"
"Logger.cpp"
"session.cpp"
"server.cpp"
"statWriter.cpp"
"sock5_impl.cpp"
"stat_db_service.cpp"
"sqlite3"
)

set(module_dependencies
""
"LogConfigReader"
"statWriter Logger sock5_impl"
"session"
""
"session"
"sqlite3"
""
)

###############################################################################################################

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows" AND CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
  message("Build for windows on unix")
  include_directories(/usr/x86_64-w64-mingw32/include/)
  set(LINK_DIRECTORIES /usr/x86_64-w64-mingw32/lib/ /usr/x86_64-w64-mingw32/bin/)
  link_directories(${LINK_DIRECTORIES})

  list(TRANSFORM module_dependencies APPEND " ws2_32 wsock32" AT 2 3)
  list(APPEND modules "-static-libgcc" "-static-libstdc++" "ws2_32" "wsock32" "-l:libwinpthread.dll.a")
  list(APPEND module_dependencies "" "" "" "" "")

endif()

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
  SET(CMAKE_FIND_LIBRARY_PREFIXES "")
  SET(CMAKE_FIND_LIBRARY_SUFFIXES ".lib" ".dll")
endif()

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
  list(REMOVE_AT module_dependencies 6)
  list(INSERT module_dependencies 6 "sqlite3")

  add_compile_options("-static")
  add_executable("service" "service/windows/main.cpp")
  add_library("serviceobj" "service/windows/service.cpp")
  target_link_libraries("service" "serviceobj")
  target_link_libraries("service" "-LKernel32.dll")
  target_link_libraries("service" "-LAdvapi32.dll")
  target_link_libraries("service" "-static-libgcc")
  target_link_libraries("service" "-static-libstdc++")
  target_link_libraries("service" "ws2_32")
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
          if(NOT EXISTS ${sourcedir}/${dep}.cpp)
            find_library(deppath ${dep} PATHS ${LINK_DIRECTORIES} REQUIRED)
            message("at " ${deppath})
          endif()
          target_link_libraries (${lib} ${dep})
        endforeach()
    else()
        message("\nexternal lib " ${src}:)
        string(REGEX REPLACE "\.[a-z]+$" "" lib "${src}")
        find_library(path ${src} PATHS ${LINK_DIRECTORIES} REQUIRED)
        message(${path})
        list(APPEND libraries ${src})
    endif()
endforeach()

message(${modules})

target_link_libraries(${executable} ${libraries})
