// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/DynamicLibrary.h"
#include "godzilla/CallStack.h"

namespace fs = std::filesystem;

namespace godzilla {

std::vector<std::filesystem::path> DynamicLibrary::search_paths_;

DynamicLibrary::DynamicLibrary(String lib_name) : handle_(nullptr)
{
#ifdef __APPLE__
    this->file_name_ = fmt::format("lib{}.dylib", lib_name);
#else
    this->file_name_ = fmt::format("lib{}.so", lib_name);
#endif
}

void
DynamicLibrary::load()
{
    CALL_STACK_MSG();
    auto ext_file_path = get_ext_file_path();
    this->handle_ = dlopen(ext_file_path.c_str(), RTLD_NOW);
    if (!this->handle_)
        throw Exception(fmt::format("Unable to load {}: {}", this->file_name_.string(), dlerror()));
    // Clear any existing error
    dlerror();
}

void
DynamicLibrary::unload()
{
    CALL_STACK_MSG();
    dlclose(this->handle_);
    this->handle_ = nullptr;
}

void
DynamicLibrary::add_search_path(const std::filesystem::path & new_path)
{
    CALL_STACK_MSG();
    for (auto & path : search_paths_)
        if (path == new_path)
            return;
    search_paths_.push_back(new_path);
}

void
DynamicLibrary::clear_search_paths()
{
    CALL_STACK_MSG();
    search_paths_.clear();
}

const std::vector<std::filesystem::path> &
DynamicLibrary::get_search_paths()
{
    CALL_STACK_MSG();
    return search_paths_;
}

fs::path
DynamicLibrary::get_ext_file_path() const
{
    CALL_STACK_MSG();
    // first, look at our paths
    for (auto & path : search_paths_) {
        fs::path ext_file_path = path / this->file_name_.c_str();
        if (fs::exists(ext_file_path))
            return ext_file_path;
    }
    // not found, so try to use system paths
    return this->file_name_;
}

} // namespace godzilla
