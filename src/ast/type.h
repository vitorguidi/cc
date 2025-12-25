#ifndef TYPE_H
#define TYPE_H

namespace CAst {

enum Type {
    INTEGER,
    VOID,
};

inline std::string type_as_str(Type type) {
    switch (type) {
        case (Type::INTEGER) :
            return std::string("int");
        case Type::VOID :
            return std::string("void");
    }
    return std::string("");
};

} // namespace CAst

#endif // TYPE_H