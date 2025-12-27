// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Types.h"
#include "godzilla/String.h"
#include "godzilla/Output.h"
#include <filesystem>

namespace fs = std::filesystem;

namespace godzilla {

class DiscreteProblemInterface;
class UnstructuredMesh;

/// Base class for file output
///
class FileOutput : public Output {
public:
    explicit FileOutput(const Parameters & pars);

    void create() override;

    /// Get the file name with the output file produced by this outputter
    ///
    /// @return The file name with the output
    fs::path get_file_name() const;

    /// Set file base
    ///
    /// @param file_base File base to set
    void set_file_base(fs::path file_base);

    /// Set the file base for a sequence of outputs
    ///
    /// @param stepi Step number
    void set_sequence_file_base(unsigned int stepi);

protected:
    fs::path get_file_base() const;
    void add_var_names(FieldID fid, std::vector<std::string> & var_names);
    void add_aux_var_names(FieldID fid, std::vector<std::string> & var_names);

    const DiscreteProblemInterface * get_discrete_problem_interface() const;
    DiscreteProblemInterface * get_discrete_problem_interface();

private:
    /// Get file extension
    ///
    /// @return File extension
    virtual String get_file_ext() const = 0;
    virtual fs::path create_file_name() const;

    /// The file base of the output file
    fs::path file_base;
    /// The file name of the output file
    fs::path file_name;
    /// Convenience pointer
    DiscreteProblemInterface * dpi;

public:
    static Parameters parameters();
};

} // namespace godzilla
