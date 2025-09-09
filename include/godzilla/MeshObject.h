// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Qtr.h"
#include "godzilla/Object.h"
#include "godzilla/PrintInterface.h"
#include "godzilla/Mesh.h"

namespace godzilla {

/// Base class for objects that create `Mesh`es. These are user-facing objects.
///
/// TODO: This needs a better name
class MeshObject : public Object, public PrintInterface {
public:
    explicit MeshObject(const Parameters & parameters);

    void create() override;

    /// Get the mesh object
    ///
    /// @return Mesh object
    template <class T>
    T *
    get_mesh() const
    {
        CALL_STACK_MSG();
        return dynamic_cast<T *>(this->mesh.get());
    }

protected:
    virtual Qtr<Mesh> create_mesh() = 0;
    void lprint_mesh_info();

private:
    /// Mesh object build by us
    Qtr<Mesh> mesh;

public:
    static Parameters parameters();
};

} // namespace godzilla
