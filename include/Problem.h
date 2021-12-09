#pragma once

#include "Object.h"
#include "PrintInterface.h"
#include "petscdm.h"
#include "petscpartitioner.h"

namespace godzilla {

class Grid;
class Output;

/// Problem
///
class Problem : public Object, public PrintInterface {
public:
    Problem(const InputParameters & parameters);
    virtual ~Problem();

    virtual void check() override;

    /// Build the problem to solve
    virtual void create() override;
    /// Run the problem
    virtual void run() = 0;
    /// Solve the problem
    virtual void solve() = 0;
    /// true if solve converged, otherwise false
    virtual bool converged() = 0;
    /// provide DM for the underlying KSP object
    virtual DM getDM() const = 0;
    /// Return solution vector
    virtual Vec getSolutionVector() const = 0;

    /// Add and output object
    ///
    /// @param output Output object to add
    virtual void addOutput(Output * output);

    /// Set partitioner type
    ///
    /// @param type Type of the partitioner
    virtual void setPartitionerType(const std::string & type);

    /// Set partition overlap
    ///
    /// @param overlap Set the partition overlap
    virtual void setPartitionOverlap(PetscInt overlap);

protected:
    virtual void setUpPartitioning();

    /// Grid
    Grid & grid;
    /// Partitioner information
    struct PartitionerInfo {
        /// Type of partitioner
        std::string type;
        /// The overlap of partitions
        PetscInt overlap;
    } partitioner_info;
    /// Mesh partitioner
    PetscPartitioner partitioner;
    /// List of output objects
    std::vector<Output *> outputs;

public:
    static InputParameters validParams();
};

} // namespace godzilla
