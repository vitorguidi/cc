#include "gtest/gtest.h"
#include "src/parser/parser.h"
#include "src/lexer/lexer.h"

const std::string basic_program = 
    "int main() {"
    "   return 2;"
    "}";

// Demonstrate some basic assertions.
TEST(ParserTest, BasicProgram) {
    std::unique_ptr<Lexer> l = std::make_unique<ManualLexer>(basic_program);
    std::unique_ptr<Parser> p = std::make_unique<RecursiveDescentParser>(std::move(l->Lex()));
    std::optional<ProgramNode> program_opt = p->parse();
    ASSERT_TRUE(program_opt.has_value());
    ProgramNode program = program_opt.value();
    ASSERT_EQ(program.functions_.size(), 1);
    FunctionNode main_function = program.functions_[0];
    ASSERT_EQ(main_function.name_, "main");
    ASSERT_EQ(main_function.return_type_.return_type_, Type::INTEGER);
    ASSERT_EQ(main_function.arguments_.arguments_.size(), 0);
    ASSERT_EQ(main_function.body_.statements_.size(), 1);
    StatementNode stmt = main_function.body_.statements_[0];
    ASSERT_EQ(stmt.type_, StatementType::RETURN_STATEMENT);
    ASSERT_EQ(std::get<ReturnParameters>(stmt.parameters_).return_type_, Type::INTEGER);
    ASSERT_EQ(std::get<int>(std::get<ReturnParameters>(stmt.parameters_).return_value_), 2);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}