#include <string>
#include <random>
#include <iostream>

#include "src/ast/ast.h"
#include "src/codegen/c_ast_to_tacky/c_ast_to_tacky.h"
#include "src/codegen/tacky_to_asm/tacky_to_asm.h"
#include "src/codegen/asm_dump/asm_dump.h"
#include "src/lexer/lexer.h"
#include "src/parser/parser.h"

#include "gtest/gtest.h"


int SEED = 42, ITERATIONS = 100, MAX_HEIGHT = 7;

class RNG {
private:
    std::mt19937 mt_;

public:
    RNG(int seed) : mt_(seed) {}

    int draw(int lo, int hi) {
        std::uniform_int_distribution<int> dist(lo, hi);
        return dist(mt_);
    }
};

std::shared_ptr<CAst::ExpressionNode> rand_unexp(RNG& rng, int height);
std::shared_ptr<CAst::ExpressionNode> rand_binexp(RNG& rng, int height);
std::shared_ptr<CAst::ExpressionNode> rand_exp(RNG& rng, int height);

class CAstPrettyPrint : public CAst::Visitor {
private:
    std::string expr_;
public:
    ~CAstPrettyPrint() = default;
    CAstPrettyPrint() : expr_("") {}
    void visit(CAst::TypeNode& node) override {}
    void visit(CAst::FunctionArgumentsNode& node) override {}
    void visit(CAst::StatementBlockNode& node) override {}
    void visit(CAst::FunctionNode& node) override {}
    void visit(CAst::ProgramNode& node) override {}

    void visit(CAst::ReturnStatementNode& node) override {
        expr_ += "return ";
        node.return_value_->accept(*this);
        expr_ += ";";
    }
    void visit(CAst::TildeUnaryExpressionNode& node) override {
        expr_ += "~(";
        node.operand_->accept(*this);
        expr_ += ")";
    }
    void visit(CAst::MinusUnaryExpressionNode& node) override {
        expr_ += "-(";
        node.operand_->accept(*this);
        expr_ += ")";
    }
    void visit(CAst::IntegerValueNode& node) override {
        expr_ += std::to_string(node.value_);
    }
    void visit(CAst::DivNode& node) {
        node.left_->accept(*this);
        expr_ += "/";
        node.right_->accept(*this);
    }
    void visit(CAst::MultNode& node) {
        node.left_->accept(*this);
        expr_ += "*";
        node.right_->accept(*this);
    }
    void visit(CAst::ModNode& node) {
        node.left_->accept(*this);
        expr_ += "%";
        node.right_->accept(*this);
    }
    void visit(CAst::MinusNode& node) {
        expr_ += "(";
        node.left_->accept(*this);
        expr_ += ")";
        expr_ += "-";
        expr_ += "(";
        node.right_->accept(*this);
        expr_ += ")";

    }
    void visit(CAst::PlusNode& node) {
        node.left_->accept(*this);
        expr_ += "+";
        node.right_->accept(*this);
    }
    std::string get_return_string() {
        return expr_;
    }
};

const std::vector<std::string> bin_ops = {
    "MULT",
    "DIV",
    "MINUS",
    "PLUS",
    "MOD",
};

std::shared_ptr<CAst::ExpressionNode> rand_binexp(RNG& rng, int height) {
    if (height == 0) {
        int number = rng.draw(0, 200);
        return std::make_shared<CAst::IntegerValueNode>(number);
    }
    auto left = rand_exp(rng, height - 1);
    auto right = rand_exp(rng, height - 1);
    auto draw_idx = rng.draw(0, bin_ops.size()-1);
    auto draw_kind = bin_ops[draw_idx];

    if (draw_kind == "MULT") {
        return std::make_shared<CAst::MultNode>(left, right);
    } else if (draw_kind == "DIV") {
        return std::make_shared<CAst::DivNode>(left, right);
    } else if (draw_kind == "PLUS") {
        return std::make_shared<CAst::PlusNode>(left, right);
    } else if (draw_kind == "MINUS") {
        return std::make_shared<CAst::MinusNode>(left, right);
    } else if (draw_kind == "MOD") {
        return std::make_shared<CAst::ModNode>(left, right);
    } else {
        throw std::runtime_error("Unsupported binop: " + draw_kind);
    }
}

const std::vector<std::string> un_ops = {
    "MINUS",
    "TILDE",
};

std::shared_ptr<CAst::ExpressionNode> rand_unexp(RNG& rng, int height) {
    if (height == 0) {
        int number = rng.draw(0, 200);
        return std::make_shared<CAst::IntegerValueNode>(number);
    }
    auto operand = rand_exp(rng, height - 1);
    auto draw_idx = rng.draw(0, un_ops.size() - 1);
    auto draw_kind = un_ops[draw_idx];
    if (draw_kind == "MINUS") {
        return std::make_shared<CAst::MinusUnaryExpressionNode>(operand);
    } else if (draw_kind == "TILDE") {
        return std::make_shared<CAst::TildeUnaryExpressionNode>(operand);
    } else {
        throw std::runtime_error("Unsupported unop: " + draw_kind);
    }
}

std::shared_ptr<CAst::ExpressionNode> rand_exp(RNG& rng, int height) {
    auto draw = rng.draw(0, 2);
    if (draw%2 == 0) {
        return rand_binexp(rng, height);
    } else {
       return rand_unexp(rng, height);
    }
}

void dump_asm_to_file(std::string filename, std::string prog) {
        auto lex = Lexer::ManualLexer(prog);
        auto parser = Parser::RecursiveDescentParser(lex.Lex());
        auto CAst = parser.parse();
        auto tacky_visitor = Codegen::AstToTackyVisitor();
        std::shared_ptr<Tacky::ProgramNode> tacky_program = tacky_visitor.get_tacky_from_c_ast(CAst.value());
        auto asm_visitor = Codegen::TackyToAsmVisitor();
        std::shared_ptr<ASM::ProgramNode> asm_program = asm_visitor.get_asm_from_tacky(tacky_program);      
        auto pseudo_replacement_visitor = Codegen::PseudoReplacerVisitor();
        auto no_pseudo_asm_program = pseudo_replacement_visitor.get_rewritten_asm_program(asm_program);
        int max_offset = pseudo_replacement_visitor.get_offset();
        auto instruction_fixup_visitor = Codegen::InstructionFixUpVisitor(max_offset);
        auto fixed_asm_program = instruction_fixup_visitor.get_rewritten_asm_program(no_pseudo_asm_program);
        auto asm_dump_visitor = Codegen::ASMDumper(std::string(filename));
        asm_dump_visitor.dump_assembly(fixed_asm_program);
}

TEST(FuzzTest, ExpressionFuzzingTest) {

    auto seeded_rng = RNG(SEED);
    
    // Use TEST_TMPDIR provided by Bazel for scratch files
    const char* bazel_tmp = std::getenv("TEST_TMPDIR");
    std::string work_dir = bazel_tmp ? std::string(bazel_tmp) : ".";

    for(int i=0;i<ITERATIONS;i++) {
        int random_height = seeded_rng.draw(1, MAX_HEIGHT);
        auto expr = rand_exp(seeded_rng, random_height);
        auto pretty_printer = CAstPrettyPrint();
        auto random_ret = CAst::ReturnStatementNode(
            CAst::Type::INTEGER,
            expr
        );
        pretty_printer.visit(random_ret);
        std::string return_as_str = pretty_printer.get_return_string();
        std::cout << return_as_str << std::endl;
        std::string prog = "int main() {" + return_as_str + "}";


        std::string c_file = work_dir + "/fuzz.c";
        std::string s_file = work_dir + "/fuzz.s";
        std::string gcc_bin = work_dir + "/gcc_bin";
        std::string my_bin = work_dir + "/my_bin";

        // Write C source
        std::ofstream(c_file) << prog;
        dump_asm_to_file(s_file, prog);

        // 2. Reference Execution (GCC)
        std::string compile_cmd = "gcc " + c_file + " -o " + gcc_bin + " 2> " + work_dir + "/gcc_err.txt";
        std::system(compile_cmd.c_str());
        // Read the error file
        std::ifstream err_file(work_dir + "/gcc_err.txt");
        std::string err_content((std::istreambuf_iterator<char>(err_file)), std::istreambuf_iterator<char>());
        if (err_content.find("division by zero") != std::string::npos) {
            std::cout << "Skipping: GCC detected division by zero at compile-time." << std::endl;
            continue; // Skip iteration
        }
        // 4. Assemble your output and Run
        std::system(("gcc " + s_file + " -o " + my_bin).c_str());
        int my_exit_code = WEXITSTATUS(std::system(my_bin.c_str()));
        int gcc_exit_code = WEXITSTATUS(std::system(gcc_bin.c_str()));

        // 5. Compare
        EXPECT_EQ(gcc_exit_code, my_exit_code) << "Mismatch for: " << prog;

    }
}

int main(int argc, char **argv) {

    for (int i = 1; i < argc; i++) {
        std::string_view arg(argv[i]);
        
        if (arg.starts_with("--seed=")) {
            SEED = std::stoi(std::string(arg.substr(7)));
        } 
        else if (arg.starts_with("--iterations=")) {
            ITERATIONS = std::stoi(std::string(arg.substr(13)));
        } 
        else if (arg.starts_with("--max_height=")) {
            MAX_HEIGHT = std::stoi(std::string(arg.substr(13)));
        }
    }

    testing::InitGoogleTest(&argc, argv);


    return RUN_ALL_TESTS();
}