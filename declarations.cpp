
#include "clang-to-dot.hpp"

#include <iostream>
#include <sstream>
#include <string>

/*************************/
/* Traverse Declarations */
/*************************/

std::string ClangToDot::Traverse(clang::Decl * decl) {
    if (decl == NULL)
        return "";

    // Look for previous translation
    std::map<clang::Decl *, std::string>::iterator it = p_decl_translation_map.find(decl);
    if (it != p_decl_translation_map.end())
        return it->second;

    // If first time, create a new entry
    std::string node_ident = genNextIdent();
    p_decl_translation_map.insert(std::pair<clang::Decl *, std::string>(decl, node_ident));
    NodeDescriptor & node_desc = p_node_desc.insert(std::pair<std::string, NodeDescriptor>(node_ident, NodeDescriptor(node_ident))).first->second;

    bool ret_status = false;

    switch (decl->getKind()) {
        case clang::Decl::TranslationUnit:
            ret_status = VisitTranslationUnitDecl((clang::TranslationUnitDecl *)decl, node_desc);
            break;
        case clang::Decl::Typedef:
            ret_status = VisitTypedefDecl((clang::TypedefDecl *)decl, node_desc);
            break;
        case clang::Decl::Var:
            ret_status = VisitVarDecl((clang::VarDecl *)decl, node_desc);
            break;
        case clang::Decl::Function:
            ret_status = VisitFunctionDecl((clang::FunctionDecl *)decl, node_desc);
            break;
        case clang::Decl::ParmVar:
            ret_status = VisitParmVarDecl((clang::ParmVarDecl *)decl, node_desc);
            break;
        case clang::Decl::CXXRecord:
          ret_status = VisitCXXRecordDecl((clang::CXXRecordDecl *)decl, node_desc);
          break;
        case clang::Decl::Record:
            ret_status = VisitRecordDecl((clang::RecordDecl *)decl, node_desc);
            break;
        case clang::Decl::Field:
            ret_status = VisitFieldDecl((clang::FieldDecl *)decl, node_desc);
            break;
        case clang::Decl::Enum:
            ret_status = VisitEnumDecl((clang::EnumDecl *)decl, node_desc);
            break;
        case clang::Decl::EnumConstant:
            ret_status = VisitEnumConstantDecl((clang::EnumConstantDecl *)decl, node_desc);
            break;
        // TODO cases
        default:
            std::cerr << "Unknown declacaration kind: " << decl->getDeclKindName() << " !" << std::endl;
            assert(false);
    }

    assert(ret_status != false);

    return node_ident;
}

/**********************/
/* Visit Declarations */
/**********************/

bool ClangToDot::VisitDecl(clang::Decl * decl, ClangToDot::NodeDescriptor & node_desc) {
    node_desc.kind_hierarchy.push_back("Decl");

    return true;
}

bool ClangToDot::VisitNamedDecl(clang::NamedDecl * named_decl, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("NamedDecl");

    node_desc.attributes.push_back(std::pair<std::string, std::string>("name", named_decl->getNameAsString()));

    return VisitDecl(named_decl, node_desc) && res;
}

bool ClangToDot::VisitTypeDecl(clang::TypeDecl * type_decl, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("TypeDecl");

    // TODO

    return VisitNamedDecl(type_decl, node_desc) && res;
}

bool ClangToDot::VisitTagDecl(clang::TagDecl * tag_decl, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("TagDecl");

    node_desc.attributes.push_back(std::pair<std::string, std::string>("kind", tag_decl->getKindName()));

    return VisitTypeDecl(tag_decl, node_desc) && res;
}

bool ClangToDot::VisitRecordDecl(clang::RecordDecl * record_decl, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("RecordDecl");

    node_desc.successors.push_back(std::pair<std::string, std::string>("previous_declaration", Traverse(record_decl->getPreviousDeclaration())));

    clang::RecordDecl::field_iterator it;
    unsigned cnt = 0;
    for (it = record_decl->field_begin(); it != record_decl->field_end(); it++) {
        std::ostringstream oss;
        oss << "field[" << cnt++ << "]";
        node_desc.successors.push_back(std::pair<std::string, std::string>(oss.str(), Traverse(*it)));
    }

    return VisitTagDecl(record_decl, node_desc) && res;
}

bool ClangToDot::VisitCXXRecordDecl(clang::CXXRecordDecl * cxx_record_decl, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("CXXRecordDecl");

    clang::CXXRecordDecl::base_class_iterator it_base;
    for (it_base = cxx_record_decl->bases_begin(); it_base !=  cxx_record_decl->bases_end(); it_base++) {
        // TODO
    }

    clang::CXXRecordDecl::base_class_iterator it_vbase;
    for (it_vbase = cxx_record_decl->vbases_begin(); it_vbase !=  cxx_record_decl->vbases_end(); it_vbase++) {
        // TODO
    }

    clang::CXXRecordDecl::method_iterator it_method;
    for (it_method = cxx_record_decl->method_begin(); it_method !=  cxx_record_decl->method_end(); it_method++) {
        // TODO
    }

    clang::CXXRecordDecl::ctor_iterator it_ctor;
    for (it_ctor = cxx_record_decl->ctor_begin(); it_ctor != cxx_record_decl->ctor_end(); it_ctor++) {
        // TODO if not tranversed as methods
    }

    clang::CXXRecordDecl::friend_iterator it_friend;
    for (it_friend = cxx_record_decl->friend_begin(); it_friend != cxx_record_decl->friend_end(); it_friend++) {
        // TODO
    }

    clang::CXXDestructorDecl * destructor = cxx_record_decl->getDestructor();

    return VisitRecordDecl(cxx_record_decl, node_desc) && res;
}

bool ClangToDot::VisitEnumDecl(clang::EnumDecl * enum_decl, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("EnumDecl");

    enum_decl->getNameAsString();

    clang::EnumDecl * prev_enum_decl = enum_decl->getPreviousDeclaration();

    clang::EnumDecl::enumerator_iterator it;
    for (it = enum_decl->enumerator_begin(); it != enum_decl->enumerator_end(); it++) {
        // TODO
    }

    return VisitTagDecl(enum_decl, node_desc) && res;
}

bool ClangToDot::VisitTypedefDecl(clang::TypedefDecl * typedef_decl, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("TypedefDecl");

    node_desc.attributes.push_back(std::pair<std::string, std::string>("name", typedef_decl->getNameAsString()));

    node_desc.successors.push_back(std::pair<std::string, std::string>("underlying_type", Traverse(typedef_decl->getUnderlyingType().getTypePtr())));

    return VisitDecl(typedef_decl, node_desc) && res;
}

bool ClangToDot::VisitValueDecl(clang::ValueDecl * value_decl, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("ValueDecl");

    value_decl->getType();

    return VisitNamedDecl(value_decl, node_desc) && res; 
}

bool ClangToDot::VisitDeclaratorDecl(clang::DeclaratorDecl * declarator_decl, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("DeclaratorDecl");

    // TODO

    return VisitValueDecl(declarator_decl, node_desc) && res; 
}

bool ClangToDot::VisitFieldDecl(clang::FieldDecl * field_decl, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;
    
    node_desc.kind_hierarchy.push_back("FieldDecl");

    field_decl->getType();

    clang::Expr * init_expr = field_decl->getInClassInitializer();

    return VisitDeclaratorDecl(field_decl, node_desc) && res; 
}

bool ClangToDot::VisitFunctionDecl(clang::FunctionDecl * function_decl, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("FunctionDecl");

    // TODO previous

    function_decl->getNameAsString();

    function_decl->getResultType();

    for (unsigned i = 0; i < function_decl->getNumParams(); i++) {
        function_decl->getParamDecl(i);
    }

    if (function_decl->isThisDeclarationADefinition()) {
        function_decl->getBody();
    }

    return VisitDecl(function_decl, node_desc) && res;
}

bool ClangToDot::VisitVarDecl(clang::VarDecl * var_decl, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("VarDecl");

    var_decl->getNameAsString();

    var_decl->getType();

    clang::Expr * init_expr = var_decl->getInit();

    return VisitDecl(var_decl, node_desc) && res;
}

bool ClangToDot::VisitParmVarDecl(clang::ParmVarDecl * param_var_decl, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("ParmVarDecl");

    param_var_decl->getNameAsString();

    param_var_decl->getOriginalType();

    if (param_var_decl->hasDefaultArg()) {
        param_var_decl->getDefaultArg();
    }

    return VisitDecl(param_var_decl, node_desc) && res;
}

bool  ClangToDot::VisitEnumConstantDecl(clang::EnumConstantDecl * enum_constant_decl, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("EnumConstantDecl");

    enum_constant_decl->getNameAsString();

    enum_constant_decl->getType();

    if (enum_constant_decl->getInitExpr() != NULL) {
        enum_constant_decl->getInitExpr();
    }

    return VisitDecl(enum_constant_decl, node_desc) && res;
}

bool ClangToDot::VisitTranslationUnitDecl(clang::TranslationUnitDecl * translation_unit_decl, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("TranslationUnitDecl");

    clang::DeclContext::decl_iterator it;
    unsigned cnt = 0;
    for (it = translation_unit_decl->decls_begin(); it != translation_unit_decl->decls_end(); it++) {
        std::ostringstream oss;
        oss << "child[" << cnt++ << "]";
        node_desc.successors.push_back(std::pair<std::string, std::string>(oss.str(), Traverse(*it)));
    }

    return VisitDecl(translation_unit_decl, node_desc) && res;
}
