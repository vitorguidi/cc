#include "src/codegen/tacky_to_asm/tacky_to_asm.h"

namespace Codegen {

void PseudoReplacerVisitor::visit(ASM::PseudoNode& node) {
    if (stack_offsets_.count(node.name_) == 0) {
        current_offset_ -= 4;
        stack_offsets_[node.name_] = current_offset_;
    }
    buffer_.push_back(
        std::make_shared<ASM::StackNode>(stack_offsets_[node.name_])
    );
}

int PseudoReplacerVisitor::get_offset() {
    // this is consumed by the instruction fix up visitor
    // let's return a positive value for sanity
    if (current_offset_ > 0) {
        throw std::runtime_error("Offset should be <=0, it is " + std::to_string(current_offset_));
    }
    return -1*current_offset_;
}


} // namespace Codegen