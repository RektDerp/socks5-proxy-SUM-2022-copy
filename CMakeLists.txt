cmake_minimum_required(VERSION 3.8)

project(socks5-proxy)
if(NOT DEFINED libdir)
set(libdir ${CMAKE_CURRENT_SOURCE_DIR}/lib)
endif()

if(NOT DEFINED includedir)
set(includedir ${CMAKE_CURRENT_SOURCE_DIR}/include)
endif()

if(NOT DEFINED Qt)
    set(Qt "Qt5")
endif()

if(NOT DEFINED Qtdir)
    set (Qtdir "${libdir}/Qt/QtStatic")
endif()

message ("Qtdir " ${Qtdir})

set (CMAKE_CXX_STANDARD 14)
set(sourcedir ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(uidir ${CMAKE_CURRENT_SOURCE_DIR}/ui)
set(servicedir ${CMAKE_CURRENT_SOURCE_DIR}/service/windows)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${libdir})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${libdir})
include_directories(${includedir})
add_compile_options("-O2")

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows" AND ${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
  message("Build for windows on unix")
  set (CMAKE_IGNORE_PREFIX_PATH "/usr")
  set(CMAKE_PREFIX_PATH "/usr/x86_64-w64-mingw32/" "${Qtdir}" "${CMAKE_CURRENT_SOURCE_DIR}")
  set(${Qt}_DIR "${Qtdir}/lib/cmake/${Qt}")
  set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
  set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
  set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
endif()

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
    message("Build for windows on windows")
	set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
	list(APPEND CMAKE_PREFIX_PATH "${Qtdir}" "${CMAKE_CURRENT_SOURCE_DIR}")
	message ("Qt " ${Qt} " ${Qtdir}/lib/cmake/${Qt}")
    set(${Qt}_DIR "${Qtdir}/lib/cmake/${Qt}")
endif()

##########################################################################
find_package(${Qt} COMPONENTS Core Widgets Sql Qml QuickControls2 REQUIRED)

set(CMAKE_AUTORCC ON)
qt_add_resources(QT_RESOURCES ${uidir}/main.qrc)

set(modules
"${sourcedir}/ConfigReader.cpp|"
"${sourcedir}/Logger.cpp|ConfigReader"
"${sourcedir}/session.cpp|server socks4 socks5"
"${sourcedir}/server.cpp|session"
"${sourcedir}/socks.cpp|session Logger"
"${sourcedir}/socks5.cpp|session ConfigReader socks"
"${sourcedir}/socks4.cpp|session socks"
"${sourcedir}/stat_db_service.cpp|Logger stat_handlers sqlite3"
"${sourcedir}/stat_handlers.cpp|sqlite3"
"${uidir}/sessionmodel.cpp|${Qt}::Widgets ${Qt}::Core ${Qt}::Sql"
"${uidir}/sortfiltersessionmodel.cpp|${Qt}::Widgets ${Qt}::Core ${Qt}::Sql"
"${uidir}/session.cpp|${Qt}::Sql"
)

set(executables
"main|${sourcedir}/main.cpp"
"Logger session ConfigReader server stat_db_service socks socks4 socks5 stat_handlers -lpthread"

"interface|${uidir}/main.cpp ${QT_RESOURCES} ${uidir}/session.h  ${uidir}/sessionmodel.h  ${uidir}/sortfiltersessionmodel.h"
"sessionmodel sortfiltersessionmodel session_d ${Qt}::Widgets ${Qt}::Core ${Qt}::Sql ${Qt}::QuickControls2"
)

set(moc "interface" "sessionmodel" "sortfiltersessionmodel")
add_compile_options("-static")
##########################################################################

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
  if (NOT EXISTS "${includedir}/sqlite3.c")
      message ("sqlite not found, downloading")
      file(MAKE_DIRECTORY "${includedir}")
      file(DOWNLOAD "https://www.sqlite.org/2022/sqlite-amalgamation-3390200.zip" "${includedir}/sqlite.zip" SHOW_PROGRESS)
      message("extracting")
	  file(ARCHIVE_EXTRACT INPUT "${includedir}/sqlite.zip" DESTINATION "${includedir}")
      file(COPY
      "${includedir}/sqlite-amalgamation-3390200/sqlite3.c"
      "${includedir}/sqlite-amalgamation-3390200/shell.c"
      "${includedir}/sqlite-amalgamation-3390200/sqlite3.h"
      "${includedir}/sqlite-amalgamation-3390200/sqlite3ext.h"
      DESTINATION "${includedir}")
      file(REMOVE_RECURSE "${includedir}/sqlite.zip" "${includedir}/sqlite-amalgamation-3390200")
  endif()
  find_path(boostcheck "boost/asio.hpp")
  message("boost " ${boostcheck})
  if(NOT EXISTS "${boostcheck}/boost/asio.hpp")
    message ("boost not found. downloading")
	file(MAKE_DIRECTORY "${includedir}")
	file(DOWNLOAD "https://boostorg.jfrog.io/artifactory/main/release/1.79.0/source/boost_1_79_0.7z" "${includedir}/boost.7z" SHOW_PROGRESS)
	file(ARCHIVE_EXTRACT INPUT "${includedir}/boost.7z" PATTERNS "boost_1_79_0/boost/*" DESTINATION "${includedir}")
	message("extracting")
	file(COPY "${includedir}/boost_1_79_0/boost" DESTINATION "${includedir}")
	file(REMOVE_RECURSE "${includedir}/boost.7z" "${includedir}/boost_1_79_0/")
  else()
      include_directories(${boostcheck})
  endif()

  SET(CMAKE_FIND_LIBRARY_PREFIXES "" "lib")
  SET(CMAKE_FIND_LIBRARY_SUFFIXES ".lib" ".dll")

  list(TRANSFORM modules APPEND " ws2_32 wsock32" AT 2 3)
  list(APPEND modules "${servicedir}/service.cpp|")
  list(APPEND modules "${includedir}/sqlite3.c|")
  list(APPEND executables "winservice|${servicedir}/main.cpp")
  list(APPEND executables "service")

  if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    list(TRANSFORM executables REPLACE "-lpthread" " -static-libgcc -static-libstdc++ -Wl,--whole-archive,-Bstatic -lssp -lpthread -Wl,--no-whole-archive" AT 1)
    list(TRANSFORM executables APPEND " -static-libgcc -static-libstdc++ -mwindows" AT 3)
  endif()
endif()

message("\n-==Defining modules==-\n")

set(i 0)
foreach (module IN LISTS modules)
    if (NOT ${module} MATCHES ".*\|.*")
        message( FATAL_ERROR "module " ${i} ": "  ${module} " has wrong format")
    endif()
    string(REPLACE "|" ";" module ${module})

    list(GET module 0 path)
    if (NOT EXISTS ${path})
        message( FATAL_ERROR "module " ${i} ": "  ${path} " does not exist")
    endif()

    get_filename_component(name ${path} NAME_WE)
    if(TARGET ${name})
        string(APPEND name "_d")
    endif()
    message ("\nModule " ${name} " at " ${path} "\nDependencies:")

    if(${name} IN_LIST moc)
      qt_wrap_cpp(moc ${path})
      message("wrapped " ${path})
    endif()

    add_library(${name} OBJECT STATIC ${path})

    list(GET module 1 dependencies)
    if(NOT ${dependencies} STREQUAL "")
      string(REPLACE " " ";" dependencies ${dependencies})
      foreach(dep ${dependencies})
        target_link_libraries (${name} ${dep})
        message(${dep})
      endforeach()
    else()
      message("<none>")
    endif()

    math(EXPR i "${i} + 1")
endforeach()

message("\n-==Linking executables==-\n")

list(LENGTH executables len)
math(EXPR parity "${len} % 2")
if(NOT ${parity} EQUAL 0)
  message(FATAL_ERROR "List of executables in wrong format. Format:\nOdd columns <name of executable>:<sources>\nEven columns: space-separated list of modules/libraries")
endif()

set(i 0)
while(${i} LESS ${len})
  list(GET executables ${i} exec)
  string(REPLACE "|" ";" exec ${exec})
  list(GET exec 0 executable)
  list(GET exec 1 executablepath)
  message (${executable} " at " ${executablepath} ":")
  string(REPLACE " " ";" executablepath ${executablepath})

  if(${executable} IN_LIST moc)
    qt_wrap_cpp(executablepath ${executablepath})
    message("wrapped " ${executablepath})
  endif()

  add_executable(${executable} ${executablepath})

  math(EXPR i "${i} + 1")

  list(GET executables ${i} libraries)
  if(NOT ${libraries} STREQUAL "")
    string(REPLACE " " ";" libraries ${libraries})
    foreach (lib ${libraries})
      if(TARGET ${lib})
        message("    lib " ${lib})
      else()
        message("    flag " ${lib})
      endif()
      target_link_libraries(${executable} ${lib})
    endforeach()
  endif()

  math(EXPR i "${i} + 1")
endwhile()


