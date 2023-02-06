#include "Godzilla.h"
#include "ParsedFunction.h"
#include "CallStack.h"
#include "fmt/format.h"

namespace godzilla {

REGISTER_OBJECT(ParsedFunction);

static double
parsed_function_eval(void * ctx, double t, double x, double y, double z)
{
    auto * fn = static_cast<ParsedFunction *>(ctx);
    Real u[1] = { 0. };
    Real coord[3] = { x, y, z };
    fn->evaluate(3, t, coord, 1, u);
    return u[0];
}

static PetscErrorCode
parsed_function(Int dim, Real time, const Real x[], Int nc, Scalar u[], void * ctx)
{
    auto * fn = static_cast<ParsedFunction *>(ctx);
    fn->evaluate(dim, time, x, nc, u);
    return 0;
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
    _F_;
    this->evalr.create(this->function);
    for (const auto & it : constants)
        this->evalr.define_constant(it.first, it.second);
}

PetscFunc *
ParsedFunction::get_function()
{
    _F_;
    return parsed_function;
}

void *
ParsedFunction::get_context()
{
    _F_;
    return this;
}

void
ParsedFunction::register_callback(mu::Parser & parser)
{
    _F_;
    if (this->function.size() == 1)
        parser.DefineFunUserData(get_name(), parsed_function_eval, this);
}

void
ParsedFunction::evaluate(Int dim, Real time, const Real x[], Int nc, Scalar u[])
{
    _F_;
    this->evalr.evaluate(dim, time, x, nc, u);
}

} // namespace godzilla
