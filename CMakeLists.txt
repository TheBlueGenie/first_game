cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_REQUIRED ON)


project(sfml-template)
add_executable(sfml-template src/main.cpp)

set_target_properties(
  sfml-template 
  PROPERTIES
    OUTPUT_NAME game
)

target_include_directories(sfml-template PUBLIC /usr/include/SFML)
target_link_libraries(sfml-template sfml-system sfml-window sfml-graphics)
