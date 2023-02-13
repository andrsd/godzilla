#include "ResidualFunc.h"

namespace godzilla {

ResidualFunc::ResidualFunc(const FEProblemInterface * fepi, const std::string & region) :
    Functional(const_cast<FEProblemInterface *>(fepi), region)
{
}

} // namespace godzilla
