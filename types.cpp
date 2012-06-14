
#include "clang-to-dot.hpp"

#include <iostream>

/******************/
/* Traverse Types */
/******************/

std::string ClangToDot::Traverse(const clang::Type * type) {
    if (type == NULL)
        return NULL;

    // Look for previous translation
    std::map<const clang::Type *, std::string>::iterator it = p_type_translation_map.find(type);
    if (it != p_type_translation_map.end()) 
         return it->second;

    // If first time, create a new entry
    std::string node_ident = "";
    p_type_translation_map.insert(std::pair<const clang::Type *, std::string>(type, node_ident));
    NodeDescriptor & node_desc = p_node_desc.insert(std::pair<std::string, NodeDescriptor>(node_ident, NodeDescriptor(node_ident))).first->second;

    bool ret_status = false;

    switch (type->getTypeClass()) {
        case clang::Type::Builtin:
            ret_status = VisitBuiltinType((clang::BuiltinType *)type, node_desc);
            break;
        case clang::Type::Complex:
            ret_status = VisitComplexType((clang::ComplexType *)type, node_desc);
            break;
        case clang::Type::Pointer:
            ret_status = VisitPointerType((clang::PointerType *)type, node_desc);
            break;
        case clang::Type::ConstantArray:
            ret_status = VisitConstantArrayType((clang::ConstantArrayType *)type, node_desc);
            break;
       case clang::Type::IncompleteArray:
            ret_status = VisitIncompleteArrayType((clang::IncompleteArrayType *)type, node_desc);
            break;
        case clang::Type::Paren:
            ret_status = VisitParenType((clang::ParenType *)type, node_desc);
            break;
        case clang::Type::FunctionProto:
            ret_status = VisitFunctionProtoType((clang::FunctionProtoType *)type, node_desc);
            break;
        case clang::Type::FunctionNoProto:
            ret_status = VisitFunctionNoProtoType((clang::FunctionNoProtoType *)type, node_desc);
            break;
        case clang::Type::Elaborated:
            ret_status = VisitElaboratedType((clang::ElaboratedType *)type, node_desc);
            break;
        case clang::Type::Record:
            ret_status = VisitRecordType((clang::RecordType *)type, node_desc);
            break;
        case clang::Type::Enum:
            ret_status = VisitEnumType((clang::EnumType *)type, node_desc);
            break;
        case clang::Type::Typedef:
            ret_status = VisitTypedefType((clang::TypedefType *)type, node_desc);
            break;
        case clang::Type::ExtVector:
        case clang::Type::Vector:
            ret_status = VisitVectorType((clang::VectorType *)type, node_desc);
            break;
        // TODO cases
        default:
            std::cerr << "Unknown type kind " << type->getTypeClassName() << " !" << std::endl;
            assert(false);
    }

    assert(ret_status != false);

    return node_ident;
}

/***************/
/* Visit Types */
/***************/

bool ClangToDot::VisitType(clang::Type * type, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    return res;
}

bool ClangToDot::VisitArrayType(clang::ArrayType * array_type, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    array_type->getElementType();

    return VisitType(array_type, node_desc) && res;
}

bool ClangToDot::VisitConstantArrayType(clang::ConstantArrayType * constant_array_type, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    constant_array_type->getSize();

    return VisitArrayType(constant_array_type, node_desc) && res;
}

bool ClangToDot::VisitIncompleteArrayType(clang::IncompleteArrayType * incomplete_array_type, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    return VisitArrayType(incomplete_array_type, node_desc) && res;
}

bool ClangToDot::VisitAttributedType(clang::AttributedType * attributed_type, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    attributed_type->getModifiedType();

    switch (attributed_type->getAttrKind()) {
        case clang::AttributedType::attr_noreturn:
            break; 
        case clang::AttributedType::attr_cdecl: 
            break; 
        case clang::AttributedType::attr_stdcall: 
            break; 
        case clang::AttributedType::attr_address_space:
            break; 
        case clang::AttributedType::attr_regparm:
            break; 
        case clang::AttributedType::attr_vector_size:
            break; 
        case clang::AttributedType::attr_neon_vector_type:
            break; 
        case clang::AttributedType::attr_neon_polyvector_type:
            break; 
        case clang::AttributedType::attr_objc_gc:
            break; 
        case clang::AttributedType::attr_objc_ownership:
            break; 
        case clang::AttributedType::attr_pcs:
            break; 
        case clang::AttributedType::attr_fastcall:
            break; 
        case clang::AttributedType::attr_thiscall:
            break; 
        case clang::AttributedType::attr_pascal:
            break; 
    } 

    return VisitType(attributed_type, node_desc) && res;
}

bool ClangToDot::VisitBuiltinType(clang::BuiltinType * builtin_type, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    switch (builtin_type->getKind()) {
        case clang::BuiltinType::Void:
            break; 
        case clang::BuiltinType::Bool:
            break; 
        case clang::BuiltinType::Short:
            break; 
        case clang::BuiltinType::Int:
            break; 
        case clang::BuiltinType::Long:
            break; 
        case clang::BuiltinType::LongLong:
            break; 
        case clang::BuiltinType::Float:
            break; 
        case clang::BuiltinType::Double:
            break; 
        case clang::BuiltinType::LongDouble:
            break; 
        case clang::BuiltinType::Char_S:
            break; 
        case clang::BuiltinType::UInt:
            break; 
        case clang::BuiltinType::UChar:
            break; 
        case clang::BuiltinType::SChar:
            break; 
        case clang::BuiltinType::UShort:
            break; 
        case clang::BuiltinType::ULong:
            break; 
        case clang::BuiltinType::ULongLong:
            break; 
        case clang::BuiltinType::NullPtr:
            break; 
        case clang::BuiltinType::UInt128:
            break; 
        case clang::BuiltinType::Int128:
            break; 
        case clang::BuiltinType::Char_U:
            break; 
        case clang::BuiltinType::WChar_U:
            break; 
        case clang::BuiltinType::Char16:
            break; 
        case clang::BuiltinType::Char32:
            break; 
        case clang::BuiltinType::WChar_S:
            break; 
        case clang::BuiltinType::ObjCId:
            break; 
        case clang::BuiltinType::ObjCClass:
            break; 
        case clang::BuiltinType::ObjCSel:
            break; 
        case clang::BuiltinType::Dependent:
            break; 
        case clang::BuiltinType::Overload:
            break; 
        case clang::BuiltinType::BoundMember:
            break; 
        case clang::BuiltinType::UnknownAny:
            break; 
    }

    return VisitType(builtin_type, node_desc) && res;
}

bool ClangToDot::VisitComplexType(clang::ComplexType * complex_type, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    complex_type->getElementType();

    return VisitType(complex_type, node_desc) && res;
}

bool ClangToDot::VisitFunctionType(clang::FunctionType * function_type, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    function_type->getResultType();

    return VisitType(function_type, node_desc) && res;
}

bool ClangToDot::VisitFunctionNoProtoType(clang::FunctionNoProtoType * function_no_proto_type, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    return VisitFunctionType(function_no_proto_type, node_desc) && res;
}

bool ClangToDot::VisitFunctionProtoType(clang::FunctionProtoType * function_proto_type, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    for (unsigned i = 0; i < function_proto_type->getNumArgs(); i++) {
        function_proto_type->getArgType(i);
    }

    function_proto_type->isVariadic();

    return VisitFunctionType(function_proto_type, node_desc) && res;
}

bool ClangToDot::VisitParenType(clang::ParenType * paren_type, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    paren_type->getInnerType();

    return VisitType(paren_type, node_desc) && res;
}

bool ClangToDot::VisitPointerType(clang::PointerType * pointer_type, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    pointer_type->getPointeeType();

    return VisitType(pointer_type, node_desc) && res;
}

bool ClangToDot::VisitTagType(clang::TagType * tag_type, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    tag_type->getDecl();

    return VisitType(tag_type, node_desc) && res;
}

bool ClangToDot::VisitEnumType(clang::EnumType * enum_type, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    return VisitTagType(enum_type, node_desc) && res;
}

bool ClangToDot::VisitRecordType(clang::RecordType * record_type, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    return VisitTagType(record_type, node_desc) && res;
}

bool ClangToDot::VisitTypedefType(clang::TypedefType * typedef_type, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    typedef_type->getDecl();

    return VisitType(typedef_type, node_desc) && res;
}

bool ClangToDot::VisitElaboratedType(clang::ElaboratedType * elaborated_type, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    elaborated_type->getNamedType();

    return VisitType(elaborated_type, node_desc) && res;
}

bool ClangToDot::VisitVectorType(clang::VectorType * vector_type, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    vector_type->getElementType();

    vector_type->getNumElements();

    return VisitType(vector_type, node_desc) && res;
}

bool ClangToDot::VisitExtVectorType(clang::ExtVectorType * ext_vector_type, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    return VisitVectorType(ext_vector_type, node_desc) && res;
}

