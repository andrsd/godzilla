#include <gmock/gmock.h>
#include "godzilla/MemoryArena.h"
#include "godzilla/Allocators.h"

using namespace godzilla;

TEST(MemoryArenaTest, long_strings_are_allocated_from_arena)
{
    // C++ is dickulus
    using String = std::basic_string<char, std::char_traits<char>, MemoryArenaAllocator<char>>;

    MemoryArena<char> arena(1024);
    MemoryArenaAllocator<char> alloc(arena);
    EXPECT_EQ(arena.mark(), 0);

    String str1("I need a very long string that has more than 26 bytes I think", alloc);
#ifdef __clang__
    EXPECT_EQ(arena.mark(), 64);
#else
    EXPECT_EQ(arena.mark(), 62);
#endif
}

TEST(MemoryArenaTest, small_strings_are_not_allocated_from_arena)
{
    // C++ is dickulus
    using String = std::basic_string<char, std::char_traits<char>, MemoryArenaAllocator<char>>;

    MemoryArena<char> arena(1024);
    MemoryArenaAllocator<char> alloc(arena);
    EXPECT_EQ(arena.mark(), 0);

    String str1("a", alloc);
    EXPECT_EQ(arena.mark(), 0);
}

TEST(MemoryArenaTest, rewind_in_arena)
{
    // C++ is dickulus
    using String = std::basic_string<char, std::char_traits<char>, MemoryArenaAllocator<char>>;

    MemoryArena<char> arena(1024);
    MemoryArenaAllocator<char> alloc(arena);
    EXPECT_EQ(arena.mark(), 0);

    String str1("I need a very long string that has more than 26 bytes I think", alloc);
#ifdef __clang__
    EXPECT_EQ(arena.mark(), 64);
#else
    EXPECT_EQ(arena.mark(), 62);
#endif
    {
        auto marker = arena.mark();
        String str2("Avoiding small string allocation optimization by using long text", alloc);
#ifdef __clang__
        EXPECT_EQ(arena.mark(), 136);
#else
        EXPECT_EQ(arena.mark(), 127);
#endif
        arena.rewind(marker);
    }
#ifdef __clang__
    EXPECT_EQ(arena.mark(), 64);
#else
    EXPECT_EQ(arena.mark(), 62);
#endif
}

TEST(MemoryArenaTest, reset_arena)
{
    // C++ is dickulus
    using String = std::basic_string<char, std::char_traits<char>, MemoryArenaAllocator<char>>;

    MemoryArena<char> arena(1024);
    MemoryArenaAllocator<char> alloc(arena);

    String str1("A very long string that has more than 24 characters", alloc);

    arena.reset();
    EXPECT_EQ(arena.mark(), 0);

    // this will override str1
    String str2("Avoiding small string allocation optimization by using long text", alloc);
    EXPECT_EQ(str1, "Avoiding small string allocation optimization by us");
}
