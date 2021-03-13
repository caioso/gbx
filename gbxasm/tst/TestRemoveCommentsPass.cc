#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "TestUtils.h"

#include <memory>

#include "../src/frontend/passes/RemoveCommentsPass.h"

using namespace std;
using namespace gbxasm::frontend::passes;
using namespace gbxasm;

TEST(TestRemoveCommentsPass, ConstructionTest)
{
    auto removeComments = make_shared<RemoveCommentsPass>();
}

TEST(TestRemoveCommentsPass, CallResultWithoutProcessing)
{
    auto removeComments = make_shared<RemoveCommentsPass>();
    
    ASSERT_EXCEPTION( { auto result = removeComments->Result(); }, 
                      PreProcessorException, 
                      "Attempted to read a result without calling 'Process'");
}

TEST(TestRemoveCommentsPass, PreserveSourceWithoutComments)
{
    const string input =  "LD HL 0xFFE1 \n"
                          "LD A, [HL]   \n";
    const string output = "LD HL 0xFFE1 \n"
                          "LD A, [HL]   \n";
    auto removeComments = make_shared<RemoveCommentsPass>();

    removeComments->Process(input);
    auto result = removeComments->Result();

    EXPECT_STREQ(output.c_str(), result.c_str());
}

TEST(TestRemoveCommentsPass, FullyRemoveSingleLineComment)
{
    const string input =  "; Single Line Comment\n";
    const string output = "                     \n";
    auto removeComments = make_shared<RemoveCommentsPass>();

    removeComments->Process(input);
    auto result = removeComments->Result();

    EXPECT_STREQ(output.c_str(), result.c_str());
}

TEST(TestRemoveCommentsPass, FullyRemoveSingleLineCommentWithoutEndline)
{
    const string input =  "; Comment at the end of the file";
    const string output = "                                ";
    auto removeComments = make_shared<RemoveCommentsPass>();

    removeComments->Process(input);
    auto result = removeComments->Result();

    EXPECT_STREQ(output.c_str(), result.c_str());
}

TEST(TestRemoveCommentsPass, MultipleCommentLines)
{
    const string input =  "; Line 1 Commment\n"
                          "; Line 2 Commment\n"
                          "LD A, B ; Loads Register A with B's Content";

    const string output = "                 \n"
                          "                 \n"
                          "LD A, B                                    ";

    auto removeComments = make_shared<RemoveCommentsPass>();

    removeComments->Process(input);
    auto result = removeComments->Result();

    EXPECT_STREQ(output.c_str(), result.c_str());
}

TEST(TestRemoveCommentsPass, FullyRemoveMultipleLineComment)
{
    const string input =  ";: Multiple  \n"
                          "   Line      \n"
                          "   Comment :;\n";
    const string output = "             \n"
                          "             \n"
                          "             \n";
    
    auto removeComments = make_shared<RemoveCommentsPass>();

    removeComments->Process(input);
    auto result = removeComments->Result();

    EXPECT_STREQ(output.c_str(), result.c_str());
}

TEST(TestRemoveCommentsPass, FullyRemoveMultipleLineCommentAtTheEndOfTheFile)
{
    const string input =  ";: Multiple  \n"
                          "   Line      \n"
                          "   Comment :;";
    const string output = "             \n"
                          "             \n"
                          "             ";
    
    auto removeComments = make_shared<RemoveCommentsPass>();

    removeComments->Process(input);
    auto result = removeComments->Result();

    EXPECT_STREQ(output.c_str(), result.c_str());
}

TEST(TestRemoveCommentsPass, MalformedMultipleLineComment)
{
    const string noEnding =  ";: Wrong   \n"
                          "   Comment \n"
                          "   1       \n";
    
    const string noStarting =  "   Wrong   \n"
                          "   Comment \n"
                          "   2     :;\n";
    
    const string onlyStarting =  ";:";
    const string onlyEnding =  ":;";
    
    
    auto removeComments = make_shared<RemoveCommentsPass>();

    ASSERT_EXCEPTION( { removeComments->Process(noEnding); }, 
                      PreProcessorException, 
                      "Expected ':;' in multiple line comment");

    ASSERT_EXCEPTION( { removeComments->Process(noStarting); }, 
                      PreProcessorException, 
                      "Expected ';:' in multiple line comment");
    
    ASSERT_EXCEPTION( { removeComments->Process(onlyStarting); }, 
                      PreProcessorException, 
                      "Expected ':;' in multiple line comment");

    ASSERT_EXCEPTION( { removeComments->Process(onlyEnding); }, 
                      PreProcessorException, 
                      "Expected ';:' in multiple line comment");
}

TEST(TestRemoveCommentsPass, FullyRemoveMultipleLineCommentAndKeepCodeIntact)
{
    const string input =  ";: Multiple  \n"
                          "   Line      \n"
                          "   Comment :;\n"
                          "LD F, 0x34 ;: Load F with 0x34 :;\n"
                          ";: Comment :;\n";

    const string output = "             \n"
                          "             \n"
                          "             \n"
                          "LD F, 0x34                       \n"
                          "             \n";
    
    auto removeComments = make_shared<RemoveCommentsPass>();

    removeComments->Process(input);
    auto result = removeComments->Result();

    EXPECT_STREQ(output.c_str(), result.c_str());
}

TEST(TestRemoveCommentsPass, MultipleLineCommentAmongCode)
{
    const string input =  "ADD A;:, [DE]:;, [HL]\n";
    const string output = "ADD A          , [HL]\n";
    
    auto removeComments = make_shared<RemoveCommentsPass>();

    removeComments->Process(input);
    auto result = removeComments->Result();

    EXPECT_STREQ(output.c_str(), result.c_str());
}

TEST(TestRemoveCommentsPass, AvoidCommentsWithinString1)
{
    const string input =  "\"; This is not a comment\"";
    const string output = "\"; This is not a comment\"";
    
    auto removeComments = make_shared<RemoveCommentsPass>();

    removeComments->Process(input);
    auto result = removeComments->Result();

    EXPECT_STREQ(output.c_str(), result.c_str());
}

TEST(TestRemoveCommentsPass, AvoidCommentsWithinString2)
{
    const string input =  "\"; This is not a comment";
    const string output = "\"; This is not a comment";
    
    auto removeComments = make_shared<RemoveCommentsPass>();

    removeComments->Process(input);
    auto result = removeComments->Result();

    EXPECT_STREQ(output.c_str(), result.c_str());
}

TEST(TestRemoveCommentsPass, AvoidCommentsWithinString3)
{
    const string input =  "\"; This is not a comment\";LD A, 0xFF";
    const string output = "\"; This is not a comment\"           ";
    
    auto removeComments = make_shared<RemoveCommentsPass>();

    removeComments->Process(input);
    auto result = removeComments->Result();

    EXPECT_STREQ(output.c_str(), result.c_str());
}

TEST(TestRemoveCommentsPass, AvoidCommentsWithinString4)
{
    const string input =  "\"; This is not a comment\" ; This is a Comment";
    const string output = "\"; This is not a comment\"                    ";
    
    auto removeComments = make_shared<RemoveCommentsPass>();

    removeComments->Process(input);
    auto result = removeComments->Result();

    EXPECT_STREQ(output.c_str(), result.c_str());
}

TEST(TestRemoveCommentsPass, AvoidCommentsWithinString5)
{
    const string input =  "\";: This is not a comment :;\"; This is a Comment";
    const string output = "\";: This is not a comment :;\"                   ";
    
    auto removeComments = make_shared<RemoveCommentsPass>();

    removeComments->Process(input);
    auto result = removeComments->Result();

    EXPECT_STREQ(output.c_str(), result.c_str());
}

TEST(TestRemoveCommentsPass, AvoidCommentsWithinString6)
{
    const string input =  "\";: This is not a comment :;\"\n"
                          ";: Real Comment Here :;";
    const string output = "\";: This is not a comment :;\"\n"
                          "                       ";
    
    auto removeComments = make_shared<RemoveCommentsPass>();

    removeComments->Process(input);
    auto result = removeComments->Result();

    EXPECT_STREQ(output.c_str(), result.c_str());
}

TEST(TestRemoveCommentsPass, AvoidCommentsWithinString7)
{
    const string input =  "\";: This is not a comment :;\"";
    const string output = "\";: This is not a comment :;\"";
    
    auto removeComments = make_shared<RemoveCommentsPass>();

    removeComments->Process(input);
    auto result = removeComments->Result();

    EXPECT_STREQ(output.c_str(), result.c_str());
}

TEST(TestRemoveCommentsPass, AvoidCommentsWithinString8)
{
    const string input =  "\";: This is not a comment :;";
    const string output = "\";: This is not a comment :;";
    
    auto removeComments = make_shared<RemoveCommentsPass>();

    removeComments->Process(input);
    auto result = removeComments->Result();

    EXPECT_STREQ(output.c_str(), result.c_str());
}

TEST(TestRemoveCommentsPass, AvoidCommentsWithinString9)
{
    const string input =  ";: This is not a comment :;\"";
    const string output = "                           \"";
    
    auto removeComments = make_shared<RemoveCommentsPass>();

    removeComments->Process(input);
    auto result = removeComments->Result();

    EXPECT_STREQ(output.c_str(), result.c_str());
}

TEST(TestRemoveCommentsPass, AvoidCommentsWithinString10)
{
    const string input =  ";  This is not a comment   \n\"";
    const string output = "                           \n\"";
    
    auto removeComments = make_shared<RemoveCommentsPass>();

    removeComments->Process(input);
    auto result = removeComments->Result();

    EXPECT_STREQ(output.c_str(), result.c_str());
}

TEST(TestRemoveCommentsPass, AvoidCommentsWithinString11)
{
    const string input =  "; This is a comment\n"
                          "\"; This is NOT is a comment\"\n"
                          ";: This is a comment :;\n"
                          "\";: This is NOT is a comment:;\"\n";

    const string output = "                   \n"
                          "\"; This is NOT is a comment\"\n"
                          "                       \n"
                          "\";: This is NOT is a comment:;\"\n";
    
    auto removeComments = make_shared<RemoveCommentsPass>();

    removeComments->Process(input);
    auto result = removeComments->Result();

    EXPECT_STREQ(output.c_str(), result.c_str());
}

TEST(TestRemoveCommentsPass, AvoidCommentsWithinCharBase)
{
    const string input =  "';'";
    const string output = "';'";
    
    auto removeComments = make_shared<RemoveCommentsPass>();

    removeComments->Process(input);
    auto result = removeComments->Result();

    EXPECT_STREQ(output.c_str(), result.c_str());
}

TEST(TestRemoveCommentsPass, AvoidCommentsWithinCharBase2)
{
    const string input =  "';::;'";
    const string output = "';::;'";
    
    auto removeComments = make_shared<RemoveCommentsPass>();

    removeComments->Process(input);
    auto result = removeComments->Result();

    EXPECT_STREQ(output.c_str(), result.c_str());
}

TEST(TestRemoveCommentsPass, AvoidCommentsWithinChar1)
{
    const string input =  "'; This is not a comment'";
    const string output = "'; This is not a comment'";
    
    auto removeComments = make_shared<RemoveCommentsPass>();

    removeComments->Process(input);
    auto result = removeComments->Result();

    EXPECT_STREQ(output.c_str(), result.c_str());
}

TEST(TestRemoveCommentsPass, AvoidCommentsWithinChar2)
{
    const string input =  "'; This is not a comment";
    const string output = "'; This is not a comment";
    
    auto removeComments = make_shared<RemoveCommentsPass>();

    removeComments->Process(input);
    auto result = removeComments->Result();

    EXPECT_STREQ(output.c_str(), result.c_str());
}

TEST(TestRemoveCommentsPass, AvoidCommentsWithinChar3)
{
    const string input =  "'; This is not a comment';LD A, 0xFF";
    const string output = "'; This is not a comment'           ";
    
    auto removeComments = make_shared<RemoveCommentsPass>();

    removeComments->Process(input);
    auto result = removeComments->Result();

    EXPECT_STREQ(output.c_str(), result.c_str());
}

TEST(TestRemoveCommentsPass, AvoidCommentsWithinChar4)
{
    const string input =  "'; This is not a comment' ; This is a Comment";
    const string output = "'; This is not a comment'                    ";
    
    auto removeComments = make_shared<RemoveCommentsPass>();

    removeComments->Process(input);
    auto result = removeComments->Result();

    EXPECT_STREQ(output.c_str(), result.c_str());
}

TEST(TestRemoveCommentsPass, AvoidCommentsWithinChar5)
{
    const string input =  "';: This is not a comment :;'; This is a Comment";
    const string output = "';: This is not a comment :;'                   ";
    
    auto removeComments = make_shared<RemoveCommentsPass>();

    removeComments->Process(input);
    auto result = removeComments->Result();

    EXPECT_STREQ(output.c_str(), result.c_str());
}

TEST(TestRemoveCommentsPass, AvoidCommentsWithinChar6)
{
    const string input =  "';: This is not a comment :;'\n"
                          ";: Real Comment Here :;";
    const string output = "';: This is not a comment :;'\n"
                          "                       ";
    
    auto removeComments = make_shared<RemoveCommentsPass>();

    removeComments->Process(input);
    auto result = removeComments->Result();

    EXPECT_STREQ(output.c_str(), result.c_str());
}

TEST(TestRemoveCommentsPass, AvoidCommentsWithinChar7)
{
    const string input =  "';: This is not a comment :;'";
    const string output = "';: This is not a comment :;'";
    
    auto removeComments = make_shared<RemoveCommentsPass>();

    removeComments->Process(input);
    auto result = removeComments->Result();

    EXPECT_STREQ(output.c_str(), result.c_str());
}

TEST(TestRemoveCommentsPass, AvoidCommentsWithinChar8)
{
    const string input =  "';: This is not a comment :;";
    const string output = "';: This is not a comment :;";
    
    auto removeComments = make_shared<RemoveCommentsPass>();

    removeComments->Process(input);
    auto result = removeComments->Result();

    EXPECT_STREQ(output.c_str(), result.c_str());
}

TEST(TestRemoveCommentsPass, AvoidCommentsWithinChar9)
{
    const string input =  ";: This is not a comment :;'";
    const string output = "                           '";
    
    auto removeComments = make_shared<RemoveCommentsPass>();

    removeComments->Process(input);
    auto result = removeComments->Result();

    EXPECT_STREQ(output.c_str(), result.c_str());
}

TEST(TestRemoveCommentsPass, AvoidCommentsWithinChar10)
{
    const string input =  ";  This is not a comment   \n'";
    const string output = "                           \n'";
    
    auto removeComments = make_shared<RemoveCommentsPass>();

    removeComments->Process(input);
    auto result = removeComments->Result();

    EXPECT_STREQ(output.c_str(), result.c_str());
}

TEST(TestRemoveCommentsPass, AvoidCommentsWithinChar11)
{
    const string input =  "; This is a comment\n"
                          "'; This is NOT is a comment'\n"
                          ";: This is a comment :;\n"
                          "';: This is NOT is a comment:;'\n";

    const string output = "                   \n"
                          "'; This is NOT is a comment'\n"
                          "                       \n"
                          "';: This is NOT is a comment:;'\n";
    
    auto removeComments = make_shared<RemoveCommentsPass>();

    removeComments->Process(input);
    auto result = removeComments->Result();

    EXPECT_STREQ(output.c_str(), result.c_str());
}


TEST(TestRemoveCommentsPass, AvoidCommentsCombinedTest)
{
    const string input =  "\"'; This is a com'\";ment\n";

    const string output = "\"'; This is a com'\"     \n";
    
    auto removeComments = make_shared<RemoveCommentsPass>();

    removeComments->Process(input);
    auto result = removeComments->Result();

    EXPECT_STREQ(output.c_str(), result.c_str());
}

TEST(TestRemoveCommentsPass, AvoidCommentsCombinedTest2)
{
    const string input =  "\"\"'; This is a com'\";ment\n";

    const string output = "\"\"'; This is a com'\";ment\n";
    
    auto removeComments = make_shared<RemoveCommentsPass>();

    removeComments->Process(input);
    auto result = removeComments->Result();

    EXPECT_STREQ(output.c_str(), result.c_str());
}

TEST(TestRemoveCommentsPass, AvoidCommentsCombinedTest3)
{
    const string input =  "\"';: This is a com :;\"';ment\n";

    const string output = "\"';: This is a com :;\"';ment\n";
    
    auto removeComments = make_shared<RemoveCommentsPass>();

    removeComments->Process(input);
    auto result = removeComments->Result();

    EXPECT_STREQ(output.c_str(), result.c_str());
}