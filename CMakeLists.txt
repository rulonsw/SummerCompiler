cmake_minimum_required(VERSION 3.7)
project(SummerCompiler)

set(Boost_USE_STATIC_LIBS ON)   # Includes boost libraries in the executable, not at runtime
find_package(BISON)
find_package(FLEX)
find_package(Boost COMPONENTS program_options system filesystem REQUIRED)

set(CMAKE_CXX_STANDARD 11)

bison_target(Parser Lex_Parse/parser.y ${CMAKE_CURRENT_SOURCE_DIR}/parser.cpp DEFINES_FILE ${CMAKE_CURRENT_SOURCE_DIR}/parser.h)
flex_target(Scanner Lex_Parse/scanner.l ${CMAKE_CURRENT_SOURCE_DIR}/scanner.cpp)
add_flex_bison_dependency(Scanner Parser)

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/Lex_Parse
        ${CMAKE_CURRENT_SOURCE_DIR}/its_complicated
        ${CMAKE_CURRENT_SOURCE_DIR}/its_complicated/components
        ${Boost_INCLUDE_DIRS}
)
set(code_src
        ${BISON_Parser_OUTPUTS}
        ${FLEX_Scanner_OUTPUTS}
        ${CMAKE_CURRENT_SOURCE_DIR}/its_complicated/components/Expression.h
        ${CMAKE_CURRENT_SOURCE_DIR}/its_complicated/components/Expression.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/its_complicated/components/LValue.h
        ${CMAKE_CURRENT_SOURCE_DIR}/its_complicated/components/LValue.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/its_complicated/MetaCoder.h
        ${CMAKE_CURRENT_SOURCE_DIR}/its_complicated/MetaCoder.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Register.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Register.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Driver.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Driver.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/its_complicated/components/Type.h
        )

source_group("interpreter" FILES ${code_src})
set(main_srcs Driver.cpp Driver.h)
source_group("Main" FILES)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(EXTRA_COMPILE_FLAGS "-g -std=c++11")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EXTRA_COMPILE_FLAGS}")

set(CMAKE_INSTALL_RPATH ".")

add_library(interpreter STATIC ${code_src})

add_executable(SummerCompiler ${main_srcs})

target_link_libraries(interpreter ${FLEX_LIBRARIES} ${BISON_LIBRARIES} ${Boost_LIBRARIES})
target_link_libraries(SummerCompiler interpreter)
