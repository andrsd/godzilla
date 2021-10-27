#include "GExecutioner.h"

GExecutioner::GExecutioner(const InputParameters & parameters) :
    MooseObject(parameters)
{
}

void
GExecutioner::execute()
{
    std::cerr << "execute" << std::endl;
}
