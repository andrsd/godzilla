// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Godzilla.h"
#include "godzilla/ParsedFunction.h"
#include "godzilla/CallStack.h"

namespace godzilla {

static double
parsed_function_eval(void * ctx, double t, double x, double y, double z)
{
    auto * fn = static_cast<ParsedFunction *>(ctx);
    std::vector<Real> u(1);
    std::array<Real, 3> coord = { x, y, z };
    fn->evaluate(t, coord.data(), u.data());
    return u[0];
}

Parameters
ParsedFunction::parameters()
{
    Parameters params = Function::parameters();
    params.add_param<std::vector<std::string>>(
        "function",
        "Text representation of the function to evaluate (one per component)");
    params.add_param<std::map<std::string, Real>>("constants", "Constants");
    return params;
}

ParsedFunction::ParsedFunction(const Parameters & params) :
    Function(params),
    function(get_param<std::vector<std::string>>("function")),
    constants(get_param<std::map<std::string, Real>>("constants"))
{
    CALL_STACK_MSG();
    this->evalr.create(this->function);
    for (const auto & it : constants)
        this->evalr.define_constant(it.first, it.second);
}

void
ParsedFunction::register_callback(mu::Parser & parser)
{
    CALL_STACK_MSG();
    if (this->function.size() == 1)
        parser.DefineFunUserData(get_name(), parsed_function_eval, this);
}

void
ParsedFunction::evaluate(Real time, const Real x[], Scalar u[])
{
    CALL_STACK_MSG();
    this->evalr.evaluate(get_dimension(), time, x, this->function.size(), u);
}

} // namespace godzilla
