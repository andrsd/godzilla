# Test HDF5 files against gold values
#

find_program(
    H5DIFF h5diff
    DOC "h5diff executable"
)

# Adds an HDF5DIFF test
#
# Params:
# - NAME - Name of the test
# - BIN - Executable to run
# - INPUT - Input file to pass to the binary via `-i` parameter
# - OUTPUT - Output file to compare against the gold file
# - GOLD - File with the expected values
function(add_test_hdf5diff)
    include(CMakeParseArguments)
    cmake_parse_arguments(
        H5T
        ""
        "NAME;BIN;INPUT;OUTPUT;GOLD"
        ""
        ${ARGN}
    )

    add_test(
        NAME
            ${H5T_NAME}
        COMMAND
            ${CMAKE_COMMAND}
            -DBIN=$<TARGET_FILE:${H5T_BIN}>
            -DINPUT=${H5T_INPUT}
            -DOUTPUT=${H5T_OUTPUT}
            -DGOLD=${H5T_GOLD}
            -P ${CMAKE_SOURCE_DIR}/cmake/TesterHDF5.cmake
    )
endfunction()

# The HDF5 tester
#
# This will:
#   1. run the provided exeecutable
#   2. compare its output to the expected values
if(
    DEFINED BIN AND
    DEFINED INPUT AND
    DEFINED OUTPUT AND
    DEFINED GOLD
)
    execute_process(
        COMMAND ${BIN} -i ${INPUT}
        RESULT_VARIABLE CMD_RESULT
        OUTPUT_VARIABLE BIN_OUT
        ERROR_VARIABLE BIN_ERR
    )
    if(NOT ${CMD_RESULT} EQUAL 0)
        message(FATAL_ERROR ${CMD_RESULT})
    endif()
    execute_process(
        COMMAND ${H5DIFF} -v ${GOLD} ${OUTPUT}
        RESULT_VARIABLE CMD_RESULT
        OUTPUT_VARIABLE DIFF_OUT
    )
    if(NOT ${CMD_RESULT} EQUAL 0)
        message(FATAL_ERROR ${DIFF_OUT})
    endif()
endif()
