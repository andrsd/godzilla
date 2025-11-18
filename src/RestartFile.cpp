// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/RestartFile.h"
#include "godzilla/Enums.h"
#include "godzilla/Vector.h"
#include "fmt/format.h"
#include <filesystem>

namespace fs = std::filesystem;

namespace godzilla {

RestartFile::RestartFile(const std::string & file_name, FileAccess faccess) :
    h5f(fs::path(file_name), faccess)
{
}

std::string
RestartFile::file_name() const
{
    return this->h5f.get_file_name();
}

std::string
RestartFile::file_path() const
{
    return this->h5f.get_file_path();
}

std::string
RestartFile::get_full_path(const std::string & app_name, const std::string & path) const
{
    if (path == "/" || path == "")
        return fmt::format("/{}", app_name);
    else
        return fmt::format("/{}/{}", app_name, path);
}

std::string
RestartFile::normalize_path(const std::string & path) const
{
    if (path == "")
        return fmt::format("/{}", path);
    else
        return path;
}

template <>
void
RestartFile::write<Vector>(const std::string & path, const std::string & name, const Vector & data)
{
    auto norm_path = normalize_path(path);
    try {
        auto group = this->h5f.create_group(norm_path);
        auto * vals = data.get_array_read();
        auto len = data.get_local_size();
        group.write_dataset(name, len, vals);
        data.restore_array_read(vals);
    }
    catch (std::exception & e) {
        throw Exception("Error writing '{}' to {}: {}", norm_path, this->file_name(), e.what());
    }
}

template <>
void
RestartFile::read<Vector>(const std::string & path, const std::string & name, Vector & data) const
{
    auto norm_path = normalize_path(path);
    try {
        auto group = this->h5f.open_group(norm_path);
        auto * vals = data.get_array();
        auto len = data.get_local_size();
        group.read_dataset(name, len, vals);
        data.restore_array(vals);
    }
    catch (std::exception & e) {
        throw Exception("Error writing '{}' to {}: {}", norm_path, this->file_name(), e.what());
    }
}

} // namespace godzilla
