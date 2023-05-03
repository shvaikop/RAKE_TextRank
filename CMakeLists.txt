cmake_minimum_required(VERSION 3.22.1)
project(project)

set(CMAKE_CXX_STANDARD 20)

add_executable(project
               src/main.cpp
               src/Rake.cpp
               src/TextPreprocess.cpp
               src/TextRank.cpp)

target_link_libraries(project
        boost_program_options)
