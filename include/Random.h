#pragma once

#include "petscsys.h"
#include "Types.h"
#include <vector>

namespace godzilla {

class Random {
public:
    Random();

    void create(MPI_Comm comm);
    void destroy();

    void get_interval(Scalar & low, Scalar & high) const;
    unsigned long get_seed() const;
    std::string get_type() const;
    Scalar get_value() const;
    Real get_value_real() const;
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
