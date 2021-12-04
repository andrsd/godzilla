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

    /// Get the file name with the output file produced by this outputter
    ///
    /// @return The file name with the output
    virtual const std::string & getFileName() const;

    /// Set the file name for single output
    virtual void setFileName();

    /// Set the file name for a sequence of outputs
    ///
    /// @param stepi Step number
    virtual void setSequenceFileName(unsigned int stepi);

    /// Get file extension
    ///
    /// @return File extension
    virtual std::string getFileExt() const = 0;

    virtual void outputMesh(DM dm) override;
    virtual void outputSolution(Vec vec) override;
    virtual void outputStep(PetscInt stepi, DM dm, Vec vec) override;

protected:
    /// The file base of the output file
    const std::string file_base;

    /// The file name of the output file
    std::string file_name;

    /// Viewer for the output
    PetscViewer viewer;

public:
    static InputParameters validParams();
};

} // namespace godzilla
