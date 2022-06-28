cmake_minimum_required(VERSION 3.1)

project(socksS5Proxy)

set (BOOST_ROOT "C:/Libraries/C++_libs/boost_1_79_0")
set (P7__ROOT "C:/Libraries/C++_libs/libP7Client_v5.6/Sources")

include_directories (SYSTEM ${Boost_INCLUDE_DIR})
add_executable(${PROJECT_NAME} ${PROJ_SOURCES})
target_link_libraries (${PROJECT_NAME} ${Boost_LIBRARIES})