// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/HDF5File.h"
#include "godzilla/CallStack.h"
#include "godzilla/Enums.h"

namespace godzilla {

namespace {

std::once_flag hdf5_init;

void
disable_hdf5_output()
{
    H5Eset_auto2(H5E_DEFAULT, NULL, NULL);
}

} // namespace

HDF5File::HDF5File(mpi::Communicator comm, fs::path file_name, FileAccess faccess) :
    file_name(std::move(file_name))
{
    std::call_once(hdf5_init, disable_hdf5_output);

    MPI_Info info = MPI_INFO_NULL;
    auto fapl = H5Pcreate(H5P_FILE_ACCESS);
    H5Pset_fapl_mpio(fapl, comm, info);

    if (faccess == FileAccess::READ)
        this->id = H5Fopen(this->file_name.c_str(), H5F_ACC_RDONLY, fapl);
    else if (faccess == FileAccess::WRITE)
        this->id = H5Fopen(this->file_name.c_str(), H5F_ACC_RDWR, fapl);
    else if (faccess == FileAccess::CREATE)
        this->id = H5Fcreate(this->file_name.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, fapl);
    else
        throw Exception("Unsupported file access");

    if (this->id == H5I_INVALID_HID)
        throw Exception("Unable to open {} or it is not a valid HDF5 file.",
                        this->file_name.string());
}

HDF5File::HDF5File(fs::path file_name, FileAccess faccess) : file_name(std::move(file_name))
{
    std::call_once(hdf5_init, disable_hdf5_output);

    if (faccess == FileAccess::READ)
        this->id = H5Fopen(this->file_name.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
    else if (faccess == FileAccess::WRITE)
        this->id = H5Fopen(this->file_name.c_str(), H5F_ACC_RDWR, H5P_DEFAULT);
    else if (faccess == FileAccess::CREATE)
        this->id = H5Fcreate(this->file_name.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    else
        throw Exception("Unsupported file access");

    if (this->id == H5I_INVALID_HID)
        throw Exception("Unable to open {} or it is not a valid HDF5 file.",
                        this->file_name.string());
}

HDF5File::~HDF5File()
{
    if (this->id != H5I_INVALID_HID)
        H5Fclose(this->id);
}

fs::path
HDF5File::get_file_name() const
{
    return this->file_name.filename();
}

fs::path
HDF5File::get_file_path() const
{
    return this->file_name;
}

void
HDF5File::Group::write_global_vector(String name, const Vector & data)
{
    auto rng = data.get_ownership_range();

    auto space = Dataspace::create(data.get_size());
    auto dset = Dataset::create<Real>(this->id, name.c_str(), space);

    auto filespace = dset.get_space();
    filespace.select_hyperslab(rng);

    auto memspace = Dataspace::create(rng.size());

    auto vals = data.borrow_array_read();
    dset.write(memspace, filespace, vals.data());
}

void
HDF5File::Group::read_global_vector(String name, Vector & data)
{
    auto rng = data.get_ownership_range();

    auto dset = Dataset::open(this->id, name.c_str());

    auto filespace = dset.get_space();
    filespace.select_hyperslab(rng);

    auto memspace = Dataspace::create(rng.size());

    auto vals = data.borrow_array();
    dset.read(memspace, filespace, vals.data());
}

} // namespace godzilla
