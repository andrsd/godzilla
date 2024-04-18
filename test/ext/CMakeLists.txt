add_library(ext SHARED main.cpp)

target_include_directories(ext PRIVATE ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(ext PRIVATE godzilla)
