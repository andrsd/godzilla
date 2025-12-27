// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Registry.h"
#include "godzilla/Utils.h"

namespace godzilla {

bool
Registry::exists(String class_name) const
{
    auto it = this->classes.find(class_name);
    return it != this->classes.end();
}

const Registry::Entry &
Registry::get(String class_name) const
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
                p.type = utils::human_type_name(param->type());
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
