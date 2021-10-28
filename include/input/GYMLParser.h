#pragma once

#include "yaml-cpp/yaml.h"
#include "ConsoleStreamInterface.h"


/**
 * YML parser for input files
 */
class GYMLParser : public ConsoleStreamInterface
{
public:
    GYMLParser(MooseApp & app);

    /// load the YML file from `file_name`
    virtual void load(const std::string & file_name);

protected:
    /// root node of the YML tree
    YAML::Node _root;
};
