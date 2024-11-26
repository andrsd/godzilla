// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Exception.h"
#include "fmt/format.h"
#include <filesystem>
#include <vector>
#include <string>
#include <dlfcn.h>

#define EXTENSION_API extern "C"

namespace godzilla {

/// Dynamically loadable library
///
/// This provides capabilities to load a shared library (also known as dynamic library) and obtain
/// C-functions that can be then called.
class DynamicLibrary {
public:
    template <typename SIGNATURE>
    class Delegate;

    /// Delegate class for calling C functions
    ///
    /// @tparam RET Return type
    /// @tparam ARGS Function arguments
    template <typename RET, typename... ARGS>
    class Delegate<RET(ARGS...)> {
    public:
        Delegate() = default;
        Delegate(const Delegate & other) = default;
        auto operator=(const Delegate & other) -> Delegate & = default;

        auto
        invoke(ARGS... args) const -> RET
        {
            CALL_STACK_MSG();
            if (this->stub == nullptr)
                throw Exception("Bad delegate call");
            return (*this->stub)(args...);
        }

        /// Bind this delegate to a C function
        ///
        /// @tparam SIGNATURE Function signature
        /// @param fn Pointer to the C function to bind
        template <typename SIGNATURE>
        auto
        bind(SIGNATURE * fn) -> void
        {
            CALL_STACK_MSG();
            this->stub = fn;
        }

    private:
        using StubFunction = RET (*)(ARGS...);
        /// A pointer to the function to invoke
        StubFunction stub = nullptr;
    };

    /// Construct a dynamic library instance
    ///
    /// @param lib_name Library name (the part after `lib` on Unix-based systems, without the file
    /// extension). For example, if your library is named `libExt.so`, you would provide `Ext` here.
    explicit DynamicLibrary(const std::string & lib_name);

    /// Load the library
    void load();

    /// Unload the library
    void unload();

    /// Get symbol as a function delegate
    ///
    /// Usage:
    /// ```
    /// DynamicLibrary dll(<name>);
    /// dll.load();
    /// auto add = dll.get_symbol<double(double, double)>("add");
    /// double res = add.invoke(10, 20);
    /// ```
    ///
    /// @tparam SIGNATURE function signature
    /// @param symbol_name symbol in the library
    /// @return `Delegate` that can call the function
    template <typename SIGNATURE>
    auto
    get_symbol(const char * symbol_name)
    {
        CALL_STACK_MSG();
        SIGNATURE * smbl;
        *(void **) (&smbl) = dlsym(this->handle, symbol_name);
        auto error = dlerror();
        if (error != nullptr || smbl == nullptr)
            throw Exception(fmt::format("Unable to locate '{}' in {}: {}",
                                        symbol_name,
                                        this->file_name,
                                        error));

        Delegate<SIGNATURE> d;
        d.template bind<SIGNATURE>(smbl);
        return d;
    }

public:
    /// Add a new path that will be searched for extensions to load
    ///
    /// @param new_path New path to search for extensions
    static void add_search_path(const std::filesystem::path & new_path);

    /// Remove all search paths
    static void clear_search_paths();

    /// Get search path
    static const std::vector<std::filesystem::path> & get_search_paths();

private:
    /// Get extension file path
    std::string get_ext_file_path() const;

    /// Extension file name
    std::string file_name;
    /// Extension handle
    void * handle;

    /// List of paths to search for the extension
    static std::vector<std::filesystem::path> search_paths;
};

} // namespace godzilla
