cmake_minimum_required(VERSION 3.8)

project(socks5-proxy)
set (CMAKE_CXX_STANDARD 11)
set (Qt "Qt6")
set(sourcedir ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(uidir ${CMAKE_CURRENT_SOURCE_DIR}/ui)
set(servicedir ${CMAKE_CURRENT_SOURCE_DIR}/service/windows)
set(libdir ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${libdir})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${libdir})
add_compile_options("-O2")
set(CMAKE_INCLUDE_CURRENT_DIR ON)


##########################################################################
find_package(${Qt} COMPONENTS Core Widgets Sql Qml QuickControls2 REQUIRED)

set(CMAKE_AUTORCC ON)
qt_add_resources(QT_RESOURCES ${uidir}/main.qrc)

set(modules
"${sourcedir}/LogConfigReader.cpp,"
"${sourcedir}/Logger.cpp,LogConfigReader"
"${sourcedir}/session.cpp,Logger sock5_impl"
"${sourcedir}/server.cpp,session"
"${sourcedir}/sock5_impl.cpp,session"
"${sourcedir}/stat_db_service.cpp,Logger stat_handlers -lsqlite3"
"${sourcedir}/stat_handlers.cpp,"
"${uidir}/sessionmodel.cpp,${Qt}::Widgets ${Qt}::Core ${Qt}::Sql"
"${uidir}/sortfiltersessionmodel.cpp,${Qt}::Widgets ${Qt}::Core ${Qt}::Sql"
)

set(executables
"main:${sourcedir}/main.cpp"
"LogConfigReader Logger session sock5_impl server sock5_impl stat_db_service -lsqlite3"

"interface:${uidir}/main.cpp ${QT_RESOURCES} ${uidir}/session.h  ${uidir}/sessionmodel.h  ${uidir}/sortfiltersessionmodel.h"
"sessionmodel sortfiltersessionmodel ${Qt}::Widgets ${Qt}::Core ${Qt}::Qml ${Qt}::QuickControls2"
)

set(moc "interface" "sessionmodel" "sortfiltersessionmodel")
##########################################################################

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows" AND CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
  message("Build for windows on unix")
  include_directories(/usr/x86_64-w64-mingw32/include/)
  set(LINK_DIRECTORIES ${libdir}/dll /usr/x86_64-w64-mingw32/lib/ /usr/x86_64-w64-mingw32/bin/)
  link_directories(${LINK_DIRECTORIES})
endif()

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
  set(LINK_DIRECTORIES ${libdir}/dll)
  link_directories(${LINK_DIRECTORIES})
  SET(CMAKE_FIND_LIBRARY_PREFIXES "")
  SET(CMAKE_FIND_LIBRARY_SUFFIXES ".lib" ".dll")

  list(TRANSFORM modules APPEND " ws2_32 wsock32" AT 2 3)
  list(APPEND modules "${servicedir}/service.cpp,")

  list(APPEND executables "winservice:${servicedir}/main.cpp")
  list(APPEND executables "service")

  if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  #Хаки чтобы не было зависимостей от дллок на компиляторе MinGW/TDM-GCC
      if(DEFINED STATIC_SQLITE AND "${STATIC_SQLITE}" STREQUAL "true")
          message ("compiling with static sqlite")
          list(TRANSFORM executables REPLACE "-lsqlite3" "-static-libgcc -static-libstdc++ -Wl,-Bstatic,--whole-archive -lssp -lwinpthread -lsqlite3  -Wl,--no-whole-archive" AT 1)
      else()
          list(TRANSFORM executables APPEND " -static-libgcc -static-libstdc++ -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive" AT 1)
      endif()
  endif()
endif()

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
    message("Build for windows on windows")
endif()

message("\n-==Defining modules==-\n")

set(i 0)
foreach (module IN LISTS modules)
    if (NOT ${module} MATCHES ".*\,.*")
        message( FATAL_ERROR "module " ${i} ": "  ${module} " has wrong format")
    endif()
    string(REPLACE "," ";" module ${module})

    list(GET module 0 path)
    if (NOT EXISTS ${path})
        message( FATAL_ERROR "module " ${i} ": "  ${path} " does not exist")
    endif()

    get_filename_component(name ${path} NAME_WE)
    message ("\nModule " ${name} " at " ${path} "\nDependencies:")

    if(${name} IN_LIST moc)
      qt_wrap_cpp(path ${path})
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
  string(REPLACE ":" ";" exec ${exec})
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
