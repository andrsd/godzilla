// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/PetscObjectWrapper.h"
#include "godzilla/Types.h"
#include "godzilla/String.h"
#include "petscsys.h"
#include <vector>

namespace godzilla {

class Random : public PetscObjectWrapper<PetscRandom> {
public:
    Random();

    void create(MPI_Comm comm);
    void destroy();

    void get_interval(Scalar & low, Scalar & high) const;
    unsigned long get_seed() const;
    String get_type() const;
    Scalar get_value() const;
    Real get_value_real() const;
    void get_values(std::vector<Scalar> & vals) const;
    void get_values_real(std::vector<Real> & vals) const;

    void seed();

    void set_interval(Scalar low, Scalar high);
    void set_seed(unsigned long seed);
    void set_type(String type);
};

} // namespace godzilla
