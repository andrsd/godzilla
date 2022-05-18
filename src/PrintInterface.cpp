#include "PrintInterface.h"
#include "CallStack.h"
#include "Object.h"
#include "App.h"

namespace godzilla {

PrintInterface::PrintInterface(const Object * obj) :
    proc_id(obj->get_processor_id()),
    verbosity_level(obj->get_app().get_verbosity_level()),
    prefix(obj->get_name())
{
    _F_;
}

void
PrintInterface::godzilla_print(unsigned int level, const char * format, ...) const
{
    _F_;
    if (level <= this->verbosity_level && this->proc_id == 0) {
        va_list argp;
        va_start(argp, format);
        fprintf(PETSC_STDOUT, "%s: ", this->prefix.c_str());
        vfprintf(PETSC_STDOUT, format, argp);
        fprintf(PETSC_STDOUT, "\n");
        va_end(argp);
    }
}

} // namespace godzilla
