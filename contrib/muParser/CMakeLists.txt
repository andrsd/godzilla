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

install(
    DIRECTORY
        ${PROJECT_SOURCE_DIR}
    DESTINATION
        ${CMAKE_INSTALL_INCLUDEDIR}/godzilla
    FILES_MATCHING
        PATTERN "*.h"
)
