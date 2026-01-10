#include "src/semantic/semantic.h"
#include "src/lexer/lexer.h"
#include "src/parser/parser.h"
#include "src/ast/c.h"
#include "gtest/gtest.h"
#include <stdexcept>

void execute_semantic_analysis(std::string code) {
    auto l = Lexer::ManualLexer(code);
    Parser::RecursiveDescentParser p(l.Lex());
    auto program = p.parse();
    ASSERT_TRUE(program.has_value());
    auto semantic_visitor = Semantic::VariableResolutionVisitor();
    auto semantic_c_program = semantic_visitor.process(*program.value());
}

std::string undeclared_call = 
"int main() {"
"    return foo(2);"
"}";

std::string dual_decl =
"int foo(int a, int b){return 2;}"
"int foo(int a, int b) {return 4;}";


std::string func_as_var =
"int main() {"
"    int foo(int a);"
"    return 2 + foo;"
"}";

std::string nested_decl = 
"int main() {"
"    int foo() {"
"        return 2>4;"
"    }"
"    return 0;"
"}";

std::string bad_arg_count =
"int foo(int a, int b);"
"int main() {"
"    return foo(2);"
"}";

auto redeclare_wrong_args_outer =
"int foo(int a, int b);"
"int foo(int x, int y, int z);"
"int main() {"
"}";

auto clashing_declaration_same_scope = 
"int main() {"
"    int a;"
"    int a();"
"    return 2;"
"}";

auto redeclare_wrong_args_inner =
"int foo(int a, int b);"
"int main() {"
"    int foo(int x, int y, int z);"
"}";

TEST(SemanticTest, InvalidFunctionUsage) {
    ASSERT_THROW(execute_semantic_analysis(undeclared_call), std::runtime_error);
    ASSERT_THROW(execute_semantic_analysis(dual_decl), std::runtime_error);
    ASSERT_THROW(execute_semantic_analysis(func_as_var), std::runtime_error);
    ASSERT_THROW(execute_semantic_analysis(nested_decl), std::runtime_error);
    ASSERT_THROW(execute_semantic_analysis(bad_arg_count), std::runtime_error);
    ASSERT_THROW(execute_semantic_analysis(redeclare_wrong_args_outer), std::runtime_error);
    ASSERT_THROW(execute_semantic_analysis(undeclared_call), std::runtime_error);
    ASSERT_THROW(execute_semantic_analysis(clashing_declaration_same_scope), std::runtime_error);
    ASSERT_THROW(execute_semantic_analysis(redeclare_wrong_args_inner), std::runtime_error);
}

std::string valid_usage =
"int foo(int x, int y);"
"int bar(int z);"
"int main() {"
"    int foo(int a, int b);"
"    int bar(int z);"
"    if (2>10) {"
"        return bar(10)*foo(2,3) + bar(20);"
"    } else {"
"        return bar(20)*foo(40,80);"
"    }"
"}"
"int foo(int x, int y) {"
"    int bar(int k);"
"    return bar(x) + 2*y;"
"}"
"int bar(int x) {"
"    int foo(int w, int z);"
"    return x > 4;"
"}";

TEST(SemanticTest, ValidFunctionUsage) {
    ASSERT_NO_THROW(execute_semantic_analysis(valid_usage));
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
