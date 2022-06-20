# Documentation

find_package(Doxygen QUIET)

if(DOXYGEN_FOUND)
    if (NOT GODZILLA_RST_DIR)
        set(GODZILLA_RST_DIR ${PROJECT_BINARY_DIR}/docs/rst)
    endif()
    set(DOXYREST_PATH $ENV{DOXYREST_PATH})
    set(DOXYGEN_GENERATE_HTML NO)
    set(DOXYGEN_GENERATE_XML YES)
    set(DOXYGEN_STRIP_FROM_INC_PATH
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/src
    )
    set(DOXYGEN_STRIP_FROM_PATH
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/src
    )

    doxygen_add_docs(
        doxygen
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/src
        STRIP_FROM_PATH
        COMMENT "Generate doxygen XML"
    )

    find_program(DOXYREST doxyrest)
    find_program(SPHINX_BUILD sphinx-build)
    mark_as_advanced(FORCE
        DOXYREST
        SPHINX_BUILD
    )

    configure_file(doxyrest-config.lua.in doxyrest-config.lua)
    configure_file(conf.py rst/conf.py)

    add_custom_target(doc DEPENDS ${PROJECT_BINARY_DIR}/html/index.html)

    add_custom_command(
        OUTPUT
            ${PROJECT_BINARY_DIR}/html/index.html
        COMMAND
            ${SPHINX_BUILD} -b html rst html
        DEPENDS
            ${GODZILLA_RST_DIR}/index.rst
            ${GODZILLA_RST_DIR}/getting-started
            ${GODZILLA_RST_DIR}/examples
            ${GODZILLA_RST_DIR}/devel/index.rst
            ${GODZILLA_RST_DIR}/conf.py
    )

    add_custom_command(
        OUTPUT
            ${GODZILLA_RST_DIR}/devel/index.rst
        COMMAND
            ${DOXYREST} -c doxyrest-config.lua
        DEPENDS
            ${PROJECT_BINARY_DIR}/xml/index.xml
    )

    add_custom_command(
        OUTPUT
            ${GODZILLA_RST_DIR}/getting-started
        COMMAND
            cmake -E copy_directory ${PROJECT_SOURCE_DIR}/docs/getting-started ${GODZILLA_RST_DIR}/getting-started
        DEPENDS
            ${PROJECT_SOURCE_DIR}/docs/getting-started
    )
    add_custom_command(
        OUTPUT
            ${GODZILLA_RST_DIR}/examples
        COMMAND
            cmake -E copy_directory ${PROJECT_SOURCE_DIR}/docs/examples ${GODZILLA_RST_DIR}/examples
        DEPENDS
            ${PROJECT_SOURCE_DIR}/docs/examples
    )
    add_custom_command(
        OUTPUT
            ${GODZILLA_RST_DIR}/index.rst
        COMMAND
            cmake -E copy ${PROJECT_SOURCE_DIR}/docs/index.rst ${GODZILLA_RST_DIR}
        DEPENDS
            ${PROJECT_SOURCE_DIR}/docs/index.rst
    )

    add_custom_command(
        OUTPUT
            ${PROJECT_BINARY_DIR}/xml/index.xml
        COMMAND
            doxygen Doxyfile.doxygen
        DEPENDS
            ${PROJECT_BINARY_DIR}/docs/Doxyfile.doxygen
            ${PROJECT_SOURCE_DIR}/include/*.h
    )

endif()

unset(GODZILLA_RST_DIR CACHE)
