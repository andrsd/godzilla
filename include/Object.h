#pragma once

#include "petsc.h"
#include "InputParameters.h"
#include "LoggingInterface.h"

namespace godzilla {

class App;

/// The base class for objects that can be created by Factory
///
/// @see Factory
class Object : public LoggingInterface {
public:
    /// Constructor for building the object via Factory
    Object(const InputParameters & parameters);
    virtual ~Object();

    /// Get the type of this object.
    /// @return the name of the type of this object
    const std::string & get_type() const;

    /// Get the name of the object
    /// @return The name of the object
    virtual const std::string & get_name() const;

    /// Get the parameters of the object
    /// @return The parameters of the object
    const InputParameters & get_parameters() const;

    /// Retrieve a parameter for the object
    /// @param name The name of the parameter
    /// @return The value of the parameter
    template <typename T>
    const T & get_param(const std::string & name) const;

    /// Test if the supplied parameter is valid
    /// @param name The name of the parameter to test
    bool is_param_valid(const std::string & name) const;

    /// Get the App this object is associated with
    const App & get_app() const;

    /// Get the MPI comm this object works on
    const MPI_Comm & get_comm() const;

    /// Get processor ID (aka MPI rank) this object is running at
    const PetscMPIInt & get_processor_id() const;

    /// Get communicator size
    const PetscMPIInt & get_comm_size() const;

    /// Called to construct the object
    virtual void create();

    /// Called to check object's integrity
    virtual void check();

protected:
    /// Parameters of this object
    const InputParameters & pars;

    /// Reference to the aplpication owning this object
    const App & app;

    /// The type of this object
    const std::string & type;

    /// The name of this object
    const std::string & name;

public:
    /// Method for building InputParameters for this class
    static InputParameters valid_params();
};

template <typename T>
const T &
Object::get_param(const std::string & name) const
{
    return this->pars.get<T>(name);
}

} // namespace godzilla
