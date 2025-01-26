// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Enums.h"
#include "godzilla/Exception.h"
#include <string>
#include <algorithm>
#include "h5pp/h5pp.h"

namespace godzilla {

/// Class for handling restart files
class RestartFile {
public:
    RestartFile(const std::string & file_name, FileAccess faccess);
    ~RestartFile() = default;

    /// Write data to the file
    ///
    /// @param path Path to the data
    /// @param data Data to write
    template <typename T>
    void write(const std::string & path, const T & data);

    /// Read data from the file
    ///
    /// @param path Path to the data
    /// @return Data read from the file
    template <typename T>
    T read(const std::string & path) const;

    /// Get the name of the file
    ///
    /// @return The name of the file
    [[nodiscard]] std::string file_name() const;

    /// Get the path of the file
    ///
    /// @return The path of the file
    [[nodiscard]] std::string file_path() const;

private:
    h5pp::File h5f;
};

template <typename T>
void
RestartFile::write(const std::string & path, const T & data)
{
    try {
        this->h5f.template writeDataset<T>(data, path);
    }
    catch (std::exception & e) {
        throw Exception("Error writing '{}' to {}: {}", path, this->file_name(), e.what());
    }
}

template <typename T>
T
RestartFile::read(const std::string & path) const
{
    try {
        return this->h5f.template readDataset<T>(path);
    }
    catch (std::exception & e) {
        throw Exception("Error reading '{}' from {}: {}", path, this->file_name(), e.what());
    }
}

} // namespace godzilla
