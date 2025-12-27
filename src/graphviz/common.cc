#include "src/graphviz/graphviz.h"
#include <string>

namespace Graphviz {


std::string labeled_node_with_kv_pairs(std::string node_id, std::string node_name, const std::vector<NodeKVPair>& kv_pairs) {
    std::string result = "";
    result += "\tn";
    result += node_id;
    result += " [label=\"{";
    result += node_name + " ";

    for(auto [key, value] : kv_pairs) {
        result += " | { " + key +  " | " + value + " } ";
    }
    result += " }\"];\n";
    return result;
}

std::string labeled_edge(std::string parent_id, std::string child_id, std::string label) {
    return "\tn" + parent_id + " -> n" + child_id + "[label=\"" + label + "\"]\n";
}

} // namespace Graphviz
