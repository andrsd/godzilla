// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Object.h"
#include "godzilla/PrintInterface.h"

namespace godzilla {

class Mesh;

/// Base class for objects that create `Mesh`es. These are user-facing objects.
///
/// TODO: This needs a better name
class MeshObject : public Object, public PrintInterface {
public:
    explicit MeshObject(const Parameters & parameters);
    ~MeshObject() override;

    void create() override;

    /// Get the mesh object
    ///
    /// @return Mesh object
    template <class T>
    T * get_mesh() const
    {
        CALL_STACK_MSG();
        return dynamic_cast<T *>(this->mesh);
    }

protected:
    virtual Mesh * create_mesh() = 0;
    void lprint_mesh_info();

private:
    /// Mesh object build by us
    Mesh * mesh;

public:
    static Parameters parameters();
};

} // namespace godzilla
