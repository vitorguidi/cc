#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>

#include "src/ast/ast.h"
#include "src/graphviz/graphviz.h"
#include "src/codegen/ast_to_tacky_visitor.h"
#include "src/codegen/tacky_to_asm_visitor.h"
#include "src/lexer/lexer.h"
#include "src/parser/parser.h"

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << "Usage: cc <source_file> <output_asm_file>" << std::endl;
        return 1;
    }
    std::string source_file = argv[1];
    std::string output_asm_file = argv[2];
    std::ifstream input_file(source_file);
    if (!input_file.is_open()) {
        std::cerr << "Error: Could not open input file " << source_file << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << input_file.rdbuf();
    std::string source_code = buffer.str();

    auto lexer = std::make_unique<Lexer::ManualLexer>(source_code);
    auto tokens = lexer->Lex();

    // 3. Parsing
    auto parser = std::make_unique<Parser::RecursiveDescentParser>(std::move(tokens));
    auto program_node = parser->parse();

    if (!program_node.has_value()) {
        std::cerr << "Error: Parsing failed." << std::endl;
        return 1;
    }

    {
        Graphviz::GraphvizCAstVisitor c_ast_graphviz(std::string("cast.dot"));
        auto program_raw = *(program_node.value());
        c_ast_graphviz.visit(program_raw);
    }

    auto tacky_visitor = Codegen::AstToTackyVisitor();
    std::shared_ptr<Tacky::ProgramNode> tacky_program = tacky_visitor.get_tacky_from_c_ast(program_node.value());
    {
        Graphviz::GraphvizTackyVisitor tacky_graphviz(std::string("tacky.dot"));
        tacky_graphviz.visit(*tacky_program);
    }
    // auto asm_visitor = Codegen::TackyToAsmVisitor();
    // std::shared_ptr<ASM::ProgramNode> asm_program = asm_visitor.get_asm_from_tacky(tacky_program);


    return 0;
}