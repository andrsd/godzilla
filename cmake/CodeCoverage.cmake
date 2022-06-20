
option(GODZILLA_CODE_COVERAGE "Builds targets with code coverage instrumentation" OFF)

if(GODZILLA_CODE_COVERAGE)

    set(COVERAGE_INFO ${PROJECT_BINARY_DIR}/coverage.info)
    mark_as_advanced(FORCE
        COVERAGE_INFO
    )
    set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_CLEAN_FILES "${COVERAGE_INFO}")

    if(CMAKE_C_COMPILER_ID MATCHES "(Apple)?[Cc]lang" OR CMAKE_CXX_COMPILER_ID MATCHES "(Apple)?[Cc]lang")
        find_program(
            LLVM_COV_PATH
            NAMES
                llvm-cov
        )
        find_program(
            LLVM_PROFDATA_PATH
            NAMES
                llvm-profdata
        )
        mark_as_advanced(FORCE
            LLVM_COV_PATH
            LLVM_PROFDATA_PATH
        )

        set(CODE_COVERAGE_PROFRAWS
            ${CMAKE_BINARY_DIR}/test/godzilla-test.profraw
            ${CMAKE_BINARY_DIR}/examples/poisson/test/default.profraw
            ${CMAKE_BINARY_DIR}/examples/heat-eqn/test/default.profraw
            ${CMAKE_BINARY_DIR}/examples/burgers-eqn/test/default.profraw
        )
        set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_CLEAN_FILES "${CODE_COVERAGE_PROFRAWS}")

        set(EXCLUDE_REGEX
            --ignore-filename-regex=/contrib/
            --ignore-filename-regex=/include/petsc*
            --ignore-filename-regex=/include/mpi*
            --ignore-filename-regex=/include/yaml-cpp/*
            --ignore-filename-regex=/include/gtest/*
            --ignore-filename-regex=/include/gmock/*
            --ignore-filename-regex=/include/boost/*
            --ignore-filename-regex=test/*
            --ignore-filename-regex=/muparser/*
        )

        set(CODE_COVERAGE_BINS
            --object=${PROJECT_BINARY_DIR}/test/src/godzilla-test
            --object=${PROJECT_BINARY_DIR}/src/libgodzilla${CMAKE_SHARED_LIBRARY_SUFFIX}
        )

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
            target_compile_options(${TARGET_NAME} PUBLIC -fprofile-instr-generate -fcoverage-mapping)
            target_link_options(${TARGET_NAME} PUBLIC -fprofile-instr-generate -fcoverage-mapping)
        endfunction()

        function(add_test_with_coverage)
            # TODO: wrap add_test so it does this:
            #   add_test(${TEST_NAME} ARGS)
            #   set_tests_properties(${TEST_NAME} PROPERTIES ENVIRONMENT LLVM_PROFILE_FILE=${TEST_NAME}.profraw)
        endfunction()

    elseif(CMAKE_C_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        find_program(GCOV_PATH NAMES gcov)
        find_program(LCOV_PATH lcov)
        find_program(GENHTML_PATH genhtml)
        mark_as_advanced(FORCE
            GCOV_PATH
            LCOV_PATH
            GENHTML_PATH
        )

        set(EXCLUDE_REGEX
            --exclude=*/contrib/*
            --exclude=*/include/petsc*
            --exclude=*/include/gtest/*
            --exclude=*/include/gmock/*
            --exclude=*/include/yaml-cpp/*
            --exclude=/usr/include/*
            --exclude=*muparser*
        )

        add_custom_target(coverage DEPENDS ${COVERAGE_INFO})

        add_custom_command(
            OUTPUT
                ${COVERAGE_INFO}
            COMMAND
                ${LCOV_PATH}
                --capture
                --directory src
                --directory test
                --directory examples
                --output-file ${COVERAGE_INFO}
                ${EXCLUDE_REGEX}
        )

        add_custom_target(htmlcov DEPENDS ${PROJECT_BINARY_DIR}/htmlcov/index.html)
        add_custom_command(
            OUTPUT
                ${PROJECT_BINARY_DIR}/htmlcov/index.html
            COMMAND
                ${GENHTML_PATH}
                --output-directory=${CMAKE_BINARY_DIR}/htmlcov
                ${COVERAGE_INFO}
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
            target_compile_options(${TARGET_NAME} PUBLIC -fprofile-arcs -ftest-coverage)
            target_link_options(${TARGET_NAME} PUBLIC -fprofile-arcs -ftest-coverage)
        endfunction()

        function(add_test_with_coverage)
        endfunction()

    else()
        message(STATUS, "Code coverage for your compiler (${CMAKE_C_COMPILER_ID}) is not supported.")
    endif()

else()

    function(target_code_coverage TARGET_NAME)
    endfunction()

    function(add_test_with_coverage)
    endfunction()

endif()
