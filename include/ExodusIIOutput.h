#pragma once

#include "FileOutput.h"

namespace godzilla {

class FEProblemInterface;

/// ExodusII output
///
/// This saves the solution into an ExodusII file.
///
/// Input file example:
/// ```
/// output:
///   exo:
///     type: ExodusIIOutput
///     file: 'out'
/// ```
///
/// This output works only with finite element problems
class ExodusIIOutput : public FileOutput {
public:
    ExodusIIOutput(const InputParameters & params);

    virtual std::string get_file_ext() const override;
    virtual void create() override;
    virtual void check() override;
    virtual void output_step(PetscInt stepi, DM dm, Vec vec) override;

protected:
    /// Create "Cell Sets" label if it does not exist
    void create_cell_sets();

    /// Write variable info into the ExodusII file
    void write_variable_info(int exoid, Vec vec);

    /// FE problem interface (convenience pointer)
    const FEProblemInterface * fepi;

    /// Number of the file in a sequence. This is a sequence of ExodusII files, which is different
    /// from the sequence of steps produced by a Problem class.
    int file_seq_no;

public:
    static InputParameters valid_params();
};

} // namespace godzilla
