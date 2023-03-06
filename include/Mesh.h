#pragma once

#include "GodzillaConfig.h"
#include "Types.h"
#include "Object.h"
#include "PrintInterface.h"
#include "petscdm.h"

namespace godzilla {

class Section;

/// Base class for meshes
///
class Mesh : public Object, public PrintInterface {
public:
    explicit Mesh(const Parameters & parameters);

    NO_DISCARD virtual DM get_dm() const = 0;

    /// Get the mesh spatial dimension
    ///
    /// @return Mesh spatial dimension
    NO_DISCARD Int get_dimension() const;

    /// Distribute mesh over processes
    virtual void distribute() = 0;

    /// Set the `Section` encoding the local data layout for the `DM`.
    void set_local_section(const Section & section) const;

    /// Set the `Section` encoding the global data layout for the `DM`.
    void set_global_section(const Section & section) const;

protected:
    /// Spatial dimension of the mesh
    Int dim;

public:
    static Parameters parameters();
};

} // namespace godzilla
