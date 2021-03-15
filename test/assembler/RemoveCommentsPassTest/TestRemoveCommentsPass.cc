#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "TestUtils.h"

#include <memory>

#include "RemoveCommentsPass.h"

using namespace std;
using namespace gbxasm::frontend::passes;
using namespace gbxasm;

TEST(AssemblerTests_RemoveCommentsPass, ConstructionTest)
{
    auto removeComments = make_shared<RemoveCommentsPass>();
}

TEST(AssemblerTests_RemoveCommentsPass, CallResultWithoutProcessing)
{
    auto removeComments = make_shared<RemoveCommentsPass>();
    
    ASSERT_EXCEPTION( { auto result = removeComments->Result(); }, 
                      PreProcessorException, 
                      "Attempted to read a result without calling 'Process'");
}

TEST(AssemblerTests_RemoveCommentsPass, PreserveSourceWithoutComments)
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

TEST(AssemblerTests_RemoveCommentsPass, FullyRemoveSingleLineComment)
{
    const string input =  "; Single Line Comment\n";
    const string output = "                     \n";
    auto removeComments = make_shared<RemoveCommentsPass>();

    removeComments->Process(input);
    auto result = removeComments->Result();

    EXPECT_STREQ(output.c_str(), result.c_str());
}

TEST(AssemblerTests_RemoveCommentsPass, FullyRemoveSingleLineCommentWithoutEndline)
{
    const string input =  "; Comment at the end of the file";
    const string output = "                                ";
    auto removeComments = make_shared<RemoveCommentsPass>();

    removeComments->Process(input);
    auto result = removeComments->Result();

    EXPECT_STREQ(output.c_str(), result.c_str());
}

TEST(AssemblerTests_RemoveCommentsPass, MultipleCommentLines)
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

TEST(AssemblerTests_RemoveCommentsPass, FullyRemoveMultipleLineComment)
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

TEST(AssemblerTests_RemoveCommentsPass, FullyRemoveMultipleLineCommentAtTheEndOfTheFile)
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

TEST(AssemblerTests_RemoveCommentsPass, MalformedMultipleLineComment)
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

TEST(AssemblerTests_RemoveCommentsPass, FullyRemoveMultipleLineCommentAndKeepCodeIntact)
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

TEST(AssemblerTests_RemoveCommentsPass, MultipleLineCommentAmongCode)
{
    const string input =  "ADD A;:, [DE]:;, [HL]\n";
    const string output = "ADD A          , [HL]\n";
    
    auto removeComments = make_shared<RemoveCommentsPass>();

    removeComments->Process(input);
    auto result = removeComments->Result();

    EXPECT_STREQ(output.c_str(), result.c_str());
}

TEST(AssemblerTests_RemoveCommentsPass, AvoidCommentsWithinString1)
{
    const string input =  "\"; This is not a comment\"";
    const string output = "\"; This is not a comment\"";
    
    auto removeComments = make_shared<RemoveCommentsPass>();

    removeComments->Process(input);
    auto result = removeComments->Result();

    EXPECT_STREQ(output.c_str(), result.c_str());
}

TEST(AssemblerTests_RemoveCommentsPass, AvoidCommentsWithinString2)
{
    const string input =  "\"; This is not a comment";
    const string output = "\"; This is not a comment";
    
    auto removeComments = make_shared<RemoveCommentsPass>();

    removeComments->Process(input);
    auto result = removeComments->Result();

    EXPECT_STREQ(output.c_str(), result.c_str());
}

TEST(AssemblerTests_RemoveCommentsPass, AvoidCommentsWithinString3)
{
    const string input =  "\"; This is not a comment\";LD A, 0xFF";
    const string output = "\"; This is not a comment\"           ";
    
    auto removeComments = make_shared<RemoveCommentsPass>();

    removeComments->Process(input);
    auto result = removeComments->Result();

    EXPECT_STREQ(output.c_str(), result.c_str());
}

TEST(AssemblerTests_RemoveCommentsPass, AvoidCommentsWithinString4)
{
    const string input =  "\"; This is not a comment\" ; This is a Comment";
    const string output = "\"; This is not a comment\"                    ";
    
    auto removeComments = make_shared<RemoveCommentsPass>();

    removeComments->Process(input);
    auto result = removeComments->Result();

    EXPECT_STREQ(output.c_str(), result.c_str());
}

TEST(AssemblerTests_RemoveCommentsPass, AvoidCommentsWithinString5)
{
    const string input =  "\";: This is not a comment :;\"; This is a Comment";
    const string output = "\";: This is not a comment :;\"                   ";
    
    auto removeComments = make_shared<RemoveCommentsPass>();

    removeComments->Process(input);
    auto result = removeComments->Result();

    EXPECT_STREQ(output.c_str(), result.c_str());
}

TEST(AssemblerTests_RemoveCommentsPass, AvoidCommentsWithinString6)
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

TEST(AssemblerTests_RemoveCommentsPass, AvoidCommentsWithinString7)
{
    const string input =  "\";: This is not a comment :;\"";
    const string output = "\";: This is not a comment :;\"";
    
    auto removeComments = make_shared<RemoveCommentsPass>();

    removeComments->Process(input);
    auto result = removeComments->Result();

    EXPECT_STREQ(output.c_str(), result.c_str());
}

TEST(AssemblerTests_RemoveCommentsPass, AvoidCommentsWithinString8)
{
    const string input =  "\";: This is not a comment :;";
    const string output = "\";: This is not a comment :;";
    
    auto removeComments = make_shared<RemoveCommentsPass>();

    removeComments->Process(input);
    auto result = removeComments->Result();

    EXPECT_STREQ(output.c_str(), result.c_str());
}

TEST(AssemblerTests_RemoveCommentsPass, AvoidCommentsWithinString9)
{
    const string input =  ";: This is not a comment :;\"";
    const string output = "                           \"";
    
    auto removeComments = make_shared<RemoveCommentsPass>();

    removeComments->Process(input);
    auto result = removeComments->Result();

    EXPECT_STREQ(output.c_str(), result.c_str());
}

TEST(AssemblerTests_RemoveCommentsPass, AvoidCommentsWithinString10)
{
    const string input =  ";  This is not a comment   \n\"";
    const string output = "                           \n\"";
    
    auto removeComments = make_shared<RemoveCommentsPass>();

    removeComments->Process(input);
    auto result = removeComments->Result();

    EXPECT_STREQ(output.c_str(), result.c_str());
}

TEST(AssemblerTests_RemoveCommentsPass, AvoidCommentsWithinString11)
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

TEST(AssemblerTests_RemoveCommentsPass, AvoidCommentsWithinCharBase)
{
    const string input =  "';'";
    const string output = "';'";
    
    auto removeComments = make_shared<RemoveCommentsPass>();

    removeComments->Process(input);
    auto result = removeComments->Result();

    EXPECT_STREQ(output.c_str(), result.c_str());
}

TEST(AssemblerTests_RemoveCommentsPass, AvoidCommentsWithinCharBase2)
{
    const string input =  "';::;'";
    const string output = "';::;'";
    
    auto removeComments = make_shared<RemoveCommentsPass>();

    removeComments->Process(input);
    auto result = removeComments->Result();

    EXPECT_STREQ(output.c_str(), result.c_str());
}

TEST(AssemblerTests_RemoveCommentsPass, AvoidCommentsWithinChar1)
{
    const string input =  "'; This is not a comment'";
    const string output = "'; This is not a comment'";
    
    auto removeComments = make_shared<RemoveCommentsPass>();

    removeComments->Process(input);
    auto result = removeComments->Result();

    EXPECT_STREQ(output.c_str(), result.c_str());
}

TEST(AssemblerTests_RemoveCommentsPass, AvoidCommentsWithinChar2)
{
    const string input =  "'; This is not a comment";
    const string output = "'; This is not a comment";
    
    auto removeComments = make_shared<RemoveCommentsPass>();

    removeComments->Process(input);
    auto result = removeComments->Result();

    EXPECT_STREQ(output.c_str(), result.c_str());
}

TEST(AssemblerTests_RemoveCommentsPass, AvoidCommentsWithinChar3)
{
    const string input =  "'; This is not a comment';LD A, 0xFF";
    const string output = "'; This is not a comment'           ";
    
    auto removeComments = make_shared<RemoveCommentsPass>();

    removeComments->Process(input);
    auto result = removeComments->Result();

    EXPECT_STREQ(output.c_str(), result.c_str());
}

TEST(AssemblerTests_RemoveCommentsPass, AvoidCommentsWithinChar4)
{
    const string input =  "'; This is not a comment' ; This is a Comment";
    const string output = "'; This is not a comment'                    ";
    
    auto removeComments = make_shared<RemoveCommentsPass>();

    removeComments->Process(input);
    auto result = removeComments->Result();

    EXPECT_STREQ(output.c_str(), result.c_str());
}

TEST(AssemblerTests_RemoveCommentsPass, AvoidCommentsWithinChar5)
{
    const string input =  "';: This is not a comment :;'; This is a Comment";
    const string output = "';: This is not a comment :;'                   ";
    
    auto removeComments = make_shared<RemoveCommentsPass>();

    removeComments->Process(input);
    auto result = removeComments->Result();

    EXPECT_STREQ(output.c_str(), result.c_str());
}

TEST(AssemblerTests_RemoveCommentsPass, AvoidCommentsWithinChar6)
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

TEST(AssemblerTests_RemoveCommentsPass, AvoidCommentsWithinChar7)
{
    const string input =  "';: This is not a comment :;'";
    const string output = "';: This is not a comment :;'";
    
    auto removeComments = make_shared<RemoveCommentsPass>();

    removeComments->Process(input);
    auto result = removeComments->Result();

    EXPECT_STREQ(output.c_str(), result.c_str());
}

TEST(AssemblerTests_RemoveCommentsPass, AvoidCommentsWithinChar8)
{
    const string input =  "';: This is not a comment :;";
    const string output = "';: This is not a comment :;";
    
    auto removeComments = make_shared<RemoveCommentsPass>();

    removeComments->Process(input);
    auto result = removeComments->Result();

    EXPECT_STREQ(output.c_str(), result.c_str());
}

TEST(AssemblerTests_RemoveCommentsPass, AvoidCommentsWithinChar9)
{
    const string input =  ";: This is not a comment :;'";
    const string output = "                           '";
    
    auto removeComments = make_shared<RemoveCommentsPass>();

    removeComments->Process(input);
    auto result = removeComments->Result();

    EXPECT_STREQ(output.c_str(), result.c_str());
}

TEST(AssemblerTests_RemoveCommentsPass, AvoidCommentsWithinChar10)
{
    const string input =  ";  This is not a comment   \n'";
    const string output = "                           \n'";
    
    auto removeComments = make_shared<RemoveCommentsPass>();

    removeComments->Process(input);
    auto result = removeComments->Result();

    EXPECT_STREQ(output.c_str(), result.c_str());
}

TEST(AssemblerTests_RemoveCommentsPass, AvoidCommentsWithinChar11)
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


TEST(AssemblerTests_RemoveCommentsPass, AvoidCommentsCombinedTest)
{
    const string input =  "\"'; This is a com'\";ment\n";

    const string output = "\"'; This is a com'\"     \n";
    
    auto removeComments = make_shared<RemoveCommentsPass>();

    removeComments->Process(input);
    auto result = removeComments->Result();

    EXPECT_STREQ(output.c_str(), result.c_str());
}

TEST(AssemblerTests_RemoveCommentsPass, AvoidCommentsCombinedTest2)
{
    const string input =  "\"\"'; This is a com'\";ment\n";

    const string output = "\"\"'; This is a com'\";ment\n";
    
    auto removeComments = make_shared<RemoveCommentsPass>();

    removeComments->Process(input);
    auto result = removeComments->Result();

    EXPECT_STREQ(output.c_str(), result.c_str());
}

TEST(AssemblerTests_RemoveCommentsPass, AvoidCommentsCombinedTest3)
{
    const string input =  "\"';: This is a com :;\"';ment\n";

    const string output = "\"';: This is a com :;\"';ment\n";
    
    auto removeComments = make_shared<RemoveCommentsPass>();

    removeComments->Process(input);
    auto result = removeComments->Result();

    EXPECT_STREQ(output.c_str(), result.c_str());
}