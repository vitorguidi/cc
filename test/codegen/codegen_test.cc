#include "src/ast/ast.h"
#include "src/parser/parser.h"
#include "src/lexer/lexer.h"
#include "src/codegen/ast_to_asm_visitor.h"
#include "gtest/gtest.h"


const std::string basic_program = 
    "int main() {"
    "   return 2;"
    "}";

TEST(AstToAsmTest, BasicProgram) {
    std::unique_ptr<Lexer> l = std::make_unique<ManualLexer>(basic_program);
    std::unique_ptr<Parser> p = std::make_unique<RecursiveDescentParser>(std::move(l->Lex()));
    std::optional<ProgramNode> program_opt = p->parse();
    auto visitor = AstToAsmVisitor();
    visitor.visit(program_opt.value());
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

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}