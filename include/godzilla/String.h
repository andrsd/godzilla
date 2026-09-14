// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Optional.h"
#include "mpicpp-lite/mpicpp-lite.h"
#include <cstdint>
#include <cstring>
#include <optional>
#include <stdexcept>
#include <string>
#include <cassert>
#include <yaml-cpp/yaml.h>
#include <fmt/core.h>

namespace godzilla {

/// String class
class String {
    struct Rep {
        std::atomic<uint32_t> refcount { 1 };
        uint32_t size { 0 };
        uint32_t capacity { 0 };
        char * data { nullptr };

        void
        retain() noexcept
        {
            this->refcount.fetch_add(1, std::memory_order_relaxed);
        }

        void
        release() noexcept
        {
            if (this->refcount.fetch_sub(1, std::memory_order_acq_rel) == 1) {
                delete[] this->data;
                delete this;
            }
        }

        static Rep *
        create(uint32_t cap)
        {
            Rep * r = new Rep;
            r->capacity = cap;
            r->data = new char[cap + 1];
            r->data[0] = '\0';
            return r;
        }
    };

    void
    detach()
    {
        if (!this->rep_ || this->rep_->refcount.load(std::memory_order_relaxed) == 1)
            return;

        Rep * fresh = Rep::create(this->rep_->capacity);
        fresh->size = this->rep_->size;
        std::memcpy(fresh->data, this->rep_->data, this->rep_->size + 1);

        this->rep_->release();
        this->rep_ = fresh;
    }

    void
    ensure_capacity(uint32_t new_cap)
    {
        if (new_cap <= this->rep_->capacity)
            return;

        Rep * fresh = Rep::create(new_cap);
        fresh->size = this->rep_->size;
        std::memcpy(fresh->data, this->rep_->data, this->rep_->size + 1);

        this->rep_->release();
        this->rep_ = fresh;
    }

    Rep * rep_ { nullptr };

public:
    String() : rep_(Rep::create(15)) {}

    String(const char * s)
    {
        uint32_t n = std::strlen(s);
        this->rep_ = Rep::create(n);
        this->rep_->size = n;
        std::memcpy(this->rep_->data, s, n + 1);
    }

    String(const std::string & str)
    {
        uint32_t n = str.length();
        this->rep_ = Rep::create(n);
        this->rep_->size = n;
        std::memcpy(this->rep_->data, str.c_str(), n + 1);
    }

    // copy

    String(const String & other) noexcept : rep_(other.rep_)
    {
        if (this->rep_)
            this->rep_->retain();
    }

    String &
    operator=(const String & other) noexcept
    {
        if (other.rep_)
            other.rep_->retain();
        if (this->rep_)
            this->rep_->release();
        this->rep_ = other.rep_;
        return *this;
    }

    // move

    String(String && other) noexcept : rep_(other.rep_) { other.rep_ = nullptr; }

    String &
    operator=(String && other) noexcept
    {
        if (this->rep_)
            this->rep_->release();
        this->rep_ = other.rep_;
        other.rep_ = nullptr;
        return *this;
    }

    ~String()
    {
        if (this->rep_)
            this->rep_->release();
    }

    uint32_t
    length() const
    {
        return this->rep_->size;
    }

    /// Convert supplied string to upper case.
    ///
    /// @param name The string to convert upper case.
    String
    to_upper() const
    {
        String upper(*this);
        for (uint32_t i = 0; i < this->rep_->size; ++i)
            this->rep_->data[i] = std::toupper(this->rep_->data[i]);
        return upper;
    }

    /// Convert supplied string to lower case.
    ///
    /// @param name The string to convert upper case.
    String
    to_lower() const
    {
        String lower(*this);
        for (uint32_t i = 0; i < this->rep_->size; ++i)
            this->rep_->data[i] = std::tolower(this->rep_->data[i]);
        return lower;
    }

    /// Check if string ends with specified text
    ///
    /// @param suffix The suffix to test
    bool
    ends_with(const String suffix) const
    {
        return length() >= suffix.length() &&
               compare(length() - suffix.length(), suffix.length(), suffix) == 0;
    }

    /// Check if string starts with specified text
    ///
    /// @param prefix The prefix to test
    bool
    starts_with(const String prefix) const
    {
        return length() >= prefix.length() && compare(0, prefix.length(), prefix) == 0;
    }

    int32_t
    compare(const String s) const
    {
        return std::strcmp(this->rep_->data, s.rep_->data);
    }

    int32_t
    compare(int32_t pos, int32_t count, const String s) const
    {
        assert(count <= s.rep->size);
        assert(pos + count <= this->rep->size);
        return std::strncmp(this->rep_->data + pos, s.rep_->data, count);
    }

    void
    append(const String other)
    {
        if (other.length() == 0)
            return;

        detach();
        ensure_capacity(this->rep_->size + other.rep_->size);

        std::memcpy(this->rep_->data + this->rep_->size, other.rep_->data, other.rep_->size + 1);

        this->rep_->size += other.rep_->size;
    }

    String
    substr(uint32_t start) const
    {
        if (start < this->rep_->size) {
            auto n = this->rep_->size - start;
            Rep * sub_rep = Rep::create(n);
            std::strncpy(sub_rep->data, this->rep_->data + start, n);
            sub_rep->data[n] = '\0';
            sub_rep->size = n;
            return String(sub_rep);
        }
        else
            // NOTE: should this be an error?
            return String("");
    }

    String
    substr(uint32_t start, uint32_t len) const
    {
        if (start + len < this->rep_->size) {
            Rep * sub_rep = Rep::create(len);
            std::strncpy(sub_rep->data, this->rep_->data + start, len);
            sub_rep->data[len] = '\0';
            sub_rep->size = len;
            return String(sub_rep);
        }
        else
            // NOTE: should this be an error?
            return String("");
    }

    Optional<uint32_t>
    find(const char * s, uint32_t pos = 0) const
    {
        if (pos < this->rep_->size) {
            auto res = std::strstr(this->rep_->data + pos, s);
            if (res != nullptr)
                return res - this->rep_->data;
            else
                return std::nullopt;
        }
        else
            // NOTE: should this be an error
            return std::nullopt;
    }

    /// Replaces the characters in the range [pos, pos + count) with given characters
    ///
    /// @param pos Start of the substring that is going to be replaced
    /// @param count Length of the substring that is going to be replaced
    /// @param str String to use for replacement
    String &
    replace(uint32_t pos, uint32_t count, String str)
    {
        if (pos + count < length()) {
            auto new_len = length() - count + str.length();
            auto len_end = length() - (pos + count);

            detach();
            ensure_capacity(new_len);

            std::memcpy(this->rep_->data + pos + str.length(),
                        this->rep_->data + pos + count,
                        len_end + 1);
            std::memcpy(this->rep_->data + pos, str.rep_->data, str.length());

            return *this;
        }
        else
            throw std::runtime_error("'pos' + 'count' is larger then length");
    }

    const char *
    c_str() const
    {
        return this->rep_->data;
    }

    bool
    operator==(const String other) const
    {
        return std::strcmp(this->rep_->data, other.rep_->data) == 0;
    }

    bool
    operator==(const char * other) const
    {
        return std::strcmp(this->rep_->data, other) == 0;
    }

    bool
    operator<(const String other) const
    {
        return std::strcmp(this->rep_->data, other.rep_->data) < 0;
    }

    char
    operator[](uint32_t idx) const
    {
        assert(idx <= this->rep->size);
        return this->rep_->data[idx];
    }

    operator std::string() const { return std::string(this->rep_->data); }

    const char *
    data() const
    {
        return this->rep_->data;
    }

    char *
    prepare_write(uint32_t capacity_needed)
    {
        detach();
        ensure_capacity(capacity_needed);
        return rep_->data;
    }

    void
    commit(uint32_t new_size)
    {
        assert(new_size <= rep->capacity);
        rep_->size = new_size;
        rep_->data[new_size] = '\0';
    }

private:
    String(Rep * rep) : rep_(rep) {}
};

inline std::ostream &
operator<<(std::ostream & os, const String & obj)
{
    os << obj.c_str();
    return os;
}

inline std::istream &
operator>>(std::istream & is, String & obj)
{
    std::string str;
    is >> str;
    obj = str;
    return is;
}

template <typename... T>
String
format(fmt::format_string<T...> fmt, T... args)
{
    return fmt::format(fmt, std::forward<T>(args)...);
}

} // namespace godzilla

template <>
struct fmt::formatter<godzilla::String> : fmt::formatter<fmt::string_view> {
    template <typename FormatContext>
    auto
    format(const godzilla::String & str, FormatContext & ctx) const
    {
        return fmt::formatter<fmt::string_view>::format(
            fmt::string_view { str.c_str(), str.length() },
            ctx);
    }
};

template <>
struct YAML::convert<godzilla::String> {
    static Node
    encode(const godzilla::String & /* rhs */)
    {
        // do nothing, becuase we only read YML
        Node node;
        return node;
    }

    static bool
    decode(const Node & /* node */, godzilla::String & /* rhs */)
    {
        // rhs = node.as<std::string>();
        return true;
    }
};

namespace mpicpp_lite {

template <>
inline void
Communicator::send(int dest, Tag tag, const godzilla::String & value) const
{
    if (size() < 2)
        return;
    send(dest, tag, value.data(), value.length() + 1);
}

template <>
inline Status
Communicator::recv(int source, Tag tag, godzilla::String & value) const
{
    std::vector<char> str;
    auto status = recv(source, tag, str);
    value = godzilla::String(str.data());
    return status;
}

} // namespace mpicpp_lite
