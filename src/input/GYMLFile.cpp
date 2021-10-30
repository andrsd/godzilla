#include "GYMLFile.h"
#include "yaml-cpp/yaml.h"


GYMLFile::GYMLFile(const std::string & file_name) :
    _file_name(file_name)
{
}

void
GYMLFile::parse()
{
  YAML::Node root = YAML::LoadFile(_file_name);
}
