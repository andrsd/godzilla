#include "gmock/gmock.h"
#include "godzilla/String.h"
#include <cctype>
#include <sstream>

using namespace godzilla;

TEST(StringTest, ctor_empty)
{
    String str;
    EXPECT_EQ(str.length(), 0);
}

TEST(StringTest, ctor_const_char)
{
    String str("some text");
    EXPECT_EQ(str.length(), 9);
    EXPECT_TRUE(str == "some text");
}

TEST(StringTest, ctor_std_string)
{
    std::string s("some other text");
    String str(s);
    EXPECT_EQ(str.length(), 15);
}

TEST(StringTest, to_lower)
{
    String str("AsDf");
    EXPECT_EQ(str.to_lower(), "asdf");
}

TEST(StringTest, to_upper)
{
    String str("aSdF");
    EXPECT_EQ(str.to_upper(), "ASDF");
}

TEST(StringTest, ends_with)
{
    String str("asdf");
    EXPECT_TRUE(str.ends_with("df"));
    EXPECT_FALSE(str.ends_with("long_string"));
    EXPECT_FALSE(str.ends_with("as"));
}

TEST(StringTest, starts_with)
{
    String str("asdf");
    EXPECT_TRUE(str.starts_with("as"));
    EXPECT_FALSE(str.starts_with("long_string"));
    EXPECT_FALSE(str.starts_with("df"));
}

TEST(StringTest, op_assign_str)
{
    String str;
    str = String("hello");
    EXPECT_EQ(str.length(), 5);
    EXPECT_EQ(str, "hello");
}

TEST(StringTest, op_assign_cchar)
{
    String str;
    str = "HELLO";
    EXPECT_EQ(str.length(), 5);
    EXPECT_EQ(str, "HELLO");
}

TEST(StringTest, iostream_shl)
{
    testing::internal::CaptureStdout();
    String str("ahoy");
    std::cout << str << std::endl;
    auto out = testing::internal::GetCapturedStdout();
    EXPECT_THAT(out, testing::HasSubstr("ahoy"));
}

TEST(StringTest, ostrstream_shl)
{
    std::ostringstream ss;
    String hello("Hello");
    ss << hello;
    ss << ", ";
    ss << "1234";
    EXPECT_EQ(ss.str(), "Hello, 1234");
}

TEST(StringTest, iostream_shr)
{
    std::string str("ahoy");
    std::stringstream ss(str);
    String s;
    ss >> s;
    EXPECT_EQ(s, "ahoy");
}

TEST(StringTest, fmt_print)
{
    testing::internal::CaptureStdout();
    String s("ahoy");
    fmt::print("{}", s);
    auto out = testing::internal::GetCapturedStdout();
    EXPECT_THAT(out, testing::HasSubstr("ahoy"));
}

TEST(StringTest, fmt_format)
{
    String world("world");
    String greeting = fmt::format("hello, {}", world);
    EXPECT_EQ(greeting, "hello, world");
}

TEST(StringTest, append)
{
    String hello("hello");
    String world("world");
    String greeting = hello;
    greeting.append(", ");
    greeting.append(world);
    EXPECT_EQ(greeting, "hello, world");
}

TEST(StringTest, substr_start)
{
    String hello("hello");
    EXPECT_EQ(hello.substr(2), "llo");
    EXPECT_EQ(hello.substr(6), "");
}

TEST(StringTest, substr_start_n_chars)
{
    String hello("hello");
    EXPECT_EQ(hello.substr(1, 3), "ell");
    EXPECT_EQ(hello.substr(3, 2), "");
}

TEST(StringTest, find_existing_substring)
{
    String hello("hello");
    auto r = hello.find("ll");
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r.value(), 2);
}

TEST(StringTest, find_existing_substring_from_middle)
{
    String hello("hello wally");
    auto r = hello.find("ll", 4);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r.value(), 8);
}

TEST(StringTest, find_non_existing_substring)
{
    String hello("hello");
    auto r = hello.find("asdf");
    ASSERT_FALSE(r.has_value());
}

TEST(StringTest, replace_1a_with_shorter_str)
{
    String str("The quick brown fox jumps");
    str.replace(10, 9, "red cat");
    EXPECT_EQ(str, "The quick red cat jumps");
}

TEST(StringTest, replace_1b_with_longer_str)
{
    String str("The quick brown fox jumps");
    str.replace(10, 9, "green chameleon");
    EXPECT_EQ(str, "The quick green chameleon jumps");
}
