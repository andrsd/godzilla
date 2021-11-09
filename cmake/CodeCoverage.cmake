
option(CODE_COVERAGE "Builds targets with code coverage instrumentation. (Requires Clang)" OFF)

if(CODE_COVERAGE)

find_program(LCOV_PATH lcov)
find_program(LLVM_COV_PATH llvm-cov)
find_program(LLVM_PROFDATA_PATH llvm-profdata)
find_program(GENHTML_PATH genhtml)
mark_as_advanced(FORCE LCOV_PATH GENHTML_PATH)

set(CODE_COVERAGE_PROFRAWS
    ${CMAKE_BINARY_DIR}/test/godzilla-test.profraw
)
set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_CLEAN_FILES "${CODE_COVERAGE_PROFRAWS}")

set(EXCLUDE_REGEX
    --ignore-filename-regex=/contrib/
    --ignore-filename-regex=/include/petsc*
    --ignore-filename-regex=/include/mpi*
    --ignore-filename-regex=/include/yaml-cpp/*
    --ignore-filename-regex=/include/gtest/*
    --ignore-filename-regex=/include/gmock/*
    --ignore-filename-regex=test/*
    --ignore-filename-regex=src/base/CallStack.cpp
)

set(CODE_COVERAGE_BINS
    --object=${PROJECT_BINARY_DIR}/test/godzilla-test
    --object=${PROJECT_BINARY_DIR}/libgodzilla.dylib
)

set(COVERAGE_INFO ${PROJECT_BINARY_DIR}/coverage.info)
set(MERGED_PROFDATA ${PROJECT_BINARY_DIR}/all-merged.profdata)


add_custom_target(coverage DEPENDS ${COVERAGE_INFO})

add_custom_command(
    OUTPUT
        ${COVERAGE_INFO}
    COMMAND
        ${LLVM_COV_PATH}
        export
        ${CODE_COVERAGE_BINS}
        -instr-profile=${MERGED_PROFDATA}
        -format="lcov"
        ${EXCLUDE_REGEX}
        > ${COVERAGE_INFO}
    DEPENDS
        ${MERGED_PROFDATA}
)

add_custom_command(
    OUTPUT
        ${MERGED_PROFDATA}
    COMMAND
        ${LLVM_PROFDATA_PATH}
        merge
        -sparse
        ${CODE_COVERAGE_PROFRAWS}
        -o ${MERGED_PROFDATA}
    DEPENDS
        ${CODE_COVERAGE_PROFRAWS}
)

add_custom_target(htmlcov DEPENDS ${PROJECT_BINARY_DIR}/htmlcov/index.html)
add_custom_command(
    OUTPUT
        ${PROJECT_BINARY_DIR}/htmlcov/index.html
    COMMAND
        ${LLVM_COV_PATH}
        show
        ${CODE_COVERAGE_BINS}
        -instr-profile=${MERGED_PROFDATA}
        -show-line-counts-or-regions
        -output-dir=${CMAKE_BINARY_DIR}/htmlcov
        -format="html"
        ${EXCLUDE_REGEX}
    DEPENDS
        ${COVERAGE_INFO}
)

add_custom_command(
    TARGET htmlcov
    POST_BUILD
    COMMAND ;
    COMMENT
        "Open ${PROJECT_BINARY_DIR}/htmlcov/index.html in your browser to view the coverage report."
)

function(target_code_coverage TARGET_NAME)
    if(CMAKE_C_COMPILER_ID MATCHES "(Apple)?[Cc]lang" OR CMAKE_CXX_COMPILER_ID MATCHES "(Apple)?[Cc]lang")
        target_compile_options(${TARGET_NAME} PUBLIC -fprofile-instr-generate -fcoverage-mapping)
        target_link_options(${TARGET_NAME} PUBLIC -fprofile-instr-generate -fcoverage-mapping)
    endif()
endfunction()

function(add_test_with_coverage)
    # TODO: wrap add_test so it does this:
    #   add_test(${TEST_NAME} ARGS)
    #   set_tests_properties(${TEST_NAME} PROPERTIES ENVIRONMENT LLVM_PROFILE_FILE=${TEST_NAME}.profraw)
endfunction()

else()

function(target_code_coverage TARGET_NAME)
endfunction()

function(add_test_with_coverage)
endfunction()

endif()
