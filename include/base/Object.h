#pragma once

#include "mpi.h"
#include "utils/InputParameters.h"

namespace godzilla {

class App;

///
class Object
{
public:
    Object(const InputParameters & parameters);

    /**
     * Get the type of this object.
     * @return the name of the type of this object
     */
    const std::string & getType() const;

    /**
     * Get the name of the object
     * @return The name of the object
     */
    virtual const std::string & getName() const;

    /**
     * Get the parameters of the object
     * @return The parameters of the object
     */
    const InputParameters & getParameters() const;

    /**
     * Retrieve a parameter for the object
     * @param name The name of the parameter
     * @return The value of the parameter
     */
    template <typename T>
    const T & getParam(const std::string & name) const;

    /**
     * Test if the supplied parameter is valid
     * @param name The name of the parameter to test
     */
    bool isParamValid(const std::string & name) const;

    /**
     * Get the MooseApp this object is associated with.
     */
    const App & getApp() const;

    /// Get the MPI comm this object works on
    MPI_Comm comm();

protected:
    /// Parameters of this object
    const InputParameters & pars;
    ///
    const App & app;
    /// The type of this object
    const std::string & type;
    /// The name of this object
    const std::string & name;

public:
    static InputParameters validParams();
};

template <typename T>
const T &
Object::getParam(const std::string & name) const
{
    return this->pars.get<T>(name);
}

}
