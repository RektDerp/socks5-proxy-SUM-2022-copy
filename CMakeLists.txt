cmake_minimum_required(VERSION 3.14)

project(socks5-proxy)
set (CMAKE_CXX_STANDARD 11)
set (libraries)
set(sourcedir ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(servicedir ${CMAKE_CURRENT_SOURCE_DIR}/service/windows)
set(libdir ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${libdir})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${libdir})

##########################################################################

set(modules
"${sourcedir}/LogConfigReader.cpp:"
"${sourcedir}/Logger.cpp:LogConfigReader"
"${sourcedir}/session.cpp:Logger sock5_impl"
"${sourcedir}/server.cpp: session"
"${sourcedir}/sock5_impl.cpp:session"
"${sourcedir}/stat_db_service.cpp:sqlite3"
"${sourcedir}/stat_handlers.cpp:"
"${servicedir}/service.cpp:"
)

set(executables
"main:${sourcedir}/main.cpp"
"LogConfigReader Logger session sock5_impl server sock5_impl stat_db_service sqlite3:STATIC"

"service:${servicedir}/main.cpp"
"service"
)

##########################################################################

set(i 0)
foreach (module IN LISTS modules)
    if (NOT ${module} MATCHES ".*\:.*")
        message( FATAL_ERROR "module " ${i} ": "  ${module} " has wrong format")
    endif()
    string(REPLACE ":" ";" module ${module})

    list(GET module 0 path)
    if (NOT EXISTS ${path})
        message( FATAL_ERROR "module " ${i} ": "  ${path} " does not exist")
    endif()

    get_filename_component(name ${path} NAME_WE)
    message (${name} " OBJECT " ${path})
    add_library(${name} OBJECT ${path})



    math(EXPR i "${i} + 1")
endforeach()
