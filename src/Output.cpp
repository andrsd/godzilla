#include "Output.h"
#include "CallStack.h"
#include "Problem.h"
#include "Utils.h"

namespace godzilla {

const unsigned int Output::ON_NONE = 0x0;
const unsigned int Output::ON_INITIAL = 0x1;
const unsigned int Output::ON_TIMESTEP = 0x2;
const unsigned int Output::ON_FINAL = 0x4;

Parameters
Output::parameters()
{
    Parameters params = Object::parameters();
    params.add_param<std::vector<std::string>>("on", "When output should happen");
    params.add_private_param<const Problem *>("_problem", nullptr);
    return params;
}

Output::Output(const Parameters & params) :
    Object(params),
    PrintInterface(this),
    problem(get_param<const Problem *>("_problem")),
    on_mask(ON_NONE)
{
    _F_;
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
    this->on_mask = mask;
}

void
Output::set_up_exec()
{
    _F_;
    if (is_param_valid("on")) {
        const auto & on = get_param<std::vector<std::string>>("on");
        if (!on.empty()) {
            bool none = false;
            unsigned int mask = 0;
            for (auto & s : on) {
                std::string ls = utils::to_lower(s);
                if (ls == "initial")
                    mask |= ON_INITIAL;
                else if (ls == "timestep")
                    mask |= ON_TIMESTEP;
                else if (ls == "final")
                    mask |= ON_FINAL;
                else if (ls == "none")
                    none = true;
            }

            if (none && (mask != 0))
                log_error("The 'none' execution flag can be used only by itself.");
            else
                this->on_mask = mask;
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
    return ((this->on_mask & flag) == flag);
}

} // namespace godzilla
