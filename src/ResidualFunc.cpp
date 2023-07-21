#include "ResidualFunc.h"

namespace godzilla {

ResidualFunc::ResidualFunc(FEProblemInterface * fepi, const std::string & region) :
    Functional(fepi, region)
{
}

} // namespace godzilla
