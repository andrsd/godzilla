cmake_minimum_required(VERSION VERSION 3.16)

project(mesh-part LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(${PROJECT_NAME}
    main.cpp
)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        ${CMAKE_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/contrib
)

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
        godzilla
        PETSc::petsc
        fmt::fmt
)
if(GODZILLA_WITH_MPI)
    target_link_libraries(${PROJECT_NAME} PRIVATE MPI::MPI_C)
endif()

install(TARGETS mesh-part DESTINATION bin)
