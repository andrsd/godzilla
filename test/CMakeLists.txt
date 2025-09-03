project(godzilla-test)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(GTest)

file(GLOB SRCS CONFIGURE_DEPENDS
    common/*.cpp
    src/*.cpp
)

add_executable(${PROJECT_NAME}
    ${SRCS}
    main.cpp
)

target_code_coverage(${PROJECT_NAME})
target_sanitization(${PROJECT_NAME})

target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        -DGODZILLA_UNIT_TESTS_ROOT="${GODZILLA_UNIT_TESTS_ROOT}"
        -DGODZILLA_UNIT_TESTS_BINARY_DIR="${PROJECT_BINARY_DIR}"
)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
        ${CMAKE_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/contrib
        ${PROJECT_SOURCE_DIR}/common
)

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
        yaml-cpp::yaml-cpp
        godzilla
        PETSc::petsc
        mpicpp-lite::mpicpp-lite
        h5pp::h5pp
        ${CMAKE_DL_LIBS}
        GTest::gmock_main
)
if (GODZILLA_WITH_MPI)
    target_link_libraries(${PROJECT_NAME} PRIVATE MPI::MPI_C)
endif()

add_test(
    NAME godzilla-test
    COMMAND ${PROJECT_NAME}
)
if(GODZILLA_CODE_COVERAGE)
    set_tests_properties(
        godzilla-test
        PROPERTIES
            ENVIRONMENT LLVM_PROFILE_FILE=${PROJECT_NAME}.profraw
    )
endif()

add_subdirectory(ext)
