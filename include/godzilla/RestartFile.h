// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Enums.h"
#include "godzilla/Exception.h"
#include "godzilla/HDF5File.h"

namespace godzilla {

class Vector;

/// Class for handling restart files
class RestartFile {
public:
    RestartFile(mpi::Communicator comm, fs::path file_name, FileAccess faccess);

    /// Restart file
    ///
    /// @param file_name Name of the file
    /// @param faccess Access mode
    RestartFile(fs::path file_name, FileAccess faccess);

    /// Write data to the file
    ///
    /// @param path Path to the data
    /// @param data Data to write
    template <typename T>
    void write(String path, String name, const T & data);

    template <typename T>
    void write(String app_name, String path, String name, const T & data);

    /// Write global vector
    void write_global_vector(String path, String name, const Vector & data);

    /// Write global vector
    void write_global_vector(String app_name, String path, String name, const Vector & data);

    /// Read data from the file
    ///
    /// @param path Path to the data
    /// @param data Variable to store the data read from the file
    template <typename T>
    void read(String path, String name, T & data) const;

    template <typename T>
    void read(String app_name, String path, String name, T & data) const;

    /// Read global vector
    void read_global_vector(String path, String name, Vector & data) const;

    /// Read global vector
    void read_global_vector(String app_name, String path, String name, Vector & data) const;

    /// Get the name of the file
    ///
    /// @return The name of the file
    [[nodiscard]] fs::path file_name() const;

    /// Get the path of the file
    ///
    /// @return The path of the file
    [[nodiscard]] fs::path file_path() const;

protected:
    /// Get the full "path" to the data inside HDF5 file
    ///
    /// @param app_name Name of the application
    /// @param path Path to the data
    String get_full_path(String app_name, String path) const;

    /// Get the "path" to the data inside HDF5 file
    ///
    /// @param path Path to the data
    /// @return Path to the data
    String normalize_path(String path) const;

private:
    HDF5File h5f;
};

template <typename T>
void
RestartFile::write(String path, String name, const T & data)
{
    auto norm_path = normalize_path(path);
    try {
        auto group = this->h5f.create_group(norm_path);
        group.template write_dataset<T>(name, data);
    }
    catch (std::exception & e) {
        throw Exception("Error writing '{}' to {}: {}",
                        norm_path,
                        this->file_name().string(),
                        e.what());
    }
}

template <typename T>
void
RestartFile::write(String app_name, String path, String name, const T & data)
{
    this->write<T>(get_full_path(app_name, path), name, data);
}

template <typename T>
void
RestartFile::read(String path, String name, T & data) const
{
    auto norm_path = normalize_path(path);
    try {
        auto group = this->h5f.open_group(norm_path);
        group.template read_dataset<T>(name, data);
    }
    catch (std::exception & e) {
        throw Exception("Error reading '{}' from {}: {}",
                        norm_path,
                        this->file_name().string(),
                        e.what());
    }
}

template <typename T>
void
RestartFile::read(String app_name, String path, String name, T & data) const
{
    this->read<T>(get_full_path(app_name, path), name, data);
}

// Specializations for our datatypes go below

template <>
void RestartFile::write<Vector>(String path, String name, const Vector & data);

template <>
void RestartFile::read<Vector>(String path, String name, Vector & data) const;

} // namespace godzilla
