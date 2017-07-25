cmake_minimum_required(VERSION 3.7)
project(SummerCompiler)

set(Boost_USE_STATIC_LIBS ON)   # Includes boost libraries in the executable, not at runtime
find_package(BISON)
find_package(FLEX)
find_package(Boost COMPONENTS program_options system filesystem REQUIRED)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES Driver.cpp Driver.h Register.cpp Register.h its_complicated/components/Expression.h its_complicated/components/LValue.cpp its_complicated/components/LValue.h its_complicated/components/Expression.cpp its_complicated/MetaCoder.cpp its_complicated/MetaCoder.h its_complicated/components/Type.h)
add_executable(SummerCompiler ${SOURCE_FILES})

bison_target(Parser Lex_Parse/parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp)
flex_target(Scanner Lex_Parse/scanner.l ${CMAKE_CURRENT_BINARY_DIR}/scanner.cpp)
add_flex_bison_dependency(Scanner Parser)

include_directories(
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/Lex_Parse
        ${Boost_INCLUDE_DIRS}
)
set(code_src
        ${BISON_Parser_OUTPUTS}
        ${FLEX_Scanner_OUTPUTS})

source_group("interpreter" FILES ${code_src})
set(main_srcs Driver.cpp Driver.h)

source_group("MAIN" FILES )

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(EXTRA_COMPILE_FLAGS "-g -std=c++11")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EXTRA_COMPILE_FLAGS}")

set(CMAKE_INSTALL_RPATH ".")
add_library(interpreter STATIC ${code_src})

set(SOURCE_FILES Driver.cpp Driver.h) # This is where main lives, and essentially, the driver of the program
add_executable(cpslc ${SOURCE_FILES})

target_link_libraries(interpreter ${FLEX_LIBRARIES} ${BISON_LIBRARIES} ${Boost_LIBRARIES})
target_link_libraries(cpslc interpreter)