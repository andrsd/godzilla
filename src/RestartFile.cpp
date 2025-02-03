// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/RestartFile.h"
#include "godzilla/Enums.h"
#include "godzilla/Vector.h"
#include "fmt/format.h"
#include <filesystem>

namespace fs = std::filesystem;

namespace {

h5pp::FileAccess
file_access(godzilla::FileAccess fa)
{
    if (fa == godzilla::FileAccess::READ)
        return h5pp::FileAccess::READONLY;
    else if (fa == godzilla::FileAccess::WRITE)
        return h5pp::FileAccess::READWRITE;
    else if (fa == godzilla::FileAccess::CREATE)
        return h5pp::FileAccess::REPLACE;
    else
        throw std::runtime_error("Unknown FileAccess value.");
}

} // namespace

namespace godzilla {

RestartFile::RestartFile(const std::string & file_name, FileAccess faccess) :
    h5f(fs::path(file_name), file_access(faccess))
{
}

std::string
RestartFile::file_name() const
{
    return this->h5f.getFileName();
}

std::string
RestartFile::file_path() const
{
    return this->h5f.getFilePath();
}

std::string
RestartFile::get_full_path(const std::string & app_name, const std::string & path) const
{
    return fmt::format("{}/{}", app_name, path);
}

template <>
void
RestartFile::write<Vector>(const std::string & path, const Vector & data)
{
    try {
        auto * vals = data.get_array_read();
        auto len = data.get_local_size();
        this->h5f.writeDataset(vals, path, len);
        data.restore_array_read(vals);
    }
    catch (std::exception & e) {
        throw Exception("Error writing '{}' to {}: {}", path, this->file_name(), e.what());
    }
}

template <>
void
RestartFile::read<Vector>(const std::string & path, Vector & data) const
{
    try {
        auto * vals = data.get_array();
        auto len = data.get_local_size();
        this->h5f.readDataset(vals, path, len);
        data.restore_array(vals);
    }
    catch (std::exception & e) {
        throw Exception("Error writing '{}' to {}: {}", path, this->file_name(), e.what());
    }
}

} // namespace godzilla
