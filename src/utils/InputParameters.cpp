#include "utils/InputParameters.h"
#include <cmath>


namespace godzilla {

InputParameters
emptyInputParameters()
{
    InputParameters params;
    return params;
}

InputParameters::InputParameters()
{
}

InputParameters::InputParameters(const InputParameters & rhs)
{
    *this = rhs;
}

}
