// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <cstdint>
#include <cstring>
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
        if (!this->rep || this->rep->refcount.load(std::memory_order_relaxed) == 1)
            return;

        Rep * fresh = Rep::create(this->rep->capacity);
        fresh->size = this->rep->size;
        std::memcpy(fresh->data, this->rep->data, this->rep->size + 1);

        this->rep->release();
        this->rep = fresh;
    }

    void
    ensure_capacity(uint32_t new_cap)
    {
        if (new_cap <= this->rep->capacity)
            return;

        Rep * fresh = Rep::create(new_cap);
        fresh->size = this->rep->size;
        std::memcpy(fresh->data, this->rep->data, this->rep->size + 1);

        this->rep->release();
        this->rep = fresh;
    }

    Rep * rep { nullptr };

public:
    String() : rep(Rep::create(15)) {}

    String(const char * s)
    {
        uint32_t n = std::strlen(s);
        this->rep = Rep::create(n);
        this->rep->size = n;
        std::memcpy(this->rep->data, s, n + 1);
    }

    String(const std::string & str)
    {
        uint32_t n = str.length();
        this->rep = Rep::create(n);
        this->rep->size = n;
        std::memcpy(this->rep->data, str.c_str(), n + 1);
    }

    // copy

    String(const String & other) noexcept : rep(other.rep)
    {
        if (this->rep)
            this->rep->retain();
    }

    String &
    operator=(const String & other) noexcept
    {
        if (other.rep)
            other.rep->retain();
        if (this->rep)
            this->rep->release();
        this->rep = other.rep;
        return *this;
    }

    // move

    String(String && other) noexcept : rep(other.rep) { other.rep = nullptr; }

    String &
    operator=(String && other) noexcept
    {
        if (this->rep)
            this->rep->release();
        this->rep = other.rep;
        other.rep = nullptr;
        return *this;
    }

    ~String()
    {
        if (this->rep)
            this->rep->release();
    }

    uint32_t
    length() const
    {
        return this->rep->size;
    }

    /// Convert supplied string to upper case.
    ///
    /// @param name The string to convert upper case.
    String
    to_upper() const
    {
        String upper(*this);
        for (uint32_t i = 0; i < this->rep->size; ++i)
            this->rep->data[i] = std::toupper(this->rep->data[i]);
        return upper;
    }

    /// Convert supplied string to lower case.
    ///
    /// @param name The string to convert upper case.
    String
    to_lower() const
    {
        String lower(*this);
        for (uint32_t i = 0; i < this->rep->size; ++i)
            this->rep->data[i] = std::tolower(this->rep->data[i]);
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

    void
    resize(uint32_t new_size)
    {
        throw std::runtime_error("no");
    }

    int32_t
    compare(const String s) const
    {
        return std::strcmp(this->rep->data, s.rep->data);
    }

    int32_t
    compare(int32_t pos, int32_t count, const String s) const
    {
        assert(count <= s.rep->size);
        assert(pos + count <= this->rep->size);
        return std::strncmp(this->rep->data + pos, s.rep->data, count);
    }

    void
    append(const String other)
    {
        if (other.length() == 0)
            return;

        detach();
        ensure_capacity(this->rep->size + other.rep->size);

        std::memcpy(this->rep->data + this->rep->size, other.rep->data, other.rep->size + 1);

        this->rep->size += other.rep->size;
    }

    const char *
    c_str() const
    {
        return this->rep->data;
    }

    bool
    operator==(const String other) const
    {
        return std::strcmp(this->rep->data, other.rep->data) == 0;
    }

    bool
    operator==(const char * other) const
    {
        return std::strcmp(this->rep->data, other) == 0;
    }

    bool
    operator<(const String other) const
    {
        return std::strcmp(this->rep->data, other.rep->data) < 0;
    }

    operator std::string() const { return std::string(this->rep->data); }

    const char *
    data() const
    {
        return this->rep->data;
    }

    char *
    data() noexcept
    {
        return this->rep->data;
    }

private:
};

} // namespace godzilla

inline std::ostream &
operator<<(std::ostream & os, const godzilla::String & obj)
{
    os << obj.c_str();
    return os;
}

inline std::istream &
operator>>(std::istream & is, godzilla::String & obj)
{
    std::string str;
    is >> str;
    obj = str;
    return is;
}

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
    encode(const godzilla::String & rhs)
    {
        // do nothing, becuase we only read YML
        Node node;
        return node;
    }

    static bool
    decode(const Node & node, godzilla::String & rhs)
    {
        // rhs = node.as<std::string>();
        return true;
    }
};
