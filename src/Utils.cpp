// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Utils.h"
#include "godzilla/CallStack.h"
#include "godzilla/PrintInterface.h"
#include "godzilla/Terminal.h"
#include "godzilla/UnstructuredMesh.h"
#include <sys/stat.h>
#include <chrono>
#include <fmt/printf.h>
#ifdef HAVE_CXXABI_H
    #include <cxxabi.h>
#endif

namespace godzilla {

namespace {

Optional<Int>
parse_region(String s)
{
    if (s.length() == 0)
        return std::nullopt;

    Int value = 0;
    auto [ptr, ec] = std::from_chars(s.data(), s.data() + s.length(), value);

    if (ec == std::errc() && ptr == s.data() + s.length() && value >= 0)
        return value;

    return std::nullopt;
}

} // namespace

namespace utils {

bool
path_exists(String path)
{
    CALL_STACK_MSG();
    struct stat buffer = {};
    return (stat(path.c_str(), &buffer) == 0);
}

String
to_upper(String name)
{
    CALL_STACK_MSG();
    String upper(name);
    return upper.to_upper();
}

String
to_lower(String name)
{
    CALL_STACK_MSG();
    String lower(name);
    return lower.to_lower();
}

bool
has_suffix(String str, String suffix)
{
    CALL_STACK_MSG();
    return str.ends_with(suffix);
}

bool
ends_with(String str, String end)
{
    CALL_STACK_MSG();
    return str.ends_with(end);
}

bool
has_prefix(String str, String prefix)
{
    CALL_STACK_MSG();
    return str.starts_with(prefix);
}

bool
starts_with(String str, String start)
{
    CALL_STACK_MSG();
    return str.starts_with(start);
}

String
human_time(PetscLogDouble time)
{
    CALL_STACK_MSG();
    using namespace std::chrono;
    duration<double, std::micro> us(time * 1e6);
    auto h = duration_cast<hours>(us);
    us -= h;
    auto m = duration_cast<minutes>(us);
    us -= m;
    auto s = duration_cast<milliseconds>(us) / 1000.;

    std::vector<String> strs;
    if (h.count() > 0)
        strs.push_back(fmt::format("{}h", h.count()));
    if (m.count() > 0)
        strs.push_back(fmt::format("{}m", m.count()));
    if ((s.count() > 0) || (h.count() == 0 && m.count() == 0))
        strs.push_back(fmt::format("{:.2f}s", s.count()));
    return join(" ", strs);
}

String
human_type_name(String type)
{
    // clang-format off
    if (type == "std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>>" ||
        type == "NSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEEE" ||
        type == "std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >" ||
        type == "N8godzilla6StringE")
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

String
demangle(String mangled_name)
{
#ifdef HAVE_CXXABI_H
    int status = -1;
    std::unique_ptr<char, void (*)(void *)> res {
        abi::__cxa_demangle(mangled_name.c_str(), nullptr, nullptr, &status),
        std::free
    };
    return (status == 0) ? res.get() : mangled_name;
#else
    return mangled_name;
#endif
}

} // namespace utils

void
print_converged_reason(PrintInterface & pi, bool converged)
{
    CALL_STACK_MSG();
    if (converged)
        pi.lprintln(8, Terminal::green, "Converged");
    else
        pi.lprintln(8, Terminal::red, "Not converged");
}

Int
get_block_id_from_region(const godzilla::UnstructuredMesh & mesh, String region)
{
    auto id = parse_region(region);
    if (id.has_value())
        return id.value();
    else
        return mesh.get_cell_set_id(region);
}

std::vector<String>
split(const char * delim, String line)
{
    std::vector<String> parts;
    if (line.length() > 0) {
        size_t start = 0;
        while (true) {
            auto pos = line.find(delim, start);
            if (!pos.has_value()) {
                parts.emplace_back(line.substr(start));
                break;
            }
            parts.emplace_back(line.substr(start, pos.value() - start));
            start = pos.value() + std::strlen(delim);
        }
    }
    return parts;
}

} // namespace godzilla
