#pragma once

#include "Output.h"
#include "petscviewer.h"

namespace godzilla {

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
class ExodusIIOutput : public Output {
public:
    ExodusIIOutput(const InputParameters & params);
    virtual ~ExodusIIOutput();

    virtual const std::string & getFileName() const override;
    virtual void setFileName() override;
    virtual void setSequenceFileName(unsigned int step) override;
    virtual void output(DM dm, Vec vec) const override;
    virtual void check() override;

protected:
    /// Viewer for ExodusII output
    PetscViewer viewer;
    /// The file base of the output file
    const std::string file_base;
    /// The file name of the output file
    std::string file_name;

public:
    static InputParameters validParams();
};

} // namespace godzilla
