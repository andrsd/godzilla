project(poisson)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(
    poisson
        src/PoissonApp.cpp
        src/PoissonEquation.cpp
        src/main.cpp
)

target_compile_options(
    poisson
    PRIVATE
        -ffile-prefix-map=${CMAKE_SOURCE_DIR}/include=include
        -ffile-prefix-map=${CMAKE_SOURCE_DIR}/src=godzilla/src
        -ffile-prefix-map=${PROJECT_SOURCE_DIR}=${PROJECT_NAME}
)

target_include_directories(
    poisson
    PUBLIC
        ${CMAKE_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/contrib
)

target_link_libraries(
    poisson
    PRIVATE
        godzilla
)
