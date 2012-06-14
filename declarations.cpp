
#include "clang-to-dot.hpp"

#include <iostream>

/*************************/
/* Traverse Declarations */
/*************************/

std::string ClangToDot::Traverse(clang::Decl * decl) {
    if (decl == NULL)
        return NULL;

    // Look for previous translation
    std::map<clang::Decl *, std::string>::iterator it = p_decl_translation_map.find(decl);
    if (it != p_decl_translation_map.end())
        return it->second;

    // If first time, create a new entry
    std::string node_ident = "";
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
    // TODO

    return true;
}

bool ClangToDot::VisitRecordDecl(clang::RecordDecl * record_decl, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    clang::RecordDecl * prev_record_decl = record_decl->getPreviousDeclaration();

  // Name

    record_decl->getNameAsString();

  // Type of class

    record_decl->getTagKind();

  // Build declaration(s)


    record_decl->isAnonymousStructOrUnion();

  // Build ClassDefinition

    clang::RecordDecl::field_iterator it;
    for (it = record_decl->field_begin(); it != record_decl->field_end(); it++) {
        // TODO
    }

    return VisitDecl(record_decl, node_desc) && res;
}

bool ClangToDot::VisitCXXRecordDecl(clang::CXXRecordDecl * cxx_record_decl, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    clang::RecordDecl * prev_record_decl = cxx_record_decl->getPreviousDeclaration();

    cxx_record_decl->getNameAsString();

    cxx_record_decl->getTagKind();

    cxx_record_decl->isAnonymousStructOrUnion();

    clang::RecordDecl::field_iterator it_field;
    for (it_field = cxx_record_decl->field_begin(); it_field != cxx_record_decl->field_end(); it_field++) {
        // TODO
    }

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

    return VisitDecl(cxx_record_decl, node_desc) && res;
}

bool ClangToDot::VisitEnumDecl(clang::EnumDecl * enum_decl, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    enum_decl->getNameAsString();

    clang::EnumDecl * prev_enum_decl = enum_decl->getPreviousDeclaration();

    clang::EnumDecl::enumerator_iterator it;
    for (it = enum_decl->enumerator_begin(); it != enum_decl->enumerator_end(); it++) {
        // TODO
    }

    return VisitDecl(enum_decl, node_desc) && res;
}

bool ClangToDot::VisitTypedefDecl(clang::TypedefDecl * typedef_decl, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    typedef_decl->getNameAsString();

    typedef_decl->getUnderlyingType();

    return VisitDecl(typedef_decl, node_desc) && res;
}

bool ClangToDot::VisitFieldDecl(clang::FieldDecl * field_decl, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;
    
    field_decl->getNameAsString();

    field_decl->getType();

    clang::Expr * init_expr = field_decl->getInClassInitializer();

    return VisitDecl(field_decl, node_desc) && res; 
}

bool ClangToDot::VisitFunctionDecl(clang::FunctionDecl * function_decl, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

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

    var_decl->getNameAsString();

    var_decl->getType();

    clang::Expr * init_expr = var_decl->getInit();

    return VisitDecl(var_decl, node_desc) && res;
}

bool ClangToDot::VisitParmVarDecl(clang::ParmVarDecl * param_var_decl, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    param_var_decl->getNameAsString();

    param_var_decl->getOriginalType();

    if (param_var_decl->hasDefaultArg()) {
        param_var_decl->getDefaultArg();
    }

    return VisitDecl(param_var_decl, node_desc) && res;
}

bool  ClangToDot::VisitEnumConstantDecl(clang::EnumConstantDecl * enum_constant_decl, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    enum_constant_decl->getNameAsString();

    enum_constant_decl->getType();

    if (enum_constant_decl->getInitExpr() != NULL) {
        enum_constant_decl->getInitExpr();
    }

    return VisitDecl(enum_constant_decl, node_desc) && res;
}

bool ClangToDot::VisitTranslationUnitDecl(clang::TranslationUnitDecl * translation_unit_decl, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    clang::DeclContext::decl_iterator it;
    for (it = translation_unit_decl->decls_begin(); it != translation_unit_decl->decls_end(); it++) {
        *it;
    }

    return VisitDecl(translation_unit_decl, node_desc) && res;
}
