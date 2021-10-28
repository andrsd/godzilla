#include "GYMLParser.h"

GYMLParser::GYMLParser(MooseApp & app) : ConsoleStreamInterface(app)
{
}

void
GYMLParser::load(const std::string & file_name)
{
    _root = YAML::LoadFile(file_name);
}
