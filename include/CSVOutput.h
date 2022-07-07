#pragma once

#include "FileOutput.h"
#include <fstream>

namespace godzilla {

/// Output to a CSV file
///
class CSVOutput : public FileOutput {
public:
    CSVOutput(const Parameters & params);
    virtual ~CSVOutput();

    virtual void create() override;
    virtual std::string get_file_ext() const override;
    virtual void output_step() override;

protected:
    void open_file();
    void write_header();
    void write_values(PetscReal time);
    void close_file();

    /// Output file
    std::FILE * f;

    /// Flag indicating if header was written
    bool has_header;

    /// Names of postprocessors to store
    std::vector<std::string> pps_names;

public:
    static Parameters valid_params();
};

} // namespace godzilla
