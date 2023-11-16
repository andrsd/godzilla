#pragma once

#include "godzilla/FileOutput.h"

namespace godzilla {

class DiscreteProblemInterface;
class UnstructuredMesh;

/// Tecplot output
///
/// This saves the solution into an Tecplot (.plt) file.
/// This output works only with finite element problems
///
/// References:
/// [1] https://tecplot.azureedge.net/products/360/current/360_data_format_guide.pdf
class TecplotOutput : public FileOutput {
public:
    explicit TecplotOutput(const Parameters & params);
    ~TecplotOutput() override;

    NO_DISCARD std::string get_file_ext() const override;
    void set_file_name() override;
    void create() override;
    void check() override;
    void output_step() override;

protected:
    void open_file();
    void close_file();
    void write_header();
    void write_dataset_aux_data();
    void write_zone();

    void write_header_binary();
    void write_zone_binary();

    void write_header_ascii();
    void write_created_by_ascii();
    void write_zone_ascii();
    void write_coordinates_ascii();
    void write_node_ids_ascii();
    void write_element_ids_ascii();
    void write_connectivity_ascii();
    void write_field_variable_values_ascii();
    void write_nodal_field_variable_values_ascii();

    void write_line(const std::string & line);

    enum Format { BINARY, ASCII } format;

    /// FE problem interface (convenience pointer)
    DiscreteProblemInterface * dpi;
    /// Unstructured mesh
    UnstructuredMesh * mesh;
    /// Variable names to be stored
    const std::vector<std::string> & variable_names;

    /// File to write into
    std::FILE * file;
    /// Was header already written to the output file
    bool header_written;
    /// Number of shared variables
    Int n_shared_vars;
    /// Number of zones
    Int n_zones;
    /// Datatypes for variables
    std::string datatypes;
    /// Variable index of the "Element IDs" field
    Int element_id_var_index;
    /// List of field variable names to output
    std::vector<std::string> field_var_names;
    /// List of auxiliary field variable names to output
    std::vector<std::string> aux_field_var_names;
    /// List of nodal variable field IDs
    std::vector<Int> nodal_var_fids;
    /// List of nodal auxiliary variable field IDs
    std::vector<Int> nodal_aux_var_fids;

public:
    static Parameters parameters();
};

} // namespace godzilla
