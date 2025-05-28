// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Types.h"
#include "godzilla/Output.h"

namespace godzilla {

class DiscreteProblemInterface;
class UnstructuredMesh;

/// Base class for file output
///
class FileOutput : public Output {
public:
    explicit FileOutput(const Parameters & params);

    void create() override;

    /// Get the file name with the output file produced by this outputter
    ///
    /// @return The file name with the output
    std::string get_file_name() const;

    /// Set file base
    ///
    /// @param file_base File base to set
    void set_file_base(const std::string & file_base);

    /// Set the file base for a sequence of outputs
    ///
    /// @param stepi Step number
    void set_sequence_file_base(unsigned int stepi);

protected:
    void add_var_names(Int fid, std::vector<std::string> & var_names);
    void add_aux_var_names(Int fid, std::vector<std::string> & var_names);

    const DiscreteProblemInterface * get_discrete_problem_interface() const;
    DiscreteProblemInterface * get_discrete_problem_interface();

private:
    /// Get file extension
    ///
    /// @return File extension
    virtual std::string get_file_ext() const = 0;

    /// The file base of the output file
    std::string file_base;
    /// Convenience pointer
    DiscreteProblemInterface * dpi;

public:
    static Parameters parameters();
};

} // namespace godzilla
