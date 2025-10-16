// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Parameters.h"

namespace godzilla {

Parameters::~Parameters()
{
    clear();
}

Parameters::Parameters(const Parameters & other)
{
    for (const auto & [key, val] : other.params)
        params[key] = val->copy();
}

Parameters::Parameters(Parameters && other) noexcept : params(std::move(other.params))
{
    other.params.clear();
}

Parameters &
Parameters::operator=(const Parameters & other)
{
    if (this != &other) {
        clear();
        for (const auto & [key, val] : other.params)
            params[key] = val->copy();
    }
    return *this;
}

Parameters &
Parameters::operator=(Parameters && other) noexcept
{
    if (this != &other) {
        clear();
        params = std::move(other.params);
        other.params.clear();
    }
    return *this;
}

Parameters &
Parameters::operator+=(const Parameters & rhs)
{
    for (const auto & [name, value] : rhs) {
        auto jt = this->params.find(name);
        if (jt != this->params.end())
            delete jt->second;
        this->params[name] = value->copy();
    }
    return *this;
}

bool
Parameters::is_param_required(const std::string & name) const
{
    return this->params.count(name) > 0 && this->params.at(name)->required;
}

bool
Parameters::is_param_valid(const std::string & name) const
{
    return this->params.count(name) > 0 && this->params.at(name)->valid;
}

bool
Parameters::is_private(const std::string & name) const
{
    return this->params.count(name) > 0 && this->params.at(name)->is_private;
}

std::string
Parameters::type(const std::string & name) const
{
    return this->params.at(name)->type();
}

std::string
Parameters::get_doc_string(const std::string & name) const
{
    auto it = this->params.find(name);
    if (it != this->params.end())
        return it->second->doc_string;
    else
        return {};
}

Parameters::iterator
Parameters::begin()
{
    return this->params.begin();
}

Parameters::const_iterator
Parameters::begin() const
{
    return this->params.begin();
}

Parameters::iterator
Parameters ::end()
{
    return this->params.end();
}

Parameters::const_iterator
Parameters::end() const
{
    return this->params.end();
}

void
Parameters::clear()
{
    for (auto & [_, value] : this->params)
        delete value;
    this->params.clear();
}

} // namespace godzilla
