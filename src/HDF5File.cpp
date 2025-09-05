// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/HDF5File.h"
#include "godzilla/Enums.h"

namespace godzilla {

HDF5File::HDF5File(const fs::path & file_name, FileAccess faccess) : file_name(file_name)
{
    if (faccess == FileAccess::READ)
        this->id = H5Fopen(file_name.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
    else if (faccess == FileAccess::WRITE)
        this->id = H5Fopen(file_name.c_str(), H5F_ACC_RDWR, H5P_DEFAULT);
    else if (faccess == FileAccess::CREATE)
        this->id = H5Fcreate(file_name.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    else
        throw Exception("Unsupported file access");

    if (this->id == H5I_INVALID_HID)
        throw Exception("Unable to open {} or it is not a valid HDF5 file.", file_name.string());
}

HDF5File::~HDF5File()
{
    H5Fclose(this->id);
}

std::string
HDF5File::get_file_name() const
{
    return this->file_name.filename().string();
}

std::string
HDF5File::get_file_path() const
{
    return this->file_name.string();
}

} // namespace godzilla
