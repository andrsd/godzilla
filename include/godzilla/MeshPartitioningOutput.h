#pragma once

#include "godzilla/FileOutput.h"

namespace godzilla {

/// Output mesh partitioning
///
class MeshPartitioningOutput : public FileOutput {
public:
    explicit MeshPartitioningOutput(const Parameters & params);

    void check() override;
    [[nodiscard]] std::string get_file_ext() const override;
    void output_step() override;

public:
    static Parameters parameters();
};

} // namespace godzilla
