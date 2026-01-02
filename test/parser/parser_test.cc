#include "gtest/gtest.h"
#include "src/parser/parser.h"
#include "src/lexer/lexer.h"
#include <ranges>

const std::string basic_program = 
    "int main() {"
    "   return 2;"
    "}";

const std::string another_basic_program = 
    "int main() {\n"
    "   return ~(-(~400));\n"
    "}\n";

const std::string some_binexps = 
    "int main() {\n"
    "   return 2*7%5 + 10 - 11;\n"
    "}\n";

namespace Parser {

TEST(ParserTest, SomeBinexps) {
    std::unique_ptr<Lexer::Lexer> l = std::make_unique<Lexer::ManualLexer>(some_binexps);
    std::unique_ptr<Parser> p = std::make_unique<RecursiveDescentParser>(std::move(l->Lex()));
    std::optional<std::shared_ptr<CAst::ProgramNode>> program_opt = p->parse();

    // Assert has main function
    ASSERT_TRUE(program_opt.has_value());
    auto program = program_opt.value();
    ASSERT_EQ(program->functions_.size(), 1);
    std::shared_ptr<CAst::FunctionNode> main_function = program->functions_[0];
    ASSERT_EQ(main_function->name_, "main");

    // Assert int return type and no args
    ASSERT_EQ(main_function->type_node_->type_, CAst::Type::INTEGER);

    // Assert zero args
    ASSERT_EQ(main_function->arguments_node_->arguments_.size(), 0);

    // Assert one statement
    ASSERT_EQ(main_function->body_->statements_.size(), 1);

    // Assert return statement present
    auto return_stmt = std::dynamic_pointer_cast<CAst::ReturnStatementNode>(main_function->body_->statements_[0]);
    ASSERT_NE(return_stmt, nullptr);


    // Expected AST has the form
    //                  -
    //               +     11
    //            %   10
    //           *  5
    //          2 7
    // Assert return value is a - binexp, with right = 11

    auto return_value = std::dynamic_pointer_cast<CAst::MinusNode>(return_stmt->return_value_);
    ASSERT_NE(return_stmt, nullptr);
    auto expected_11 = std::dynamic_pointer_cast<CAst::IntegerValueNode>(return_value->right_);
    ASSERT_NE(expected_11, nullptr);
    ASSERT_EQ(expected_11->value_, 11);

    // Assert left operand is a X + 10 operation

    auto plus_op = std::dynamic_pointer_cast<CAst::PlusNode>(return_value->left_);
    ASSERT_NE(plus_op, nullptr);

    auto expected_10 = std::dynamic_pointer_cast<CAst::IntegerValueNode>(plus_op->right_);
    ASSERT_NE(expected_10, nullptr);
    ASSERT_EQ(expected_10->value_, 10);

    // Assert the mod Operation

    auto mod_op = std::dynamic_pointer_cast<CAst::ModNode>(plus_op->left_);
    ASSERT_NE(mod_op, nullptr);
    auto expected_5 = std::dynamic_pointer_cast<CAst::IntegerValueNode>(mod_op->right_);
    ASSERT_NE(expected_5, nullptr);
    ASSERT_EQ(expected_5->value_, 5);

    // Assert the mult operation

    auto mult_op = std::dynamic_pointer_cast<CAst::MultNode>(mod_op->left_);
    ASSERT_NE(mult_op, nullptr);
    auto expected_2 = std::dynamic_pointer_cast<CAst::IntegerValueNode>(mult_op->left_);
    ASSERT_NE(expected_2, nullptr);
    ASSERT_EQ(2, expected_2->value_);
    auto expected_7 = std::dynamic_pointer_cast<CAst::IntegerValueNode>(mult_op->right_);
    ASSERT_NE(expected_7, nullptr);
    ASSERT_EQ(7, expected_7->value_);
}

TEST(ParserTest, AnotherBasicProgram) {
    std::unique_ptr<Lexer::Lexer> l = std::make_unique<Lexer::ManualLexer>(another_basic_program);
    std::unique_ptr<Parser> p = std::make_unique<RecursiveDescentParser>(std::move(l->Lex()));
    std::optional<std::shared_ptr<CAst::ProgramNode>> program_opt = p->parse();

    // Assert has main function
    ASSERT_TRUE(program_opt.has_value());
    auto program = program_opt.value();
    ASSERT_EQ(program->functions_.size(), 1);
    std::shared_ptr<CAst::FunctionNode> main_function = program->functions_[0];
    ASSERT_EQ(main_function->name_, "main");

    // Assert int return type and no args
    ASSERT_EQ(main_function->type_node_->type_, CAst::Type::INTEGER);
    ASSERT_EQ(main_function->arguments_node_->arguments_.size(), 0);
    ASSERT_EQ(main_function->body_->statements_.size(), 1);

    // Assert return statement present
    auto return_stmt = std::dynamic_pointer_cast<CAst::ReturnStatementNode>(main_function->body_->statements_[0]);
    ASSERT_NE(return_stmt, nullptr);

    // Assert return val is a tilde unary op
    auto first_inner_op = std::dynamic_pointer_cast<CAst::TildeUnaryExpressionNode>(return_stmt->return_value_);
    ASSERT_NE(first_inner_op, nullptr);

    // Assert second inner exp is a minus unary op
    auto second_inner_op = std::dynamic_pointer_cast<CAst::MinusUnaryExpressionNode>(first_inner_op->operand_);
    ASSERT_NE(second_inner_op, nullptr);

    // Assert third inner exp is another tilde unary op
    auto third_inner_op = std::dynamic_pointer_cast<CAst::TildeUnaryExpressionNode>(second_inner_op->operand_);
    ASSERT_NE(third_inner_op, nullptr);

    // Assert final inner operand is 400
    auto inner_integer = std::dynamic_pointer_cast<CAst::IntegerValueNode>(third_inner_op->operand_);
    ASSERT_NE(inner_integer, nullptr);
    ASSERT_EQ(inner_integer->value_, 400);
}

TEST(ParserTest, BasicProgram) {
    std::unique_ptr<Lexer::Lexer> l = std::make_unique<Lexer::ManualLexer>(basic_program);
    std::unique_ptr<Parser> p = std::make_unique<RecursiveDescentParser>(std::move(l->Lex()));
    std::optional<std::shared_ptr<CAst::ProgramNode>> program_opt = p->parse();
    ASSERT_TRUE(program_opt.has_value());
    auto program = program_opt.value();
    ASSERT_EQ(program->functions_.size(), 1);
    std::shared_ptr<CAst::FunctionNode> main_function = program->functions_[0];
    ASSERT_EQ(main_function->name_, "main");
    ASSERT_EQ(main_function->type_node_->type_, CAst::Type::INTEGER);
    ASSERT_EQ(main_function->arguments_node_->arguments_.size(), 0);
    ASSERT_EQ(main_function->body_->statements_.size(), 1);
    auto return_stmt = std::dynamic_pointer_cast<CAst::ReturnStatementNode>(main_function->body_->statements_[0]);
    ASSERT_NE(return_stmt, nullptr);
    auto return_value = std::dynamic_pointer_cast<CAst::IntegerValueNode>(return_stmt->return_value_);
    ASSERT_NE(return_value, nullptr);
    ASSERT_EQ(return_value->value_, 2);
}

} // namespace Parser;

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}