project(ns-incomp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB SRCS CONFIGURE_DEPENDS src/*.cpp)

add_executable(${PROJECT_NAME} ${SRCS})

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
    PRIVATE
        godzilla
        PETSc::petsc
)

if (GODZILLA_BUILD_TESTS)
    # temporarily disbaled
    # add_subdirectory(test)
endif()
