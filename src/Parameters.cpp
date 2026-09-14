// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Parameters.h"
#include "godzilla/Error.h"
#include "godzilla/Expected.h"

namespace godzilla {

Parameters::~Parameters()
{
    clear();
}

Parameters::Parameters(const Parameters & other)
{
    CALL_STACK_MSG();
    for (const auto & [key, val] : other.params_)
        params_[key] = Qtr<Value>(val->copy());
}

Parameters::Parameters(Parameters && other) noexcept : params_(std::move(other.params_))
{
    CALL_STACK_MSG();
    other.params_.clear();
}

Parameters &
Parameters::operator=(const Parameters & other)
{
    CALL_STACK_MSG();
    if (this != &other) {
        clear();
        for (const auto & [key, val] : other.params_)
            params_[key] = Qtr<Value>(val->copy());
    }
    return *this;
}

Parameters &
Parameters::operator=(Parameters && other) noexcept
{
    CALL_STACK_MSG();
    if (this != &other) {
        clear();
        params_ = std::move(other.params_);
        other.params_.clear();
    }
    return *this;
}

Parameters &
Parameters::operator+=(const Parameters & rhs)
{
    CALL_STACK_MSG();
    for (const auto & [name, value] : rhs)
        this->params_[name] = Qtr<Value>(value->copy());
    return *this;
}

bool
Parameters::is_param_required(String name) const
{
    CALL_STACK_MSG();
    return this->params_.count(name) > 0 && this->params_.at(name)->required;
}

bool
Parameters::is_param_valid(String name) const
{
    CALL_STACK_MSG();
    return this->params_.count(name) > 0 && this->params_.at(name)->valid;
}

bool
Parameters::is_param_private(String name) const
{
    CALL_STACK_MSG();
    return this->params_.count(name) > 0 && this->params_.at(name)->is_private;
}

String
Parameters::get_doc_string(String name) const
{
    CALL_STACK_MSG();
    auto it = this->params_.find(name);
    if (it != this->params_.end())
        return it->second->doc_string;
    else
        return {};
}

Expected<std::source_location, ErrorCode>
Parameters::get_source_location(String name) const
{
    CALL_STACK_MSG();
    auto it = this->params_.find(name);
    if (it != this->params_.end()) {
        if (it->second->src_loc.has_value())
            return it->second->src_loc.value();
        else
            return Unexpected(ErrorCode::NotSet);
    }
    else
        return Unexpected(ErrorCode::NotFound);
}

Parameters::iterator
Parameters::begin()
{
    CALL_STACK_MSG();
    return this->params_.begin();
}

Parameters::const_iterator
Parameters::begin() const
{
    CALL_STACK_MSG();
    return this->params_.begin();
}

Parameters::iterator
Parameters ::end()
{
    CALL_STACK_MSG();
    return this->params_.end();
}

Parameters::const_iterator
Parameters::end() const
{
    CALL_STACK_MSG();
    return this->params_.end();
}

void
Parameters::clear()
{
    CALL_STACK_MSG();
    this->params_.clear();
}

std::string
Parameters::suggestion(String name) const
{
    auto best_dist = std::numeric_limits<uint32_t>::max();
    Optional<String> best;

    for (const auto & [key, _] : this->params_) {
        auto d = levenshtein_distance(name, key);
        // small bonus if we are matching a prefix of 2 letters
        if ((name.length() >= 2) && key.starts_with(name.substr(0, 2)))
            d -= 1;

        if (d < best_dist) {
            best_dist = d;
            best = key;
        }
    }

    // Heuristic: only accept "close enough"
    if (best && best_dist <= 2)
        return fmt::format(" Did you mean '{}'?", best.value());

    return "";
}

} // namespace godzilla
