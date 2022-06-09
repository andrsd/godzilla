#pragma once

#include "FileOutput.h"

namespace godzilla {

/// Output mesh partitioning
///
class MeshPartitioningOutput : public FileOutput {
public:
    MeshPartitioningOutput(const InputParameters & params);

    virtual std::string get_file_ext() const override;
    virtual void output_step() override;

public:
    static InputParameters valid_params();
};

} // namespace godzilla
