#include "src/ast/ast.h"
#include "src/parser/parser.h"
#include "src/lexer/lexer.h"
#include "gtest/gtest.h"
#include "src/tacky/tacky.h"
#include "src/codegen/ast_to_tacky_visitor.h"
#include <string>

namespace Codegen {

const std::string basic_program = 
    "int main() {\n"
    "   return ~(-(~400));\n"
    "}\n";

TEST(TackyTest, BasicProgram) {
    std::unique_ptr<Lexer::Lexer> l = std::make_unique<Lexer::ManualLexer>(basic_program);
    auto lexered_tokens = l->Lex();
    std::unique_ptr<Parser::Parser> p = std::make_unique<Parser::RecursiveDescentParser>(std::move(lexered_tokens));
    std::optional<std::shared_ptr<CAst::ProgramNode>> program_opt = p->parse();
    ASSERT_TRUE(program_opt.has_value());
    auto program = program_opt.value();

    Codegen::AstToTackyVisitor visitor;
    std::shared_ptr<Tacky::ProgramNode> tacky_program = visitor.get_tacky_from_c_ast(program);

    // Basic checks on the Tacky AST structure
    ASSERT_NE(tacky_program, nullptr);

    auto functions = tacky_program->functions_;
    ASSERT_EQ(functions.size(), 1);
    ASSERT_EQ(functions[0]->name_, "main");
    // Further detailed checks can be added here to validate the structure of tacky_program
    auto instructions = functions[0]->instructions_;
    ASSERT_FALSE(instructions.empty());
}

} // namespace Codegen

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}