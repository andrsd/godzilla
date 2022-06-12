#include "FunctionEvaluator.h"
#include "CallStack.h"
#include "Function.h"
#include <assert.h>

namespace godzilla {

FunctionEvaluator::FunctionEvaluator()
{
    _F_;
}

void
FunctionEvaluator::create(const std::string & expr)
{
    _F_;
    this->parser.SetExpr(expr);
    this->parser.DefineConst("pi", 3.14159265359);
    this->parser.DefineConst("e", 2.71828182846);
}

void
FunctionEvaluator::define_constant(const std::string & name, PetscReal value)
{
    _F_;
    this->parser.DefineConst(name, value);
}

void
FunctionEvaluator::register_function(Function * fn)
{
    _F_;
    fn->register_callback(this->parser);
}

PetscReal
FunctionEvaluator::evaluate(PetscInt dim, PetscReal time, const PetscReal x[])
{
    _F_;
    PetscReal * xx = const_cast<PetscReal *>(x);
    PetscReal zero = 0.;
    try {
        this->parser.DefineVar("t", &time);
        this->parser.DefineVar("x", &(xx[0]));
        if (dim >= 2)
            this->parser.DefineVar("y", &(xx[1]));
        else
            this->parser.DefineVar("y", &zero);
        if (dim >= 3)
            this->parser.DefineVar("z", &(xx[2]));
        else
            this->parser.DefineVar("z", &zero);
        return this->parser.Eval();
    }
    catch (mu::Parser::exception_type & e) {
        return std::nan("");
    }
}

bool
FunctionEvaluator::evaluate(PetscInt dim,
                            PetscReal time,
                            const PetscReal x[],
                            PetscInt nc,
                            PetscReal u[])
{
    _F_;
    PetscReal * xx = const_cast<PetscReal *>(x);
    PetscReal zero = 0.;
    try {
        this->parser.DefineVar("t", &time);
        this->parser.DefineVar("x", &(xx[0]));
        if (dim >= 2)
            this->parser.DefineVar("y", &(xx[1]));
        else
            this->parser.DefineVar("y", &zero);
        if (dim >= 3)
            this->parser.DefineVar("z", &(xx[2]));
        else
            this->parser.DefineVar("z", &zero);

        int n_num;
        mu::value_type * val = this->parser.Eval(n_num);
        assert(nc == n_num);
        for (int i = 0; i < n_num; i++)
            u[i] = val[i];
        return true;
    }
    catch (mu::Parser::exception_type & e) {
        return false;
    }
}

} // namespace godzilla
