// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/RestartFile.h"
#include "godzilla/Enums.h"
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

} // namespace godzilla
