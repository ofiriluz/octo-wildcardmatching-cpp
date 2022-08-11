/**
 * @file wildcard-path-matcher-tests.cpp
 * @author ofir iluz (iluzofir@gmail.com)
 * @brief
 * @version 0.1
 * @date 2022-08-11
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "octo-wildcardmatching-cpp/wildcard-path-matcher.hpp"

#define MATCH 0
#define NO_MATCH 1

void perform_tests(std::vector<std::pair<std::string, bool>>& test_inputs,
                   std::vector<std::string>& wildcard_paths,
                   bool no_match_last_wildcard_as_many_paths)
{
    octo::wildcardmatching::WildcardPathMatcher path_matcher(no_match_last_wildcard_as_many_paths);

    EXPECT_NO_THROW(path_matcher.add_wildcard_paths(wildcard_paths));

    // Go over each input and test it agasint the writable paths
    for (std::vector<std::pair<std::string, bool>>::iterator input_iter = test_inputs.begin();
         input_iter != test_inputs.end();
         ++input_iter)
    {
        std::string match = path_matcher.get_wildcard_match(input_iter->first);
        if (match != "")
        {
            EXPECT_EQ(input_iter->second, MATCH) << "Failed on: [" << input_iter->first << "] Expected: ["
                                                 << input_iter->second << "] Match: [" + match + "]";
        }
        else
        {
            EXPECT_EQ(input_iter->second, NO_MATCH)
                << "Failed on: [" << input_iter->first << "] Expected: [" << input_iter->second << "]";
        }
    }
}

TEST(WildcardPathMatcherTest, TestHasMatch)
{
    std::vector<std::pair<std::string, bool>> test_inputs = {
        {"/usr/lib64/security/pam.so", MATCH},
        {"/usr/pam.so", MATCH},
        {"/usr/bla.txt", NO_MATCH},
        {"/home/john/Desktop/cake.json", NO_MATCH},
        {"/home/tony/test.json", MATCH},
        {"/home/shtivi/.ssh/id_rsa.pub", NO_MATCH},
        {"/home/shtivi/.ssh", MATCH},
        {"/root/.bashrc", MATCH},
        {"/home/john/.bash_profile", MATCH},
        {"/home/john/.bashrc", MATCH},
        {"/tmp/x", NO_MATCH},
        {"/dev/tty", NO_MATCH},
        {"/home/rafi/abjsaabjsjsjs", MATCH},
        {"/home/test/test/test2/test/test/3/.bashrc", MATCH},
        {"/home/test/test/test2/test/test/test2/3/test/test2/test2.txt", NO_MATCH},
        {"/home/test/test/test2/test/t*st/test2/3/test/test2.txt", MATCH},
        {"/etc/opt/x.txt", NO_MATCH},
        {"/etc/x.txt", MATCH},
        {"/a/b/c", MATCH},
        {"/a/b/a/b/c", MATCH},
        {"/b/a/c", NO_MATCH},
        {"/single/abc", MATCH},
        {"/single/ababc", MATCH},
        {"/single/bac", NO_MATCH},
        {"/prexbxaxpost", NO_MATCH},
        {"/prexaxbxpost", MATCH},
        {"/x/test3/x/y/test4/x/y/z/w/test3", MATCH},
        {"/test5/test5/test5/test6", MATCH}};
    std::vector<std::string> wildcard_paths = {"/usr/**/*.so",
                                               "/home/*/*.json",
                                               "/home/*/*js",
                                               "**/.ssh",
                                               "/home/john/.bash*",
                                               "**/.bashrc",
                                               "**/test/test2.txt",
                                               "/etc/*",
                                               "**/a/**/b/**/c",
                                               "/single/*a*b*c",
                                               "/pre*a*b*post",
                                               "**/test3/**/test4/**",
                                               "**/test5/test6"};

    perform_tests(test_inputs, wildcard_paths, false);
}

TEST(WildcardPathMatcherTest, TestHasMatchSingleWildcard)
{
    std::vector<std::pair<std::string, bool>> test_inputs = {
        {"/s1/_a_b_c_a_b_c_a_b_c_a_/", MATCH},
        {"/s1/_a_b_c_a_b_c_a_/", NO_MATCH},
        {"/s2/_a_a_a_b_b_b_a_a_a_/", MATCH},
        {"/s2/aaaaaaabbbbbbaaaaaa/", MATCH},
        {"/s2/aaabbbaaa/", MATCH},
        {"/s3/ababababab/", MATCH},
        {"/s3/ababbaabab/", NO_MATCH},
        {"/s3/aabbaabbaabbaabbaabb/", MATCH},
        {"/s4/aaaaaaaaaaaaaaaaaaaaaaaaab/", MATCH},
        {"/s4/aaaaabaaaaaa/", MATCH},
        {"/s4/_baaaa/", NO_MATCH},
        {"/s5/__a__b__a__", MATCH},
        {"/s5/__a_a_a_b_b_b_a_a_a__/", MATCH},
    };
    std::vector<std::string> wildcard_paths = {"/s1/*a*b*c*a*b*a*b*c*a*/",
                                               "/s2/*a*a*a*b*b*b*a*a*a*/",
                                               "/s3/*a*b*a*b*a*b*a*b*a*b*/",
                                               "/s4/*a*b*/",
                                               "/s5/*a*b*a*"};

    perform_tests(test_inputs, wildcard_paths, false);
}

TEST(WildcardPathMatcherTest, TestHasMatchDoubleWildcard)
{
    std::vector<std::pair<std::string, bool>> test_inputs = {
        {"/d1/_/a/_/b/_/c/_/a/_/b/_/c/_/a/_/b/_/c/_/a/_/", MATCH},
        {"/d1/__/a/_/_/b/_/_/c/_/_/a/_/_/b/_/_/c/_/_/a/_/_/b/_/_/c/_/_/a/_/_/", MATCH},
        {"/d1/_/a/_/b/_/c/_/a/_/b/_/c/_/a/_/", NO_MATCH},
        {"/d2/_/a/_/a/_/a/_/b/_/b/_/b/_/a/_/a/_/a/_/", MATCH},
        {"/d2/a/a/a/a/a/a/a/b/b/b/b/b/b/a/a/a/a/a/a/", MATCH},
        {"/d2/a/a/a/b/b/b/a/a/a/", MATCH},
        {"/d3/a/b/a/b/a/b/a/b/a/b/", MATCH},
        {"/d3/a/b/a/b/b/a/a/b/a/b/", NO_MATCH},
        {"/d3/a/a/b/b/a/a/b/b/a/a/b/b/a/a/b/b/a/a/b/b/", MATCH},
        {"/d4/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/b/", MATCH},
        {"/d4/a/a/a/a/a/b/a/a/a/a/a/a/", MATCH},
        {"/d4/_/b/a/a/a/a/", NO_MATCH},
        {"/d5/xx/a/xx/b/xx/a/xx", MATCH},
        {"/d5/xx/a/a/a/b/b/b/a/a/a/xx/", MATCH}};
    std::vector<std::string> wildcard_paths = {
        "/d1/**/a/**/b/**/c/**/a/**/b/**/a/**/b/**/c/**/a/**/",
        "/d2/**/a/**/a/**/a/**/b/**/b/**/b/**/a/**/a/**/a/**/",
        "/d3/**/a/**/b/**/a/**/b/**/a/**/b/**/a/**/b/**/a/**/b/**/",
        "/d4/**/a/**/b/**/",
        "/d5/**/a/**/b/**/a/**",
    };

    perform_tests(test_inputs, wildcard_paths, false);
}

TEST(WildcardPathMatcherTest, TestHasMatchMixedWildcard)
{
    std::vector<std::pair<std::string, bool>> test_inputs = {{"/m1/aa/aaa/aa/bbb/aa/aaa/", MATCH},
                                                             {"/m1/aa/aaa/bb/bbb/bb/aaa/", MATCH},
                                                             {"/m1/aa/bbb/bb/bbb/bb/aaa/", MATCH},
                                                             {"/m1/bb/aaa/aa/bbb/aa/", MATCH},
                                                             {"/m1/bb/aaa/aa/bbb/aa/bbb/", NO_MATCH},
                                                             {"/m1/bb/bb/aaa/bb/bbb/bbb/", NO_MATCH},
                                                             {"/m1/a/b/a/", MATCH},
                                                             {"/m1/aba/aba/aba/", MATCH},
                                                             {"/m1/aba/", NO_MATCH},
                                                             {"/m1/ab/ab/", NO_MATCH},
                                                             {"/m1/ab/ba/", NO_MATCH},
                                                             {"/m1/aba/aba/", NO_MATCH},
                                                             {"/m2/x/a/b/a/b/c/x/", MATCH},
                                                             {"/m3/x/a/b/a/c/a/b/a/b/c/x/", MATCH}};
    std::vector<std::string> wildcard_paths = {
        "/m1/**/*a*/**/*b*/**/*a*/", "/m2/**/a/b/c/**/x/", "/m3/**/a/b/a/b/c/**/x"};

    perform_tests(test_inputs, wildcard_paths, false);
}

TEST(WildcardPathMatcherTest, TestHasMatchFu)
{
    std::vector<std::pair<std::string, bool>> test_inputs = {{"/fu1/x/a/b/c/x/", MATCH},
                                                             {"/fu1/x/a/a/b/c/x/", MATCH},
                                                             {"/fu1/x/a/b/a/b/c/x/", MATCH},
                                                             {"/fu1/x/a/b/a/a/b/c/x/", MATCH},
                                                             {"/fu1/x/a/b/a/b/a/b/c/x/", MATCH},
                                                             {"/fu1/x/a/b/a/c/x/", NO_MATCH},
                                                             {"/fu1/x/a/b/b/c/x/", NO_MATCH},
                                                             {"/fu1/x/a/b/a/b/a/c/x/", NO_MATCH},
                                                             {"/fu1/x/a/b/a/b/b/c/x/", NO_MATCH},
                                                             {"/fu2/x/a/b/c/x/", MATCH},
                                                             {"/fu2/x/a/a/b/c/x/", MATCH},
                                                             {"/fu2/x/a/b/a/b/c/x/", MATCH},
                                                             {"/fu2/x/a/b/a/a/b/c/x/", MATCH},
                                                             {"/fu2/x/a/b/a/b/a/b/c/x/", MATCH},
                                                             {"/fu2/x/a/b/a/c/x/", NO_MATCH},
                                                             {"/fu2/x/a/b/b/c/x/", NO_MATCH},
                                                             {"/fu2/x/a/b/a/b/a/c/x/", NO_MATCH},
                                                             {"/fu2/x/a/b/a/b/b/c/x/", NO_MATCH},
                                                             {"/fu3/x/a/b/c/d/x/", MATCH},
                                                             {"/fu3/x/a/b/x/c/d/x/", MATCH},
                                                             {"/fu3/x/a/b/x/x/c/d/x/", MATCH},
                                                             {"/fu3/x/a/a/b/x/c/c/d/x/", MATCH},
                                                             {"/fu3/x/a/a/b/c/d/x/", MATCH},
                                                             {"/fu3/x/a/a/b/c/c/d/x/", MATCH},
                                                             {"/fu3/x/a/a/b/c/x/c/d/x/", MATCH},
                                                             {"/fu3/x/a/a/b/c/x/c/c/d/x/", MATCH},
                                                             {"/fu3/x/a/a/b/c/x/x/c/d/x/", MATCH},
                                                             {"/fu4/x/a/b/c/x/", MATCH},
                                                             {"/fu4/x/a/b/x/", NO_MATCH},
                                                             {"/fu4/x/a/b/cc/x/", NO_MATCH},
                                                             {"/fu4/x/a/b/x/", NO_MATCH},
                                                             {"/fu5/x/a/a/b/b/c/c/x/", MATCH},
                                                             {"/fu5/x/a/a/x/b/b/c/c/x/", MATCH},
                                                             {"/fu5/x/a/a/b/b/x/c/c/x/", MATCH},
                                                             {"/fu5/x/a/a/x/b/b/x/c/c/x/", MATCH},
                                                             {"/fu5/x/a/a/b/b/x/", NO_MATCH},
                                                             {"/fu5/x/b/b/c/c/x/", NO_MATCH},
                                                             {"/fu5/x/a/a/b/b/c/x/", NO_MATCH},
                                                             {"/fu5/x/a/b/b/c/c/x/", NO_MATCH},
                                                             {"/fu5/x/a/a/b/c/c/x/", NO_MATCH},
                                                             {"/fu5/x/a/a/c/c/b/b/x/", NO_MATCH},
                                                             {"/fu5/x/b/b/a/a/c/c/x/", NO_MATCH},
                                                             {"/fu6/x/a/a/b/b/c/c/x/", MATCH},
                                                             {"/fu6/x/a/x/a/b/x/b/c/x/c/x/", MATCH},
                                                             {"/fu6/x/a/a/b/x/b/c/x/c/x/", MATCH},
                                                             {"/fu6/x/a/x/a/b/x/b/c/c/x/", MATCH},
                                                             {"/fu6/x/a/x/a/b/b/c/x/c/x/", MATCH},
                                                             {"/fu6/x/a/a/b/b/c/x/c/x/", MATCH},
                                                             {"/fu6/x/a/x/a/b/b/c/c/x/", MATCH},
                                                             {"/fu6/x/a/a/b/x/b/c/c/x/", MATCH},
                                                             {"/fu6/x/a/b/b/c/c/x/", NO_MATCH},
                                                             {"/fu6/x/a/a/b/c/c/x/", NO_MATCH},
                                                             {"/fu6/x/a/a/b/b/c/x/", NO_MATCH},
                                                             {"/fu6/x/a/a/b/c/c/x/", NO_MATCH},
                                                             {"/fu6/x/a/b/b/c/x/", NO_MATCH},
                                                             {"/fu7/x/x", MATCH},
                                                             {"/fu7/x/x/", MATCH},
                                                             {"/fu7/x/x/x", MATCH},
                                                             {"/fu7/x/x/x/", MATCH},
                                                             {"/fu7/x/x/x/x/", MATCH},
                                                             {"/fu7/x/", NO_MATCH},
                                                             {"/fu7/x", NO_MATCH},
                                                             {"/fu7/xx/", NO_MATCH},
                                                             {"/fu7/xx", NO_MATCH},
                                                             {"/fu8/x/x", MATCH},
                                                             {"/fu8/x/x/", MATCH},
                                                             {"/fu8/x/x/x", MATCH},
                                                             {"/fu8/x/x/x/", MATCH},
                                                             {"/fu8/x/x/x/x/", MATCH},
                                                             {"/fu8/x/", NO_MATCH},
                                                             {"/fu8/x", NO_MATCH},
                                                             {"/fu8/xx/", NO_MATCH},
                                                             {"/fu8/xx", NO_MATCH},
                                                             {"/fu9/x/x", MATCH},
                                                             {"/fu9/x/x/", MATCH},
                                                             {"/fu9/x/x/x", NO_MATCH},
                                                             {"/fu9/x/x/x/", NO_MATCH},
                                                             {"/fu9/x/x/x/x/", NO_MATCH},
                                                             {"/fu9/x/", NO_MATCH},
                                                             {"/fu9/x", NO_MATCH},
                                                             {"/fu9/xx/", NO_MATCH},
                                                             {"/fu9/xx", NO_MATCH},
                                                             {"/fu10/x/a/b/", MATCH},
                                                             {"/fu10/x/ab/b/", MATCH},
                                                             {"/fu10/x/a/ab/", MATCH},
                                                             {"/fu10/x/ab/ab/", MATCH},
                                                             {"/fu10/x/aa/bb/", MATCH},
                                                             {"/fu10/x/a/x/b/", NO_MATCH},
                                                             {"/fu10/x/b/a/", NO_MATCH},
                                                             {"/fu10/x/a/a/b/", MATCH},
                                                             {"/fu10/x/a/a/a/b/", MATCH},
                                                             {"/fu10/x/a/a/a/a/b/", MATCH},
                                                             {"/fu10/x/a/a/a/a/b/b", NO_MATCH},
                                                             {"/fu10/x/a/a/a/a/b/b/", NO_MATCH},
                                                             {"/fu10/x/a/a/a/b/b/", NO_MATCH},
                                                             {"/fu10/x/a/a/b/b/", NO_MATCH},
                                                             {"/fu10/x/a/b/b/", NO_MATCH},
                                                             {"/fu10/x/a/b/b", NO_MATCH},
                                                             {"/fu11/x/a/b/", MATCH},
                                                             {"/fu11/x/ab/b/", MATCH},
                                                             {"/fu11/x/a/ab/", MATCH},
                                                             {"/fu11/x/ab/ab/", MATCH},
                                                             {"/fu11/x/aa/bb/", MATCH},
                                                             {"/fu11/x/a/x/b/", NO_MATCH},
                                                             {"/fu11/x/b/a/", NO_MATCH},
                                                             {"/fu11/x/a/a/b/", MATCH},
                                                             {"/fu11/x/a/a/a/b/", MATCH},
                                                             {"/fu11/x/a/a/a/a/b/", MATCH},
                                                             {"/fu11/x/a/a/a/a/b/b", MATCH},
                                                             {"/fu11/x/a/a/a/a/b/b/", MATCH},
                                                             {"/fu11/x/a/a/a/b/b/", MATCH},
                                                             {"/fu11/x/a/a/b/b/", MATCH},
                                                             {"/fu11/x/a/b/b/", MATCH},
                                                             {"/fu11/x/a/b/b", MATCH},
                                                             {"/fu12/x/a/a/b/b/c/c/x/", MATCH},
                                                             {"/fu12/x/a/x/a/b/x/b/c/x/c/x/", MATCH},
                                                             {"/fu12/x/a/a/b/x/b/c/x/c/x/", MATCH},
                                                             {"/fu12/x/a/x/a/b/x/b/c/c/x/", MATCH},
                                                             {"/fu12/x/a/x/a/b/b/c/x/c/x/", MATCH},
                                                             {"/fu12/x/a/a/b/b/c/x/c/x/", MATCH},
                                                             {"/fu12/x/a/x/a/b/b/c/c/x/", MATCH},
                                                             {"/fu12/x/a/a/b/x/b/c/c/x/", MATCH},
                                                             {"/fu12/x/a/b/b/c/c/x/", NO_MATCH},
                                                             {"/fu12/x/a/a/b/c/c/x/", NO_MATCH},
                                                             {"/fu12/x/a/a/b/b/c/x/", NO_MATCH},
                                                             {"/fu12/x/a/a/b/c/c/x/", NO_MATCH},
                                                             {"/fu12/x/a/b/b/c/x/", NO_MATCH},
                                                             {"/fu13/x/a/a/b/b/c/c/x/", MATCH},
                                                             {"/fu13/x/a/a/x/b/b/c/c/x/", MATCH},
                                                             {"/fu13/x/a/a/b/b/x/c/c/x/", MATCH},
                                                             {"/fu13/x/a/a/x/b/b/x/c/c/x/", MATCH},
                                                             {"/fu13/x/a/a/b/b/x/", NO_MATCH},
                                                             {"/fu13/x/b/b/c/c/x/", NO_MATCH},
                                                             {"/fu13/x/a/a/b/b/c/x/", NO_MATCH},
                                                             {"/fu13/x/a/b/b/c/c/x/", NO_MATCH},
                                                             {"/fu13/x/a/a/b/c/c/x/", NO_MATCH},
                                                             {"/fu13/x/a/a/c/c/b/b/x/", NO_MATCH},
                                                             {"/fu13/x/b/b/a/a/c/c/x/", NO_MATCH}};
    std::vector<std::string> wildcard_paths = {"/fu1/**/a/b/c/**/x/",
                                               "/fu2/**/a/*/c/**/x/",
                                               "/fu3/**/a/b/**/c/d/**/x/",
                                               "/fu4/**/a/**/b/**/c/**/x/",
                                               "/fu5/**/a/a/**/b/b/**/c/c/**/x/",
                                               "/fu6/**/a/**/a/b/**/b/c/**/c/**/x/",
                                               "/fu7/**/*/*/**",
                                               "/fu8/**/*/*",
                                               "/fu9/*/*",
                                               "/fu10/**/*a*/*b*",
                                               "/fu11/**/*a*/*b*/**",
                                               "/fu12/**/*a*/**/*a*/*b*/**/*b*/*c*/**/*c*/**/x/",
                                               "/fu13/**/*a*/*a*/**/*b*/*b*/**/*c*/*c*/**/x/"};

    perform_tests(test_inputs, wildcard_paths, false);
}

TEST(WildcardPathMatcherTest, TestHasMatchWithSingleWildcardEndAllowed)
{
    std::vector<std::pair<std::string, bool>> test_inputs = {{"/tmp/x", NO_MATCH},
                                                             {"/home/lisa/ssh/id_rsa.pub", MATCH},
                                                             {"/dev/tty", NO_MATCH},
                                                             {"/root/.bashrc", MATCH},
                                                             {"/var/test.txt", MATCH},
                                                             {"/t/t2ttt/t", MATCH},
                                                             {"/t/t2/t", MATCH},
                                                             {"/t", NO_MATCH},
                                                             {"/t/t2", MATCH},
                                                             {"/t/t2/t2/t2/t", MATCH},
                                                             {"/t2/t2", NO_MATCH},
                                                             {"/t3", MATCH}};
    std::vector<std::string> wildcard_paths = {"/home/lisa/*", "/root/*", "/var/*", "/t/t2*", "/t3/*"};

    perform_tests(test_inputs, wildcard_paths, true);
}

TEST(WildcardPathMatcherTest, TestHasMatchWithSingleWildcardEndDenied)
{
    std::vector<std::pair<std::string, bool>> test_inputs = {{"/tmp/x", NO_MATCH},
                                                             {"/home/lisa/ssh/id_rsa.pub", NO_MATCH},
                                                             {"/dev/tty", NO_MATCH},
                                                             {"/root/.bashrc", MATCH},
                                                             {"/var/test.txt", MATCH}};
    std::vector<std::string> wildcard_paths = {"/home/lisa/*", "/root/*", "/var/*"};

    perform_tests(test_inputs, wildcard_paths, false);
}

TEST(WildcardPathMatcherTest, TestOnlySingleWildcard)
{
    std::vector<std::pair<std::string, bool>> test_inputs = {{"/root", MATCH}, {"/var/test.txt", NO_MATCH}};
    std::vector<std::string> wildcard_paths = {"*"};

    perform_tests(test_inputs, wildcard_paths, false);
}

TEST(WildcardPathMatcherTest, TestOnlyDoubleWildcard)
{
    std::vector<std::pair<std::string, bool>> test_inputs = {{"/root", MATCH}, {"/var/test.txt", MATCH}};
    std::vector<std::string> wildcard_paths = {"**"};

    perform_tests(test_inputs, wildcard_paths, false);
}

TEST(WildcardPathMatcherTest, TestInvalidTripleWildcard_THROW)
{
    std::vector<std::string> wildcard_paths = {"***"};

    octo::wildcardmatching::WildcardPathMatcher path_matcher(false);

    EXPECT_THROW(path_matcher.add_wildcard_paths(wildcard_paths), std::runtime_error);
}

TEST(WildcardPathMatcherTest, TestEmptyString)
{
    std::vector<std::pair<std::string, bool>> test_inputs = {{"/root", NO_MATCH}, {"/var/test.txt", NO_MATCH}};
    std::vector<std::string> wildcard_paths = {""};

    perform_tests(test_inputs, wildcard_paths, true);
}

TEST(WildcardPathMatcherTest, TestValidatePaths)
{
    octo::wildcardmatching::WildcardPathMatcher path_matcher;
    EXPECT_EQ(path_matcher.validate_wildcard_path("**/.bashrc"), true);
    EXPECT_EQ(path_matcher.validate_wildcard_path("***/.bashrc"), false);
    EXPECT_EQ(path_matcher.validate_wildcard_path("*/.bashrc"), true);
    EXPECT_EQ(path_matcher.validate_wildcard_path(".*rc"), true);
    EXPECT_EQ(path_matcher.validate_wildcard_path("/home/**/***/*"), false);
    EXPECT_EQ(path_matcher.validate_wildcard_path("/home/**/**/*"), true);
}

TEST(WildcardPathMatcherTest, TestInvalidPaths)
{
    std::vector<std::pair<std::string, bool>> test_inputs = {{"/tmp///x", NO_MATCH},
                                                             {"/home//lisa/ssh/id_rsa.pub", NO_MATCH},
                                                             {"/dev///tty", NO_MATCH},
                                                             {"/root/.bashrc", MATCH},
                                                             {"/var//test.txt", MATCH}};
    std::vector<std::string> wildcard_paths = {"/home/lisa/*", "/root/*", "/var/*"};

    perform_tests(test_inputs, wildcard_paths, false);
}