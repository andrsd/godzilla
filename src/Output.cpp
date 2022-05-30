#include "Output.h"
#include "CallStack.h"
#include "Problem.h"
#include "Utils.h"
#include <assert.h>

namespace godzilla {

const unsigned int Output::ON_NONE = 0x0;
const unsigned int Output::ON_INITIAL = 0x1;
const unsigned int Output::ON_TIMESTEP = 0x2;
const unsigned int Output::ON_FINAL = 0x4;

InputParameters
Output::valid_params()
{
    InputParameters params = Object::valid_params();
    params.add_param<std::vector<std::string>>("on", "When output should happen");
    params.add_private_param<const Problem *>("_problem");
    return params;
}

Output::Output(const InputParameters & params) :
    Object(params),
    PrintInterface(this),
    problem(get_param<const Problem *>("_problem")),
    on(ON_NONE)
{
    _F_;
    assert(this->problem != nullptr);
}

void
Output::create()
{
    _F_;
    set_up_exec();
}

void
Output::set_exec_mask(unsigned int mask)
{
    _F_;
    this->on = mask;
}

void
Output::set_up_exec()
{
    _F_;
    if (is_param_valid("on")) {
        const auto & on = get_param<std::vector<std::string>>("on");
        if (on.size() > 0) {
            bool none = false;
            unsigned int mask = 0;
            for (auto & s : on) {
                std::string ls = utils::to_lower(s);
                if (ls.compare("initial") == 0)
                    mask |= ON_INITIAL;
                else if (ls.compare("timestep") == 0)
                    mask |= ON_TIMESTEP;
                else if (ls.compare("final") == 0)
                    mask |= ON_FINAL;
                else if (ls.compare("none") == 0)
                    none = true;
            }

            if (none && (mask != 0))
                log_error("The 'none' execution flag can be used only by itself.");
            else
                this->on = mask;
        }
        else
            log_error("The 'on' parameter can be either 'none' or a combination of 'initial', "
                      "'timestep' and/or 'final'.");
    }
}

bool
Output::should_output(unsigned int flag)
{
    _F_;
    return ((this->on & flag) == flag);
}

} // namespace godzilla
