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
    CALL_STACK_MSG();
    for (const auto & [key, val] : other.params)
        params[key] = Qtr<Value>(val->copy());
}

Parameters::Parameters(Parameters && other) noexcept : params(std::move(other.params))
{
    CALL_STACK_MSG();
    other.params.clear();
}

Parameters &
Parameters::operator=(const Parameters & other)
{
    CALL_STACK_MSG();
    if (this != &other) {
        clear();
        for (const auto & [key, val] : other.params)
            params[key] = Qtr<Value>(val->copy());
    }
    return *this;
}

Parameters &
Parameters::operator=(Parameters && other) noexcept
{
    CALL_STACK_MSG();
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
    CALL_STACK_MSG();
    for (const auto & [name, value] : rhs)
        this->params[name] = Qtr<Value>(value->copy());
    return *this;
}

bool
Parameters::is_param_required(const std::string & name) const
{
    CALL_STACK_MSG();
    return this->params.count(name) > 0 && this->params.at(name)->required;
}

bool
Parameters::is_param_valid(const std::string & name) const
{
    CALL_STACK_MSG();
    return this->params.count(name) > 0 && this->params.at(name)->valid;
}

bool
Parameters::is_param_private(const std::string & name) const
{
    CALL_STACK_MSG();
    return this->params.count(name) > 0 && this->params.at(name)->is_private;
}

std::string
Parameters::get_doc_string(const std::string & name) const
{
    CALL_STACK_MSG();
    auto it = this->params.find(name);
    if (it != this->params.end())
        return it->second->doc_string;
    else
        return {};
}

Parameters::iterator
Parameters::begin()
{
    CALL_STACK_MSG();
    return this->params.begin();
}

Parameters::const_iterator
Parameters::begin() const
{
    CALL_STACK_MSG();
    return this->params.begin();
}

Parameters::iterator
Parameters ::end()
{
    CALL_STACK_MSG();
    return this->params.end();
}

Parameters::const_iterator
Parameters::end() const
{
    CALL_STACK_MSG();
    return this->params.end();
}

void
Parameters::clear()
{
    CALL_STACK_MSG();
    this->params.clear();
}

} // namespace godzilla
