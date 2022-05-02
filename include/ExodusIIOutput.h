#pragma once

#include "FileOutput.h"

namespace godzilla {

/// ExodusII output
///
/// Input file example:
/// ```
/// output:
///   exodus:
///     type: ExodusIIOutput
///     file: 'out'
/// ```
class ExodusIIOutput : public FileOutput {
public:
    ExodusIIOutput(const InputParameters & params);

    virtual std::string get_file_ext() const override;
    virtual void create() override;
    virtual void check() override;
    virtual void output() override;

protected:
    void output_file(int exoid);
    void output_coords(int exoid);
    void output_elements(int exoid);
    void output_variables(int exoid);
    void output_nodal_variables(int exoid);

    static const char * get_elem_type(DMPolytopeType elem_type);

public:
    static InputParameters valid_params();
};

} // namespace godzilla
