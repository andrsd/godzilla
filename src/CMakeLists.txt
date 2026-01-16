file(GLOB SRCS CONFIGURE_DEPENDS *.cpp)

add_library(${PROJECT_NAME} SHARED ${SRCS})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

target_code_coverage(${PROJECT_NAME})
target_sanitization(${PROJECT_NAME})
target_compile_options(${PROJECT_NAME}
    PRIVATE
        -Wall
        -ffile-prefix-map=${CMAKE_SOURCE_DIR}/include=include
        -ffile-prefix-map=${CMAKE_SOURCE_DIR}/src=godzilla/src
)

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

target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        HAVE_CXXABI_H
        GODZILLA_VERSION="${PROJECT_VERSION}"
)

if (PETSC_HAVE_HYPRE)
    target_compile_definitions(${PROJECT_NAME} PRIVATE -DPETSC_HAVE_HYPRE)
endif()

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_SOURCE_DIR}/contrib
)

target_link_libraries(
    ${PROJECT_NAME}
    PUBLIC
        fmt::fmt
        spdlog::spdlog
        mpicpp-lite::mpicpp-lite
        HDF5::HDF5
        PETSc::petsc
    PRIVATE
        exodusIIcpp::exodusIIcpp
        yaml-cpp::yaml-cpp
)
if (GODZILLA_WITH_MPI)
    target_include_directories(${PROJECT_NAME} PRIVATE ${MPI_C_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} PRIVATE MPI::MPI_C)
endif()
if (GODZILLA_WITH_TECIOCPP)
    target_compile_definitions(${PROJECT_NAME} PUBLIC GODZILLA_WITH_TECIOCPP)
    target_link_libraries(${PROJECT_NAME} PUBLIC teciocpp::teciocpp)
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
    COMPATIBILITY SameMajorVersion
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
