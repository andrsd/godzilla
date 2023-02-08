#pragma once

#include "GodzillaConfig.h"
#include "Types.h"
#include "Output.h"
#include "petscviewer.h"

namespace godzilla {

/// Base class for file output
///
class FileOutput : public Output {
public:
    explicit FileOutput(const Parameters & params);

    void create() override;

    /// Get the file name with the output file produced by this outputter
    ///
    /// @return The file name with the output
    NO_DISCARD virtual const std::string & get_file_name() const;

    /// Set the file name for single output
    virtual void set_file_name();

    /// Set the file name for a sequence of outputs
    ///
    /// @param stepi Step number
    virtual void set_sequence_file_name(unsigned int stepi);

    /// Get file extension
    ///
    /// @return File extension
    NO_DISCARD virtual std::string get_file_ext() const = 0;

protected:
    /// The file base of the output file
    std::string file_base;

    /// The file name of the output file
    std::string file_name;

public:
    static Parameters parameters();
};

} // namespace godzilla
