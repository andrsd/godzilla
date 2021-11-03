
option(ADDRESS_SANITIZATION "Build with address sanitizer (required clang)" NO)

if (ADDRESS_SANITIZATION)
    function(target_address_sanitization TARGET_NAME)
        if(CMAKE_C_COMPILER_ID MATCHES "(Apple)?[Cc]lang" OR CMAKE_CXX_COMPILER_ID MATCHES "(Apple)?[Cc]lang")
            target_compile_options(${TARGET_NAME} PUBLIC -fsanitize=address -fno-omit-frame-pointer)
            target_link_options(${TARGET_NAME} PUBLIC -fsanitize=address)
        endif()
    endfunction()
else()
    function(target_address_sanitization TARGET_NAME)
    endfunction()
endif()
