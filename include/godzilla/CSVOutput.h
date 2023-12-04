// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/FileOutput.h"
#include <fstream>

namespace godzilla {

/// Output to a CSV file
///
class CSVOutput : public FileOutput {
public:
    explicit CSVOutput(const Parameters & params);
    ~CSVOutput() override;

    void create() override;
    [[nodiscard]] std::string get_file_ext() const override;
    void output_step() override;

    /// Get post-processor names
    ///
    /// @return Post-processor names
    const std::vector<std::string> &
    get_pps_names() const
    {
        return pps_names;
    }

protected:
    void open_file();
    void write_header();
    void write_values(Real time);
    void close_file();

private:
    /// Output file
    std::FILE * f;

    /// Flag indicating if header was written
    bool has_header;

    /// Names of postprocessors to store
    std::vector<std::string> pps_names;

public:
    static Parameters parameters();
};

} // namespace godzilla
