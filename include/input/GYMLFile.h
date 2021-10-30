#pragma once

#include <string>

/**
 * YML parser for input files
 */
class GYMLFile
{
public:
    GYMLFile(const std::string & file_name);

    /// parse the YML file
    virtual void parse();

protected:
    const std::string & _file_name;
};
