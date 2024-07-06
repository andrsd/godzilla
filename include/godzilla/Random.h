// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "petscsys.h"
#include "godzilla/Types.h"
#include <vector>

namespace godzilla {

class Random {
public:
    Random();

    void create(MPI_Comm comm);
    void destroy();

    void get_interval(Scalar & low, Scalar & high) const;
    [[nodiscard]] unsigned long get_seed() const;
    [[nodiscard]] std::string get_type() const;
    [[nodiscard]] Scalar get_value() const;
    [[nodiscard]] Real get_value_real() const;
    void get_values(std::vector<Scalar> & vals) const;
    void get_values_real(std::vector<Real> & vals) const;

    void seed();

    void set_interval(Scalar low, Scalar high);
    void set_seed(unsigned long seed);
    void set_type(const std::string & type);

private:
    PetscRandom rng;
};

} // namespace godzilla
