# Test ExodusII files against gold values
#

find_program(
    EXODIFF exodiff
    DOC "exodiff executable"
)
mark_as_advanced(FORCE
    EXODIFF
)
if (${EXODIFF} STREQUAL EXODIFF-NOTFOUND)
    message(FATAL_ERROR "Unable to find exodiff")
endif()

# Adds an EXODIFF test
#
# Params:
# - NAME - Name of the test
# - BIN - Executable to run
# - INPUT - Input file to pass to the binary via `-i` parameter
# - OUTPUT - Output file to compare against the gold file
# - GOLD - File with the expected values
function(add_test_exodiff)
    include(CMakeParseArguments)
    cmake_parse_arguments(
        EXODIFFT
        ""
        "NAME;OUTPUT;GOLD"
        "COMMAND;ARGS"
        ${ARGN}
    )

    list(GET EXODIFFT_COMMAND 0 first_cmd)
    if(TARGET ${first_cmd})
        list(REMOVE_AT EXODIFFT_COMMAND 0)
        set(EXODIFFT_COMMAND "$<TARGET_FILE:${first_cmd}>" ${EXODIFFT_COMMAND})
    endif()

    string(JOIN "\;" CMD_STR ${EXODIFFT_COMMAND})
    string(JOIN "\;" ARG_STR ${EXODIFFT_ARGS})
    add_test(
        NAME
            ${EXODIFFT_NAME}
        COMMAND
            ${CMAKE_COMMAND}
            -DCMD=${CMD_STR}
            -DOUTPUT=${EXODIFFT_OUTPUT}
            -DGOLD=${EXODIFFT_GOLD}
            -DARGS=${ARG_STR}
            -P ${CMAKE_SOURCE_DIR}/cmake/TesterExodusII.cmake
    )
endfunction()

# The Exodiff tester
#
# This will:
#   1. run the provided executable
#   2. compare its output to the expected values
if(
    DEFINED CMD AND
    DEFINED OUTPUT AND
    DEFINED GOLD AND
    DEFINED ARGS
)
    execute_process(
        COMMAND ${CMD}
        RESULT_VARIABLE CMD_RESULT
        OUTPUT_VARIABLE BIN_OUT
        ERROR_VARIABLE BIN_ERR
    )
    if(NOT ${CMD_RESULT} EQUAL 0)
        message(FATAL_ERROR ${CMD_RESULT})
    endif()
    execute_process(
        COMMAND ${EXODIFF} ${ARGS} ${GOLD} ${OUTPUT}
        RESULT_VARIABLE CMD_RESULT
        OUTPUT_VARIABLE DIFF_OUT
    )
    if(NOT ${CMD_RESULT} EQUAL 0)
        message(FATAL_ERROR ${DIFF_OUT})
    endif()
endif()
