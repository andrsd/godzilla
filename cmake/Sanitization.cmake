# LLVM: Sanitization

option(GODZILLA_ADDRESS_SANITIZATION "Build with address sanitizer (requires clang)" NO)
option(GODZILLA_MEMORY_SANITIZATION "Build with memory sanitizer (requires clang)" NO)

function(target_sanitization TARGET_NAME)
    if (GODZILLA_ADDRESS_SANITIZATION)
        if(CMAKE_C_COMPILER_ID MATCHES "(Apple)?[Cc]lang" OR CMAKE_CXX_COMPILER_ID MATCHES "(Apple)?[Cc]lang")
            target_compile_options(${TARGET_NAME} PUBLIC -fsanitize=address -fno-omit-frame-pointer)
            target_link_options(${TARGET_NAME} PUBLIC -fsanitize=address)
        endif()
    endif()

    if (GODZILLA_MEMORY_SANITIZATION)
        if(CMAKE_C_COMPILER_ID MATCHES "(Apple)?[Cc]lang" OR CMAKE_CXX_COMPILER_ID MATCHES "(Apple)?[Cc]lang")
            target_compile_options(${TARGET_NAME} PUBLIC -fsanitize=leak -fno-omit-frame-pointer)
            target_link_options(${TARGET_NAME} PUBLIC -fsanitize=leak)
        endif()
    endif()
endfunction()
