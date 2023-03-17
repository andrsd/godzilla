project(muParser)

add_library(${PROJECT_NAME}
    STATIC
        muParser.cpp
        muParserBase.cpp
        muParserBytecode.cpp
        muParserCallback.cpp
        muParserError.cpp
        muParserTokenReader.cpp
)

set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)

install(
    DIRECTORY
        ${PROJECT_SOURCE_DIR}
    DESTINATION
        ${CMAKE_INSTALL_INCLUDEDIR}/godzilla
    FILES_MATCHING
        PATTERN "*.h"
)
