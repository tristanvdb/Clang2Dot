
#include "clang-to-dot.hpp"

#include <iostream>
#include <sstream>
#include <string>

/******************/
/* Traverse Types */
/******************/

std::string ClangToDot::Traverse(const clang::Type * type) {
    if (type == NULL)
        return "";

    // Look for previous translation
    std::map<const clang::Type *, std::string>::iterator it = p_type_translation_map.find(type);
    if (it != p_type_translation_map.end()) 
         return it->second;

    // If first time, create a new entry
    std::string node_ident = genNextIdent();
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
        case clang::Type::LValueReference:
            ret_status = VisitLValueReferenceType((clang::LValueReferenceType *)type, node_desc);
            break;
        case clang::Type::RValueReference:
            ret_status = VisitRValueReferenceType((clang::RValueReferenceType *)type, node_desc);
            break;
        case clang::Type::ConstantArray:
            ret_status = VisitConstantArrayType((clang::ConstantArrayType *)type, node_desc);
            break;
        case clang::Type::DependentSizedArray:
            ret_status = VisitDependentSizedArrayType((clang::DependentSizedArrayType *)type, node_desc);
            break;
        case clang::Type::IncompleteArray:
            ret_status = VisitIncompleteArrayType((clang::IncompleteArrayType *)type, node_desc);
            break;
        case clang::Type::VariableArray:
            ret_status = VisitVariableArrayType((clang::VariableArrayType *)type, node_desc);
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

    node_desc.kind_hierarchy.push_back("Type");

    switch (type->getLinkage()) {
        case clang::NoLinkage:
            break;
        case clang::InternalLinkage:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("linkage", "internal"));
            break;
        case clang::UniqueExternalLinkage:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("linkage", "unique external"));
            break;
        case clang::ExternalLinkage:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("linkage", "external"));
            break;
    }

    switch (type->getVisibility()) {
        case clang::HiddenVisibility:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("visibility", "hidden"));
            break;
        case clang::ProtectedVisibility:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("visibility", "protected"));
            break;
        case clang::DefaultVisibility:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("visibility", "default"));
            break;
    }

    return res;
}

bool ClangToDot::VisitArrayType(clang::ArrayType * array_type, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("ArrayType");

    node_desc.successors.push_back(std::pair<std::string, std::string>("element_type", Traverse(array_type->getElementType().getTypePtr())));

    return VisitType(array_type, node_desc) && res;
}

bool ClangToDot::VisitConstantArrayType(clang::ConstantArrayType * constant_array_type, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("ConstantArrayType");

    node_desc.attributes.push_back(std::pair<std::string, std::string>("size", constant_array_type->getSize().toString(10, false)));

    return VisitArrayType(constant_array_type, node_desc) && res;
}

bool ClangToDot::VisitIncompleteArrayType(clang::IncompleteArrayType * incomplete_array_type, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("IncompleteArrayType");

    return VisitArrayType(incomplete_array_type, node_desc) && res;
}

bool ClangToDot::VisitDependentSizedArrayType(clang::DependentSizedArrayType * dependent_sized_array_type, NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.successors.push_back(std::pair<std::string, std::string>("size_expr", Traverse(dependent_sized_array_type->getSizeExpr())));

    return VisitArrayType(dependent_sized_array_type, node_desc) && res;
}

bool ClangToDot::VisitVariableArrayType(clang::VariableArrayType * variable_array_type, NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.successors.push_back(std::pair<std::string, std::string>("size_expr", Traverse(variable_array_type->getSizeExpr())));

    return VisitArrayType(variable_array_type, node_desc) && res;
}

bool ClangToDot::VisitAttributedType(clang::AttributedType * attributed_type, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("AttributedType");

    node_desc.successors.push_back(std::pair<std::string, std::string>("modified_type", Traverse(attributed_type->getModifiedType().getTypePtr())));

    node_desc.successors.push_back(std::pair<std::string, std::string>("equivalent_type", Traverse(attributed_type->getEquivalentType().getTypePtr())));

    switch (attributed_type->getAttrKind()) {
        case clang::AttributedType::attr_noreturn:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("attribut_kind", "noreturn"));
            break; 
        case clang::AttributedType::attr_cdecl: 
            node_desc.attributes.push_back(std::pair<std::string, std::string>("attribut_kind", "cdecl"));
            break; 
        case clang::AttributedType::attr_stdcall: 
            node_desc.attributes.push_back(std::pair<std::string, std::string>("attribut_kind", "stdcall"));
            break; 
        case clang::AttributedType::attr_address_space:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("attribut_kind", "address_space"));
            break; 
        case clang::AttributedType::attr_regparm:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("attribut_kind", "regparm"));
            break; 
        case clang::AttributedType::attr_vector_size:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("attribut_kind", "vector_size"));
            break; 
        case clang::AttributedType::attr_neon_vector_type:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("attribut_kind", "neon_vector_type"));
            break; 
        case clang::AttributedType::attr_neon_polyvector_type:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("attribut_kind", "neon_polyvector_type"));
            break; 
        case clang::AttributedType::attr_objc_gc:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("attribut_kind", "objc_gc"));
            break; 
        case clang::AttributedType::attr_objc_ownership:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("attribut_kind", "objc_ownership"));
            break; 
        case clang::AttributedType::attr_pcs:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("attribut_kind", "pcs"));
            break; 
        case clang::AttributedType::attr_fastcall:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("attribut_kind", "fastcall"));
            break; 
        case clang::AttributedType::attr_thiscall:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("attribut_kind", "thiscall"));
            break; 
        case clang::AttributedType::attr_pascal:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("attribut_kind", "pascal"));
            break; 
    } 

    return VisitType(attributed_type, node_desc) && res;
}

bool ClangToDot::VisitBuiltinType(clang::BuiltinType * builtin_type, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("BuiltinType");

    switch (builtin_type->getKind()) {
        case clang::BuiltinType::Void:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("type", "void"));
            break; 
        case clang::BuiltinType::Bool:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("type", "bool"));
            break; 
        case clang::BuiltinType::Short:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("type", "short"));
            break; 
        case clang::BuiltinType::Int:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("type", "int"));
            break; 
        case clang::BuiltinType::Long:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("type", "long"));
            break; 
        case clang::BuiltinType::LongLong:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("type", "long long"));
            break; 
        case clang::BuiltinType::Float:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("type", "float"));
            break; 
        case clang::BuiltinType::Double:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("type", "double"));
            break; 
        case clang::BuiltinType::LongDouble:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("type", "long double"));
            break; 
        case clang::BuiltinType::Char_S:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("type", "char_s"));
            break; 
        case clang::BuiltinType::UInt:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("type", "unsigned int"));
            break; 
        case clang::BuiltinType::UChar:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("type", "unsigned char"));
            break; 
        case clang::BuiltinType::SChar:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("type", "signed char"));
            break; 
        case clang::BuiltinType::UShort:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("type", "unsigned short"));
            break; 
        case clang::BuiltinType::ULong:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("type", "unsigned long"));
            break; 
        case clang::BuiltinType::ULongLong:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("type", "unsigned long long"));
            break; 
        case clang::BuiltinType::NullPtr:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("type", "null pointer"));
            break; 
        case clang::BuiltinType::UInt128:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("type", "uint_128"));
            break; 
        case clang::BuiltinType::Int128:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("type", "int_128"));
            break; 
        case clang::BuiltinType::Char_U:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("type", "char_u"));
            break; 
        case clang::BuiltinType::WChar_U:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("type", "wchar_u"));
            break; 
        case clang::BuiltinType::Char16:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("type", "char_16"));
            break; 
        case clang::BuiltinType::Char32:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("type", "char_32"));
            break; 
        case clang::BuiltinType::WChar_S:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("type", "wchar_s"));
            break; 
        case clang::BuiltinType::ObjCId:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("type", "ObjCId"));
            break; 
        case clang::BuiltinType::ObjCClass:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("type", "ObjCClass"));
            break; 
        case clang::BuiltinType::ObjCSel:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("type", "ObjCSel"));
            break; 
        case clang::BuiltinType::Dependent:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("type", "Dependent"));
            break; 
        case clang::BuiltinType::Overload:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("type", "Overload"));
            break; 
        case clang::BuiltinType::BoundMember:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("type", "BoundMember"));
            break; 
        case clang::BuiltinType::UnknownAny:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("type", "UnknownAny"));
            break; 
    }

    return VisitType(builtin_type, node_desc) && res;
}

bool ClangToDot::VisitComplexType(clang::ComplexType * complex_type, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("ComplexType");

    node_desc.successors.push_back(std::pair<std::string, std::string>("element_type", Traverse(complex_type->getElementType().getTypePtr())));

    return VisitType(complex_type, node_desc) && res;
}

bool ClangToDot::VisitFunctionType(clang::FunctionType * function_type, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("FunctionType");

    node_desc.successors.push_back(std::pair<std::string, std::string>("result_type", Traverse(function_type->getResultType().getTypePtr())));

    // TODO some attr

    return VisitType(function_type, node_desc) && res;
}

bool ClangToDot::VisitFunctionNoProtoType(clang::FunctionNoProtoType * function_no_proto_type, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("FunctionNoProtoType");

    return VisitFunctionType(function_no_proto_type, node_desc) && res;
}

bool ClangToDot::VisitFunctionProtoType(clang::FunctionProtoType * function_proto_type, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("FunctionProtoType");

    for (unsigned i = 0; i < function_proto_type->getNumArgs(); i++) {
        std::ostringstream oss;
        oss << "arg_type[" << i << "]";
        node_desc.successors.push_back(std::pair<std::string, std::string>(oss.str(), Traverse(function_proto_type->getArgType(i).getTypePtr())));
    }

    if (function_proto_type->isVariadic())
       node_desc.attributes.push_back(std::pair<std::string, std::string>("have", "variadic"));

    return VisitFunctionType(function_proto_type, node_desc) && res;
}

bool ClangToDot::VisitParenType(clang::ParenType * paren_type, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("ParenType");

    node_desc.successors.push_back(std::pair<std::string, std::string>("inner_type", Traverse(paren_type->getInnerType().getTypePtr())));

    return VisitType(paren_type, node_desc) && res;
}

bool ClangToDot::VisitPointerType(clang::PointerType * pointer_type, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("PointerType");

    node_desc.successors.push_back(std::pair<std::string, std::string>("pointee_type", Traverse(pointer_type->getPointeeType().getTypePtr())));

    return VisitType(pointer_type, node_desc) && res;
}

bool ClangToDot::VisitReferenceType(clang::ReferenceType * reference_type, NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("ReferenceType");

    node_desc.successors.push_back(std::pair<std::string, std::string>("pointee_type", Traverse(reference_type->getPointeeType().getTypePtr())));

    return VisitType(reference_type, node_desc) && res;
}

bool ClangToDot::VisitLValueReferenceType(clang::LValueReferenceType * lvalue_reference_type, NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("LValueReferenceType");

    return VisitType(lvalue_reference_type, node_desc) && res;
}

bool ClangToDot::VisitRValueReferenceType(clang::RValueReferenceType * rvalue_reference_type, NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("RValueReferenceType");

    return VisitType(rvalue_reference_type, node_desc) && res;
}

bool ClangToDot::VisitTagType(clang::TagType * tag_type, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("TagType");

    node_desc.successors.push_back(std::pair<std::string, std::string>("declaration", Traverse(tag_type->getDecl())));

    return VisitType(tag_type, node_desc) && res;
}

bool ClangToDot::VisitEnumType(clang::EnumType * enum_type, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("EnumType");

    return VisitTagType(enum_type, node_desc) && res;
}

bool ClangToDot::VisitRecordType(clang::RecordType * record_type, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("RecordType");

    return VisitTagType(record_type, node_desc) && res;
}

bool ClangToDot::VisitTypedefType(clang::TypedefType * typedef_type, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("TypedefType");

    node_desc.successors.push_back(std::pair<std::string, std::string>("declaration", Traverse(typedef_type->getDecl())));

    return VisitType(typedef_type, node_desc) && res;
}

bool ClangToDot::VisitTypeWithKeyword(clang::TypeWithKeyword * type_with_keyword, NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("TypeWithKeyword");

    switch (type_with_keyword->getKeyword()) {
        case clang::ETK_Struct:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("elaborated_type_keyword", "struct"));
            break;
        case clang::ETK_Union:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("elaborated_type_keyword", "union"));
            break;
        case clang::ETK_Class:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("elaborated_type_keyword", "class"));
            break;
        case clang::ETK_Enum:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("elaborated_type_keyword", "enum"));
            break;
        case clang::ETK_Typename:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("elaborated_type_keyword", "typename"));
            break;
        case clang::ETK_None:
            break;
    }

    return VisitType(type_with_keyword, node_desc) && res;
}

bool ClangToDot::VisitDependentNameType(clang::DependentNameType * dependent_name_type, NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("DependentNameType");

    // TODO

    return VisitTypeWithKeyword(dependent_name_type, node_desc) && res;
}

bool ClangToDot::VisitDependentTemplateSpecializationType(
    clang::DependentTemplateSpecializationType * dependent_template_specialization_type,
    NodeDescriptor & node_desc
) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("DependentTemplateSpecializationType");

    // TODO

    return VisitTypeWithKeyword(dependent_template_specialization_type, node_desc) && res;
}

bool ClangToDot::VisitElaboratedType(clang::ElaboratedType * elaborated_type, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("ElaboratedType");

    node_desc.successors.push_back(std::pair<std::string, std::string>("named_type", Traverse(elaborated_type->getNamedType().getTypePtr())));

    return VisitTypeWithKeyword(elaborated_type, node_desc) && res;
}

bool ClangToDot::VisitVectorType(clang::VectorType * vector_type, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("VectorType");

    node_desc.successors.push_back(std::pair<std::string, std::string>("element_type", Traverse(vector_type->getElementType().getTypePtr())));

    std::ostringstream oss;
    oss << vector_type->getNumElements();
    node_desc.attributes.push_back(std::pair<std::string, std::string>("number_element", oss.str()));

    return VisitType(vector_type, node_desc) && res;
}

bool ClangToDot::VisitExtVectorType(clang::ExtVectorType * ext_vector_type, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("ExtVectorType");

    return VisitVectorType(ext_vector_type, node_desc) && res;
}

