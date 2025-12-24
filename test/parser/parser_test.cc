#include "gtest/gtest.h"
#include "src/parser/parser.h"
#include "src/lexer/lexer.h"

const std::string basic_program = 
    "int main() {"
    "   return 2;"
    "}";

namespace Parser {

// Demonstrate some basic assertions.
TEST(ParserTest, BasicProgram) {
    std::unique_ptr<Lexer::Lexer> l = std::make_unique<Lexer::ManualLexer>(basic_program);
    std::unique_ptr<Parser> p = std::make_unique<RecursiveDescentParser>(std::move(l->Lex()));
    std::optional<std::shared_ptr<CAst::ProgramNode>> program_opt = p->parse();
    ASSERT_TRUE(program_opt.has_value());
    auto program = program_opt.value();
    ASSERT_EQ(program->functions_.size(), 1);
    std::shared_ptr<CAst::FunctionNode> main_function = program->functions_[0];
    ASSERT_EQ(main_function->name_, "main");
    ASSERT_EQ(main_function->type_node_->type_, Type::INTEGER);
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