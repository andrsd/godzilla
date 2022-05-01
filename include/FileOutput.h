#pragma once

#include "Output.h"

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
    virtual const std::string & get_file_name() const;

    /// Set the file name for single output
    virtual void set_file_name();

    /// Get file extension
    ///
    /// @return File extension
    virtual std::string get_file_ext() const = 0;

protected:
    /// The file base of the output file
    const std::string file_base;

    /// The file name of the output file
    std::string file_name;

public:
    static InputParameters valid_params();
};

} // namespace godzilla
