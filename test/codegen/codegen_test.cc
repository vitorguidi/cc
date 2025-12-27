#include "src/ast/ast.h"
#include "src/parser/parser.h"
#include "src/lexer/lexer.h"
#include "src/codegen/c_ast_to_asm/c_ast_to_asm.h"
#include "gtest/gtest.h"

const std::string basic_program = 
    "int main() {"
    "   return 2;"
    "}";

namespace Codegen {

TEST(AstToAsmTest, BasicProgram) {
    std::unique_ptr<Lexer::Lexer> l = std::make_unique<Lexer::ManualLexer>(basic_program);
    std::unique_ptr<Parser::Parser> p = std::make_unique<Parser::RecursiveDescentParser>(std::move(l->Lex()));
    std::optional<std::shared_ptr<CAst::ProgramNode>> program_opt = p->parse();
    ASSERT_TRUE(program_opt.has_value());
    auto visitor = AstToAsmVisitor();
    visitor.visit(std::ref(*program_opt.value()));
    const std::vector<std::string>& assembly_output = visitor.get_assembly_output();
    std::vector<std::string> expected_output = {
        "\t.globl main",
        "main:",
        "\tmovl $2, %eax",
        "\tret",
        "\t.section .note.GNU-stack,\"\",@progbits"
    };
    ASSERT_EQ(assembly_output.size(), expected_output.size());
    for (size_t i = 0; i < assembly_output.size(); i++) {
        ASSERT_EQ(assembly_output[i], expected_output[i]);
    }
}

} // namespace Codegen

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}