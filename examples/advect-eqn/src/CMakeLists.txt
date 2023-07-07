set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(
    ${PROJECT_NAME}
        AdvectionEquation.cpp
        InflowBC.cpp
        OutflowBC.cpp
        main.cpp
)
target_sanitization(${PROJECT_NAME})

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
    PUBLIC
        godzilla
    PRIVATE
        ${PETSC_LIBRARY}
)

if (DEFINED ENV{CONDA_PREFIX})
    set_target_properties(${PROJECT_NAME} PROPERTIES BUILD_RPATH $ENV{CONDA_PREFIX}/lib)
endif()
