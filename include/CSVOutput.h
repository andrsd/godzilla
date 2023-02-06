#pragma once

#include "GodzillaConfig.h"
#include "FileOutput.h"
#include <fstream>

namespace godzilla {

/// Output to a CSV file
///
class CSVOutput : public FileOutput {
public:
    explicit CSVOutput(const Parameters & params);
    ~CSVOutput() override;

    void create() override;
    NO_DISCARD std::string get_file_ext() const override;
    void output_step() override;

protected:
    void open_file();
    void write_header();
    void write_values(Real time);
    void close_file();

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
