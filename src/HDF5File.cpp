// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/HDF5File.h"
#include "godzilla/CallStack.h"
#include "godzilla/Enums.h"

namespace godzilla {

HDF5File::HDF5File(mpi::Communicator comm, const fs::path & file_name, FileAccess faccess) :
    file_name(file_name)
{
    MPI_Info info = MPI_INFO_NULL;
    auto fapl = H5Pcreate(H5P_FILE_ACCESS);
    H5Pset_fapl_mpio(fapl, comm, info);

    if (faccess == FileAccess::READ)
        this->id = H5Fopen(file_name.c_str(), H5F_ACC_RDONLY, fapl);
    else if (faccess == FileAccess::WRITE)
        this->id = H5Fopen(file_name.c_str(), H5F_ACC_RDWR, fapl);
    else if (faccess == FileAccess::CREATE)
        this->id = H5Fcreate(file_name.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, fapl);
    else
        throw Exception("Unsupported file access");

    if (this->id == H5I_INVALID_HID)
        throw Exception("Unable to open {} or it is not a valid HDF5 file.", file_name.string());
}

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
    if (this->id != H5I_INVALID_HID)
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

void
HDF5File::Group::write_global_vector(const std::string & name, const Vector & data)
{
    auto rng = data.get_ownership_range();

    auto space = Dataspace::create(data.get_size());
    auto dset = Dataset::create<Real>(this->id, name.c_str(), space);

    auto filespace = dset.get_space();
    filespace.select_hyperslab(rng);

    auto memspace = Dataspace::create(rng.size());

    auto * vals = data.get_array_read();
    dset.write(memspace, filespace, vals);
    data.restore_array_read(vals);
}

void
HDF5File::Group::read_global_vector(const std::string & name, Vector & data)
{
    auto rng = data.get_ownership_range();

    auto dset = Dataset::open(this->id, name.c_str());

    auto filespace = dset.get_space();
    filespace.select_hyperslab(rng);

    auto memspace = Dataspace::create(rng.size());

    auto * vals = data.get_array();
    dset.read(memspace, filespace, vals);
    data.restore_array(vals);
}

} // namespace godzilla
