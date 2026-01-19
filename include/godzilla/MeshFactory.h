// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Mesh.h"
#include "godzilla/Parameters.h"
#include "godzilla/Qtr.h"

namespace godzilla {

template <typename Producer, typename = void>
struct CreateMeshResult {
    using type = void;
};

template <typename Producer>
struct CreateMeshResult<Producer, std::void_t<decltype(std::declval<Producer>().create_mesh())>> {
    using type = decltype(std::declval<Producer>().create_mesh());
};

template <typename Producer>
using CreateMeshReturnType = typename CreateMeshResult<Producer>::type;

template <typename Producer>
constexpr bool HasCreateMeshValue = !std::is_void_v<CreateMeshReturnType<Producer>>;

// Helper: get the raw element type produced (Derived for Qtr<Derived>, Derived* for raw
// etc.)
template <typename R>
struct ProducedRawTypeImpl {
    using type = std::remove_cv_t<std::remove_reference_t<R>>;
};

template <typename T>
struct ProducedRawTypeImpl<Qtr<T>> {
    using type = T;
};

template <typename T>
struct ProducedRawTypeImpl<T *> {
    using type = std::remove_pointer_t<T *>;
};

template <typename R>
using ProducedRawType = typename ProducedRawTypeImpl<std::decay_t<R>>::type;

/// Class for building Mesh-derived objects
class MeshFactory {
public:
    MeshFactory() = default;

    template <typename PRODUCER, typename Ret = CreateMeshReturnType<PRODUCER>>
    static Ret
    create(const Parameters & pars)
    {
        static_assert(IsConstructibleFromParams<PRODUCER>::value,
                      "PRODUCER must be constructible from `const Parameters &`");
        static_assert(HasCreateMeshValue<PRODUCER>,
                      "PRODUCER must have a create_mesh() member function");
        static_assert(std::is_base_of_v<Mesh, ProducedRawType<Ret>>,
                      "PRODUCER::create_mesh() must return something convertible to a Mesh (e.g. "
                      "Qtr<Mesh>, ...)");

        PRODUCER producer { pars };
        auto mesh = producer.create_mesh();
        if (mesh)
            mesh->set_up();
        return mesh;
    }
};

} // namespace godzilla
