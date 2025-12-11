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
parse_region(const String & s)
{
    if (s.empty())
        return std::nullopt;

    Int value = 0;
    auto [ptr, ec] = std::from_chars(s.data(), s.data() + s.size(), value);

    if (ec == std::errc() && ptr == s.data() + s.size() && value >= 0)
        return value;

    return std::nullopt;
}

} // namespace

namespace utils {

bool
path_exists(const String & path)
{
    CALL_STACK_MSG();
    struct stat buffer = {};
    return (stat(path.c_str(), &buffer) == 0);
}

String
to_upper(const String & name)
{
    CALL_STACK_MSG();
    String upper(name);
    std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
    return upper;
}

String
to_lower(const String & name)
{
    CALL_STACK_MSG();
    String lower(name);
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    return lower;
}

bool
has_suffix(const String & str, const String & suffix)
{
    CALL_STACK_MSG();
    return str.size() >= suffix.size() &&
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

bool
ends_with(const String & str, const String & end)
{
    CALL_STACK_MSG();
    return has_suffix(str, end);
}

bool
has_prefix(const String & str, const String & prefix)
{
    CALL_STACK_MSG();
    return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
}

bool
starts_with(const String & str, const String & start)
{
    CALL_STACK_MSG();
    return has_prefix(str, start);
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

    String tm;
    if (h.count() > 0)
        tm += fmt::format(" {}h", h.count());
    if (m.count() > 0)
        tm += fmt::format(" {}m", m.count());
    if ((s.count() > 0) || (h.count() == 0 && m.count() == 0)) {
        tm += fmt::format(" {:.2f}", s.count());
        tm += fmt::format("s");
    }
    return tm.substr(1);
}

String
human_type_name(const String & type)
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
demangle(const String & mangled_name)
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
get_block_id_from_region(const godzilla::UnstructuredMesh & mesh, const String & region)
{
    auto id = parse_region(region);
    if (id.has_value())
        return id.value();
    else
        return mesh.get_cell_set_id(region);
}

} // namespace godzilla
