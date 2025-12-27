// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/RestartFile.h"
#include "godzilla/Enums.h"
#include "godzilla/Vector.h"
#include "fmt/format.h"
#include <filesystem>

namespace fs = std::filesystem;

namespace godzilla {

RestartFile::RestartFile(mpi::Communicator comm, fs::path file_name, FileAccess faccess) :
    h5f(comm, file_name, faccess)
{
}

RestartFile::RestartFile(fs::path file_name, FileAccess faccess) : h5f(file_name, faccess) {}

fs::path
RestartFile::file_name() const
{
    return this->h5f.get_file_name();
}

fs::path
RestartFile::file_path() const
{
    return this->h5f.get_file_path();
}

String
RestartFile::get_full_path(String app_name, String path) const
{
    if (path == "/" || path == "")
        return fmt::format("/{}", app_name);
    else
        return fmt::format("/{}/{}", app_name, path);
}

String
RestartFile::normalize_path(String path) const
{
    if (path == "")
        return fmt::format("/{}", path);
    else
        return path;
}

template <>
void
RestartFile::write<Vector>(String path, String name, const Vector & data)
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
        throw Exception("Error writing '{}' to {}: {}",
                        norm_path,
                        this->file_name().string(),
                        e.what());
    }
}

void
RestartFile::write_global_vector(String app_name, String path, String name, const Vector & data)
{
    write_global_vector(get_full_path(app_name, path), name, data);
}

void
RestartFile::write_global_vector(String path, String name, const Vector & data)
{
    auto norm_path = normalize_path(path);
    try {
        auto group = this->h5f.create_group(norm_path);
        group.write_global_vector(name, data);
    }
    catch (std::exception & e) {
        throw Exception("Error writing '{}' to {}: {}",
                        norm_path,
                        this->file_name().string(),
                        e.what());
    }
}

template <>
void
RestartFile::read<Vector>(String path, String name, Vector & data) const
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
        throw Exception("Error writing '{}' to {}: {}",
                        norm_path,
                        this->file_name().string(),
                        e.what());
    }
}

void
RestartFile::read_global_vector(String path, String name, Vector & data) const
{
    auto norm_path = normalize_path(path);
    try {
        auto group = this->h5f.open_group(norm_path);
        group.read_global_vector(name, data);
    }
    catch (std::exception & e) {
        throw Exception("Error reading '{}/{}' from {}: {}",
                        norm_path,
                        name,
                        this->file_name().string(),
                        e.what());
    }
}

void
RestartFile::read_global_vector(String app_name, String path, String name, Vector & data) const
{
    read_global_vector(get_full_path(app_name, path), name, data);
}

} // namespace godzilla
