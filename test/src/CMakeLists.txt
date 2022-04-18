if (DEFINED ENV{CONDA_PREFIX})
    set(CMAKE_BUILD_RPATH $ENV{CONDA_PREFIX}/lib)
endif()

add_executable(${PROJECT_NAME}
    Array_test.cpp
    CallStack_test.cpp
    Factory_test.cpp
    GodzillaApp_test.cpp
    GYMLFile_test.cpp
    InputParameters_test.cpp
    Logger_test.cpp
    Object_test.cpp
    PrintInterface_test.cpp
    Utils_test.cpp
    main.cpp
)
target_code_coverage(${PROJECT_NAME})
target_sanitization(${PROJECT_NAME})

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
        ${GODZILLA_INCLUDE_DIRS}
        ${CONDA_INCLUDE_DIR}
        ${CMAKE_SOURCE_DIR}/test/include
        ${CMAKE_BINARY_DIR}
)

target_link_libraries(
    ${PROJECT_NAME}
    PUBLIC
        ${GODZILLA_LIBRARIES}
        gtest_main
        gmock_main
)
