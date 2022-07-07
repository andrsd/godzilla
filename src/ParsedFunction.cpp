#include "Godzilla.h"
#include "ParsedFunction.h"
#include "CallStack.h"
#include "fmt/format.h"

namespace godzilla {

REGISTER_OBJECT(ParsedFunction);

static double
__parsed_function_eval(void * ctx, double t, double x, double y, double z)
{
    ParsedFunction * fn = static_cast<ParsedFunction *>(ctx);
    PetscReal u[1] = { 0. };
    PetscReal coord[3] = { x, y, z };
    fn->evaluate(3, t, coord, 1, u);
    return u[0];
}

static PetscErrorCode
__parsed_function(PetscInt dim,
                  PetscReal time,
                  const PetscReal x[],
                  PetscInt nc,
                  PetscScalar u[],
                  void * ctx)
{
    ParsedFunction * fn = static_cast<ParsedFunction *>(ctx);
    fn->evaluate(dim, time, x, nc, u);
    return 0;
}

Parameters
ParsedFunction::valid_params()
{
    Parameters params = Function::valid_params();
    params.add_param<std::vector<std::string>>(
        "function",
        "Text representation of the function to evaluate (one per component)");
    params.add_param<std::map<std::string, PetscReal>>("constants", "Constants");
    return params;
}

ParsedFunction::ParsedFunction(const Parameters & params) :
    Function(params),
    function(get_param<std::vector<std::string>>("function")),
    constants(get_param<std::map<std::string, PetscReal>>("constants"))
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
    return __parsed_function;
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
        parser.DefineFunUserData(get_name(), __parsed_function_eval, this);
}

void
ParsedFunction::evaluate(PetscInt dim,
                         PetscReal time,
                         const PetscReal x[],
                         PetscInt nc,
                         PetscScalar u[])
{
    _F_;
    this->evalr.evaluate(dim, time, x, nc, u);
}

} // namespace godzilla
