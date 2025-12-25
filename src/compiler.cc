#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>

#include "src/ast/ast.h"
#include "src/ast/ast_graphviz.h"
#include "src/codegen/ast_to_asm_visitor.h"
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
        CAst::GraphvizCAstVisitor graphviz(std::string("test_graph.dot"));
        auto program_raw = *(program_node.value());
        graphviz.visit(program_raw);
    }
    // 4. Code Generation (Visitor)
    auto visitor = Codegen::AstToAsmVisitor();
    visitor.visit(*program_node.value());
    const auto& assembly = visitor.get_assembly_output();

    // 5. Write to output file
    std::ofstream output_file(output_asm_file);
    if (!output_file.is_open()) {
        std::cerr << "Error: Could not open output file " << output_asm_file << std::endl;
        return 1;
    }

    for (const auto& line : assembly) {
        output_file << line << "\n";
    }

    return 0;
}