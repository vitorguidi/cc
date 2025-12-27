#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>

#include "src/ast/ast.h"
#include "src/graphviz/graphviz.h"
#include "src/codegen/c_ast_to_tacky/c_ast_to_tacky.h"
#include "src/codegen/tacky_to_asm/tacky_to_asm.h"
#include "src/codegen/asm_dump/asm_dump.h"
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
    auto asm_visitor = Codegen::TackyToAsmVisitor();
    std::shared_ptr<ASM::ProgramNode> asm_program = asm_visitor.get_asm_from_tacky(tacky_program);

    {
        Graphviz::GraphvizASMVisitor asm_graphviz(std::string("asm_1st_pass.dot"));
        asm_graphviz.visit(*asm_program);
    }

    auto pseudo_replacement_visitor = Codegen::PseudoReplacerVisitor();
    auto no_pseudo_asm_program = pseudo_replacement_visitor.get_rewritten_asm_program(asm_program);

    {
        Graphviz::GraphvizASMVisitor asm_graphviz(std::string("asm_2nd_pass.dot"));
        asm_graphviz.visit(*no_pseudo_asm_program);
    }

    int max_offset = pseudo_replacement_visitor.get_offset();

    auto instruction_fixup_visitor = Codegen::InstructionFixUpVisitor(max_offset);
    auto fixed_asm_program = instruction_fixup_visitor.get_rewritten_asm_program(no_pseudo_asm_program);

    {
        Graphviz::GraphvizASMVisitor asm_graphviz(std::string("asm_3rd_pass.dot"));
        asm_graphviz.visit(*fixed_asm_program);
    }

    {
        auto asm_dump_visitor = Codegen::ASMDumper(std::string(output_asm_file));
        asm_dump_visitor.dump_assembly(fixed_asm_program);
    }

    return 0;
}