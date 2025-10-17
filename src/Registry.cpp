// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Registry.h"
#include "godzilla/Utils.h"

namespace godzilla {

namespace {

/// Convert C++ names into human readable names
std::string
human_type_name(const std::string & type)
{
    // clang-format off
    if (type == "std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>>" ||
        type == "NSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEEE" ||
        type == "std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >")
        return "String";
    else if (type == "int" || type == "long" || type == "long long")
        return "Integer";
    else if (type == "double" || type == "float")
        return "Real";
    else if (type == "bool")
        return "Boolean";
    else if (type == "std::__1::vector<std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>>, std::__1::allocator<std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>>>>" ||
             type == "std::vector<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::allocator<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > > >")
        return "[String, ...]";
    else if (type == "std::__1::vector<int, std::__1::allocator<int>>")
        return "[Integer, ...]";
    else if (type == "std::__1::vector<long long, std::__1::allocator<long long>>")
        return "[Integer, ...]";
    else if (type == "std::__1::vector<double, std::__1::allocator<double>>" ||
             type == "std::vector<double, std::allocator<double> >")
        return "[Real, ...]";
    // clang-format on
    else
        return type;
}

} // namespace

bool
Registry::exists(const std::string & class_name) const
{
    auto it = this->classes.find(class_name);
    return it != this->classes.end();
}

const Registry::Entry &
Registry::get(const std::string & class_name) const
{
    auto it = this->classes.find(class_name);
    if (it == this->classes.end())
        throw Exception("Class '{}' is not registered.", class_name);
    return it->second;
}

std::vector<Registry::ObjectDescription>
Registry::get_object_description() const
{
    std::vector<Registry::ObjectDescription> objs;
    for (const auto & [class_name, entry] : this->classes) {
        Registry::ObjectDescription descr;
        descr.name = class_name;

        auto params = Parameters((*entry.params_ptr)());
        for (const auto & [name, param] : params) {
            if (!param->is_private) {
                Registry::ObjectDescription::Parameter p;
                p.name = name;
                p.type = human_type_name(utils::demangle(param->type()));
                p.description = param->doc_string;
                p.required = param->required;
                descr.parameters.push_back(p);
            }
        }

        objs.emplace_back(descr);
    }

    return objs;
}

} // namespace godzilla
