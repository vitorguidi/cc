#include "src/ast/ast.h"
#include "src/parser/parser.h"
#include "src/lexer/lexer.h"
#include "gtest/gtest.h"
#include "src/tacky/tacky.h"
#include "src/codegen/c_ast_to_tacky/c_ast_to_tacky.h"
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

    auto functions = tacky_program->functions_;
    ASSERT_EQ(functions.size(), 1);
    ASSERT_EQ(functions[0]->name_, "main");

    auto instructions = functions[0]->instructions_;

    ASSERT_EQ(instructions.size(), 4);

    auto op0 = std::dynamic_pointer_cast<Tacky::ComplementNode>(instructions[0]);
    ASSERT_NE(nullptr, op0);

    auto op0_src = std::dynamic_pointer_cast<Tacky::IntegerNode>(op0->src_);
    ASSERT_NE(nullptr, op0_src);
    ASSERT_EQ(op0_src->value_, 400);

    auto op0_dst = std::dynamic_pointer_cast<Tacky::VariableNode>(op0->dst_);
    ASSERT_NE(nullptr, op0_dst);
    ASSERT_EQ(op0_dst->name_, "_tacky_temp_0");
    
    auto op1 = std::dynamic_pointer_cast<Tacky::NegateNode>(instructions[1]);
    ASSERT_NE(nullptr, op1);

    auto op1_src = std::dynamic_pointer_cast<Tacky::VariableNode>(op1->src_);
    ASSERT_NE(nullptr, op1_src);
    ASSERT_EQ(op1_src->name_, "_tacky_temp_0");

    auto op1_dst = std::dynamic_pointer_cast<Tacky::VariableNode>(op1->dst_);
    ASSERT_NE(nullptr, op1_dst);
    ASSERT_EQ(op1_dst->name_, "_tacky_temp_1");

    auto op2 = std::dynamic_pointer_cast<Tacky::ComplementNode>(instructions[2]);
    ASSERT_NE(nullptr, op2);

    auto op2_src = std::dynamic_pointer_cast<Tacky::VariableNode>(op2->src_);
    ASSERT_NE(nullptr, op2_src);
    ASSERT_EQ(op2_src->name_, "_tacky_temp_1");

    auto op2_dst = std::dynamic_pointer_cast<Tacky::VariableNode>(op2->dst_);
    ASSERT_NE(nullptr, op2_dst);
    ASSERT_EQ(op2_dst->name_, "_tacky_temp_2");

    auto op3 = std::dynamic_pointer_cast<Tacky::ReturnNode>(instructions[3]);
    ASSERT_NE(nullptr, op3);

    auto op3_value = std::dynamic_pointer_cast<Tacky::VariableNode>(op3->value_);
    ASSERT_NE(nullptr, op3_value);
    ASSERT_EQ(op3_value->name_, "_tacky_temp_2");

    ASSERT_TRUE(visitor.result_buffer_.empty());

}

} // namespace Codegen

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}