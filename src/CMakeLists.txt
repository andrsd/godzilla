file(GLOB SRCS CONFIGURE_DEPENDS *.cpp)

add_library(${PROJECT_NAME} SHARED ${SRCS})

target_code_coverage(${PROJECT_NAME})
target_sanitization(${PROJECT_NAME})

set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
        UNITY_BUILD_MODE BATCH
)

set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
)

if (PETSC_HAVE_HYPRE)
    target_compile_definitions(${PROJECT_NAME} PRIVATE -DPETSC_HAVE_HYPRE)
endif()

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/contrib
)

target_link_libraries(
    ${PROJECT_NAME}
    PUBLIC
        fmt::fmt
        mpicpp-lite::mpicpp-lite
    PRIVATE
        PETSc::petsc
        muParser
        exodusIIcpp::exodusIIcpp
        yaml-cpp::yaml-cpp
)
if (GODZILLA_WITH_MPI)
    target_include_directories(${PROJECT_NAME} PRIVATE ${MPI_C_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} PRIVATE MPI::MPI_C)
endif()


# install

configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/cmake/godzilla-config.cmake.in
    godzilla-config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/godzilla
    NO_SET_AND_CHECK_MACRO
)
write_basic_package_version_file(
    godzilla-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(
    TARGETS godzilla
    EXPORT GodzillaTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    DIRECTORY ${CMAKE_SOURCE_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h"
)

install(
    FILES ${CMAKE_BINARY_DIR}/godzilla/Config.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/godzilla
)

install(
    DIRECTORY ${CMAKE_SOURCE_DIR}/contrib/cxxopts
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/godzilla
    FILES_MATCHING PATTERN "*.hpp"
)

install(
    EXPORT GodzillaTargets
    FILE godzilla-targets.cmake
    NAMESPACE godzilla::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/godzilla
)

install(
    FILES
    "${CMAKE_CURRENT_BINARY_DIR}/godzilla-config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/godzilla-config-version.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/godzilla
)
