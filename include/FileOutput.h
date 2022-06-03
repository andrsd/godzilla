#pragma once

#include "Output.h"
#include "petscviewer.h"

namespace godzilla {

/// Base class for file output
///
class FileOutput : public Output {
public:
    FileOutput(const InputParameters & params);
    virtual ~FileOutput();

    virtual void create() override;

    /// Get the file name with the output file produced by this outputter
    ///
    /// @return The file name with the output
    virtual const std::string & get_file_name() const;

    /// Set the file name for single output
    virtual void set_file_name();

    /// Set the file name for a sequence of outputs
    ///
    /// @param stepi Step number
    virtual void set_sequence_file_name(unsigned int stepi);

    /// Get file extension
    ///
    /// @return File extension
    virtual std::string get_file_ext() const = 0;

    virtual void output_step() override;

protected:
    /// Store the mesh
    ///
    /// @param dm DM holding the mesh to store into the file
    virtual void output_mesh(DM dm);

    /// Store the solution vector
    ///
    /// @param vec Solution vector to store into the file
    virtual void output_solution(Vec vec);

    /// The file base of the output file
    std::string file_base;

    /// The file name of the output file
    std::string file_name;

    /// Viewer for the output
    PetscViewer viewer;

public:
    static InputParameters valid_params();
};

} // namespace godzilla
