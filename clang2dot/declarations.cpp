
#include "clang-to-dot.hpp"

#include <iostream>
#include <sstream>
#include <string>

#define SHORT_CUT_BUILTIN

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
        case clang::Decl::AccessSpec:
        case clang::Decl::Block:
        case clang::Decl::ClassScopeFunctionSpecialization:
        case clang::Decl::FileScopeAsm:
            assert(DEBUG_TODO == 0); // TODO
            break;
        case clang::Decl::Friend:
            ret_status = VisitFriendDecl((clang::FriendDecl *)decl, node_desc);
            break;
        case clang::Decl::FriendTemplate:
        case clang::Decl::LinkageSpec:
            assert(DEBUG_TODO == 0); // TODO
            break;
     // Named
        case clang::Decl::Label:
        case clang::Decl::NamespaceAlias:
            assert(DEBUG_TODO == 0); // TODO
            break;
        case clang::Decl::Namespace:
            ret_status = VisitNamespaceDecl((clang::NamespaceDecl *)decl, node_desc);
            break;
     // Objective C ... 
     // Template
     // RedeclarableTemplate
        case clang::Decl::ClassTemplate:
            ret_status = VisitClassTemplateDecl((clang::ClassTemplateDecl *)decl, node_desc);
            break;
        case clang::Decl::FunctionTemplate:
        case clang::Decl::TypeAliasTemplate:
        case clang::Decl::TemplateTemplateParm:
            assert(DEBUG_TODO == 0); // TODO
            break;
     // Type
     // Tag
        case clang::Decl::Enum:
            ret_status = VisitEnumDecl((clang::EnumDecl *)decl, node_desc);
            break;
        case clang::Decl::Record:
            ret_status = VisitRecordDecl((clang::RecordDecl *)decl, node_desc);
            break;
        case clang::Decl::CXXRecord:
            ret_status = VisitCXXRecordDecl((clang::CXXRecordDecl *)decl, node_desc);
            break;
        case clang::Decl::ClassTemplateSpecialization:
        case clang::Decl::ClassTemplatePartialSpecialization:
            assert(DEBUG_TODO == 0); // TODO
            break;
        case clang::Decl::TemplateTypeParm:
            ret_status = VisitTemplateTypeParmDecl((clang::TemplateTypeParmDecl *)decl, node_desc);
            break;
     // TypedefName
        case clang::Decl::TypeAlias:
            assert(DEBUG_TODO == 0); // TODO
            break;
        case clang::Decl::Typedef:
            ret_status = VisitTypedefDecl((clang::TypedefDecl *)decl, node_desc);
            break;
        case clang::Decl::UnresolvedUsingTypename:
        case clang::Decl::Using:
        case clang::Decl::UsingDirective:
        case clang::Decl::UsingShadow:
            assert(DEBUG_TODO == 0); // TODO
            break;
     // Value
     // Declarator
        case clang::Decl::Field:
            ret_status = VisitFieldDecl((clang::FieldDecl *)decl, node_desc);
            break;
     // Objective-C ....
        case clang::Decl::Function:
            ret_status = VisitFunctionDecl((clang::FunctionDecl *)decl, node_desc);
            break;
        case clang::Decl::CXXMethod:
            ret_status = VisitCXXMethodDecl((clang::CXXMethodDecl *)decl, node_desc);
            break;
        case clang::Decl::CXXConstructor:
            ret_status = VisitCXXConstructorDecl((clang::CXXConstructorDecl *)decl, node_desc);
            break;
        case clang::Decl::CXXConversion:
            ret_status = VisitCXXConversionDecl((clang::CXXConversionDecl *)decl, node_desc);
            break;
        case clang::Decl::CXXDestructor:
            ret_status = VisitCXXDestructorDecl((clang::CXXDestructorDecl *)decl, node_desc);
            break;
        case clang::Decl::NonTypeTemplateParm:
            ret_status = VisitNonTypeTemplateParmDecl((clang::NonTypeTemplateParmDecl *)decl, node_desc);
            break;
        case clang::Decl::Var:
            ret_status = VisitVarDecl((clang::VarDecl *)decl, node_desc);
            break;
        case clang::Decl::ImplicitParam:
            assert(DEBUG_TODO == 0); // TODO
            break;
        case clang::Decl::ParmVar:
            ret_status = VisitParmVarDecl((clang::ParmVarDecl *)decl, node_desc);
            break;
        case clang::Decl::EnumConstant:
            ret_status = VisitEnumConstantDecl((clang::EnumConstantDecl *)decl, node_desc);
            break;
        case clang::Decl::IndirectField:
        case clang::Decl::UnresolvedUsingValue:
     // Objective-C ...
        case clang::Decl::StaticAssert:
            assert(DEBUG_TODO == 0); // TODO
            break;
        case clang::Decl::TranslationUnit:
            ret_status = VisitTranslationUnitDecl((clang::TranslationUnitDecl *)decl, node_desc);
            break;
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

    switch (decl->getAccess()) {
        case clang::AS_public:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("access_specifier", "public"));
            break;
        case clang::AS_protected:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("access_specifier", "protected"));
            break;
        case clang::AS_private:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("access_specifier", "private`"));
            break;
        case clang::AS_none:
            break;
    }

    clang::Decl::attr_iterator it;
    unsigned cnt = 0;
    for (it = decl->attr_begin(); it != decl->attr_end(); it++) {
        std::ostringstream oss;
        oss << "attribute[" << cnt++ << "]";
        switch ((*it)->getKind()) {
#define ATTR(X)            case clang::attr::X: \
                node_desc.attributes.push_back(std::pair<std::string, std::string>(oss.str(), "X")); \
                break;
#include "clang/Basic/AttrList.inc"
        }
    }

    node_desc.successors.push_back(std::pair<std::string, std::string>("canonical_decl", Traverse(decl->getCanonicalDecl())));

    return true;
}

bool ClangToDot::VisitFriendDecl(clang::FriendDecl * friend_decl, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("FriendDecl");

    clang::NamedDecl * named_decl = friend_decl->getFriendDecl();
    clang::TypeSourceInfo * type_source_info = friend_decl->getFriendType();

    assert(named_decl == NULL xor type_source_info == NULL); // I think it is and only one: let see!

    if (named_decl != NULL) {
      node_desc.successors.push_back(std::pair<std::string, std::string>("friend_decl", Traverse(named_decl)));
    }

    if (type_source_info != NULL) {
      node_desc.successors.push_back(std::pair<std::string, std::string>("friend_type", Traverse(type_source_info->getType().getTypePtr())));
    }

    return VisitDecl(friend_decl, node_desc) && res;
}

bool ClangToDot::VisitNamedDecl(clang::NamedDecl * named_decl, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("NamedDecl");

    node_desc.attributes.push_back(std::pair<std::string, std::string>("name", named_decl->getNameAsString()));

    switch (named_decl->getLinkage()) {
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

    switch (named_decl->getVisibility()) {
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

    node_desc.successors.push_back(std::pair<std::string, std::string>("underlying_decl", Traverse(named_decl->getUnderlyingDecl())));

    return VisitDecl(named_decl, node_desc) && res;
}

bool ClangToDot::VisitNamespaceDecl(clang::NamespaceDecl * namespace_decl, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("NamespaceDecl");

    node_desc.successors.push_back(std::pair<std::string, std::string>("original_namespace", Traverse(namespace_decl->getOriginalNamespace())));

    node_desc.successors.push_back(std::pair<std::string, std::string>("next_namespace", Traverse(namespace_decl->getNextNamespace())));

    clang::DeclContext::decl_iterator it;
    unsigned cnt = 0;
    for (it = namespace_decl->decls_begin(); it != namespace_decl->decls_end(); it++) {
        std::ostringstream oss;
        oss << "child[" << cnt++ << "]";
        node_desc.successors.push_back(std::pair<std::string, std::string>(oss.str(), Traverse(*it)));
    }

    return VisitNamedDecl(namespace_decl, node_desc) && res;
}

bool ClangToDot::VisitTemplateDecl(clang::TemplateDecl * template_decl, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("TemplateDecl");

    clang::TemplateParameterList * template_parameters = template_decl->getTemplateParameters();
    assert(template_parameters != NULL);

    clang::TemplateParameterList::iterator it;
    unsigned cnt = 0;
    for (it = template_parameters->begin(); it != template_parameters->end(); it++) {
        std::ostringstream oss;
        oss << "template_parameter[" << cnt++ << "]";
        node_desc.successors.push_back(std::pair<std::string, std::string>(oss.str(), Traverse(*it)));
    }

    node_desc.successors.push_back(std::pair<std::string, std::string>("templated_decl", Traverse(template_decl->getTemplatedDecl())));

    return VisitNamedDecl(template_decl, node_desc) && res;
}

bool ClangToDot::VisitRedeclarableTemplateDecl(clang::RedeclarableTemplateDecl * redeclarable_template_decl, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("RedeclarableTemplateDecl");

    return VisitTemplateDecl(redeclarable_template_decl, node_desc) && res;
}

bool ClangToDot::VisitClassTemplateDecl(clang::ClassTemplateDecl * class_template_decl, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("ClassTemplateDecl");

    return VisitRedeclarableTemplateDecl(class_template_decl, node_desc) && res;
}

bool ClangToDot::VisitTypeDecl(clang::TypeDecl * type_decl, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("TypeDecl");

    node_desc.successors.push_back(std::pair<std::string, std::string>("type_for_decl", Traverse(type_decl->getTypeForDecl())));

    return VisitNamedDecl(type_decl, node_desc) && res;
}

bool ClangToDot::VisitTagDecl(clang::TagDecl * tag_decl, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("TagDecl");

    node_desc.successors.push_back(std::pair<std::string, std::string>("canonical_decl", Traverse(tag_decl->getCanonicalDecl())));

    node_desc.successors.push_back(std::pair<std::string, std::string>("definition", Traverse(tag_decl->getDefinition())));

    node_desc.attributes.push_back(std::pair<std::string, std::string>("kind", tag_decl->getKindName()));

    node_desc.successors.push_back(std::pair<std::string, std::string>("typedef_name_for_anon_decl", Traverse(tag_decl->getTypedefNameForAnonDecl())));

    // TODO NestedNameSpecifier * getQualifier () const 

    return VisitTypeDecl(tag_decl, node_desc) && res;
}

bool ClangToDot::VisitEnumDecl(clang::EnumDecl * enum_decl, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("EnumDecl");

    node_desc.successors.push_back(std::pair<std::string, std::string>("previous_declaration", Traverse(enum_decl->getPreviousDeclaration())));

    clang::EnumDecl::enumerator_iterator it;
    unsigned cnt = 0;
    for (it = enum_decl->enumerator_begin(); it != enum_decl->enumerator_end(); it++) {
        std::ostringstream oss;
        oss << "enumerator[" << cnt++ << "]";
        node_desc.successors.push_back(std::pair<std::string, std::string>(oss.str(), Traverse(*it)));
    }

    node_desc.successors.push_back(std::pair<std::string, std::string>("promotion_type", Traverse(enum_decl->getPromotionType().getTypePtr())));

    return VisitTagDecl(enum_decl, node_desc) && res;
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
    unsigned cnt = 0;
    for (it_base = cxx_record_decl->bases_begin(); it_base !=  cxx_record_decl->bases_end(); it_base++) {
        std::ostringstream oss;
        oss << "base_type[" << cnt++ << "]";
        switch (it_base->getAccessSpecifier()) {
            case clang::AS_public:
                oss << " (public)";
                break;
            case clang::AS_protected:
                oss << " (protected)";
                break;
            case clang::AS_private:
                oss << " (private)";
                break;
            case clang::AS_none:
                break;
        }
        node_desc.successors.push_back(std::pair<std::string, std::string>(oss.str(), Traverse(it_base->getType().getTypePtr())));
    }

    clang::CXXRecordDecl::base_class_iterator it_vbase;
    cnt = 0;
    for (it_vbase = cxx_record_decl->vbases_begin(); it_vbase !=  cxx_record_decl->vbases_end(); it_vbase++) {
        std::ostringstream oss;
        oss << "virtual_base_type[" << cnt++ << "]";
        switch (it_base->getAccessSpecifier()) {
            case clang::AS_public:
                oss << " (public)";
                break;
            case clang::AS_protected:
                oss << " (protected)";
                break;
            case clang::AS_private:
                oss << " (private)";
                break;
            case clang::AS_none:
                break;
        }
        node_desc.successors.push_back(std::pair<std::string, std::string>(oss.str(), Traverse(it_vbase->getType().getTypePtr())));
    }

    clang::CXXRecordDecl::method_iterator it_method;
    cnt = 0;
    for (it_method = cxx_record_decl->method_begin(); it_method !=  cxx_record_decl->method_end(); it_method++) {
        std::ostringstream oss;
        oss << "method[" << cnt++ << "]";
        node_desc.successors.push_back(std::pair<std::string, std::string>(oss.str(), Traverse(*it_method)));
    }

    clang::CXXRecordDecl::ctor_iterator it_ctor;
    cnt = 0;
    for (it_ctor = cxx_record_decl->ctor_begin(); it_ctor != cxx_record_decl->ctor_end(); it_ctor++) {
        std::ostringstream oss;
        oss << "constructor[" << cnt++ << "]";
        node_desc.successors.push_back(std::pair<std::string, std::string>(oss.str(), Traverse(*it_ctor)));
    }

    clang::CXXRecordDecl::friend_iterator it_friend;
    cnt = 0;
    for (it_friend = cxx_record_decl->friend_begin(); it_friend != cxx_record_decl->friend_end(); it_friend++) {
        std::ostringstream oss;
        oss << "friend[" << cnt++ << "]";
        node_desc.successors.push_back(std::pair<std::string, std::string>(oss.str(), Traverse(*it_friend)));
    }

    node_desc.successors.push_back(std::pair<std::string, std::string>("destructor", Traverse(cxx_record_decl->getDestructor())));

    return VisitRecordDecl(cxx_record_decl, node_desc) && res;
}

bool ClangToDot::VisitTemplateTypeParmDecl(clang::TemplateTypeParmDecl * template_type_parm_decl, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("TemplateTypeParmDecl");

    if (template_type_parm_decl->hasDefaultArgument())
        node_desc.successors.push_back(
            std::pair<std::string, std::string>("default_argument", Traverse(template_type_parm_decl->getDefaultArgument().getTypePtr()))
        );

    return VisitTypeDecl(template_type_parm_decl, node_desc) && res;
}

bool ClangToDot::VisitTypedefNameDecl(clang::TypedefNameDecl * typedef_name_decl, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("TypedefNameDecl");

    node_desc.successors.push_back(std::pair<std::string, std::string>("underlying_type", Traverse(typedef_name_decl->getUnderlyingType().getTypePtr())));

    return VisitTypeDecl(typedef_name_decl, node_desc) && res;
}

bool ClangToDot::VisitTypedefDecl(clang::TypedefDecl * typedef_decl, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("TypedefDecl");

    return VisitTypedefNameDecl(typedef_decl, node_desc) && res;
}

bool ClangToDot::VisitValueDecl(clang::ValueDecl * value_decl, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("ValueDecl");

     node_desc.successors.push_back(std::pair<std::string, std::string>("type", Traverse(value_decl->getType().getTypePtr())));

    return VisitNamedDecl(value_decl, node_desc) && res; 
}

bool ClangToDot::VisitDeclaratorDecl(clang::DeclaratorDecl * declarator_decl, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("DeclaratorDecl");

    return VisitValueDecl(declarator_decl, node_desc) && res; 
}

bool ClangToDot::VisitFieldDecl(clang::FieldDecl * field_decl, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;
    
    node_desc.kind_hierarchy.push_back("FieldDecl");

    node_desc.successors.push_back(std::pair<std::string, std::string>("in_class_initializer", Traverse(field_decl->getInClassInitializer())));

    node_desc.successors.push_back(std::pair<std::string, std::string>("bit_width", Traverse(field_decl->getBitWidth())));

    node_desc.successors.push_back(std::pair<std::string, std::string>("parent", Traverse(field_decl->getParent())));

    return VisitDeclaratorDecl(field_decl, node_desc) && res; 
}

bool ClangToDot::VisitFunctionDecl(clang::FunctionDecl * function_decl, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("FunctionDecl");

    node_desc.successors.push_back(std::pair<std::string, std::string>("previous_declaration", Traverse(function_decl->getPreviousDeclaration())));

    node_desc.successors.push_back(std::pair<std::string, std::string>("result_type", Traverse(function_decl->getResultType().getTypePtr())));

    for (unsigned i = 0; i < function_decl->getNumParams(); i++) {
        std::ostringstream oss;
        oss << "parameter[" << i << "]";
        node_desc.successors.push_back(std::pair<std::string, std::string>(oss.str(), Traverse(function_decl->getParamDecl(i))));
    }

    node_desc.successors.push_back(std::pair<std::string, std::string>("body", Traverse(function_decl->getBody())));

    return VisitDeclaratorDecl(function_decl, node_desc) && res;
}

bool ClangToDot::VisitCXXMethodDecl(clang::CXXMethodDecl * cxx_method_decl, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("CXXMethodDecl");

    // ...

    return VisitFunctionDecl(cxx_method_decl, node_desc) && res;
}

bool ClangToDot::VisitCXXConstructorDecl(clang::CXXConstructorDecl * cxx_constructor_decl, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("CXXConstructorDecl");

    // ...

    clang::CXXConstructorDecl::init_iterator it;
    unsigned cnt = 0;
    for (it = cxx_constructor_decl->init_begin(); it != cxx_constructor_decl->init_end(); it++) {
        std::ostringstream oss;
        oss << "init[" << cnt++ << "]";
//      node_desc.successors.push_back(std::pair<std::string, std::string>(oss.str(), Traverse(*it)));
        node_desc.attributes.push_back(std::pair<std::string, std::string>(oss.str(), ""));
    }

    return VisitCXXMethodDecl(cxx_constructor_decl, node_desc) && res;
}

bool ClangToDot::VisitCXXConversionDecl(clang::CXXConversionDecl * cxx_conversion_decl, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("CXXConversionDecl");

    // ...

    return VisitCXXMethodDecl(cxx_conversion_decl, node_desc) && res;
}

bool ClangToDot::VisitCXXDestructorDecl(clang::CXXDestructorDecl * cxx_destructor_decl, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("CXXDestructorDecl");

    // ...

    return VisitCXXMethodDecl(cxx_destructor_decl, node_desc) && res;
}

bool ClangToDot::VisitNonTypeTemplateParmDecl(clang::NonTypeTemplateParmDecl * non_type_template_param_decl, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("NonTypeTemplateParmDecl");

    if (non_type_template_param_decl->hasDefaultArgument())
        node_desc.successors.push_back(std::pair<std::string, std::string>("default_argument", Traverse(non_type_template_param_decl->getDefaultArgument())));

    return VisitDeclaratorDecl(non_type_template_param_decl, node_desc) && res;
}

bool ClangToDot::VisitVarDecl(clang::VarDecl * var_decl, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("VarDecl");

    node_desc.successors.push_back(std::pair<std::string, std::string>("acting_definition", Traverse(var_decl->getActingDefinition())));

    node_desc.successors.push_back(std::pair<std::string, std::string>("definition", Traverse(var_decl->getDefinition())));

    node_desc.successors.push_back(std::pair<std::string, std::string>("out_of_line_definition", Traverse(var_decl->getOutOfLineDefinition())));

    node_desc.successors.push_back(std::pair<std::string, std::string>("init", Traverse(var_decl->getInit())));

    return VisitDeclaratorDecl(var_decl, node_desc) && res;
}

bool ClangToDot::VisitParmVarDecl(clang::ParmVarDecl * param_var_decl, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("ParmVarDecl");

    node_desc.successors.push_back(std::pair<std::string, std::string>("original_type", Traverse(param_var_decl->getOriginalType().getTypePtr())));

    node_desc.successors.push_back(std::pair<std::string, std::string>("default_arg", Traverse(param_var_decl->getDefaultArg())));

    return VisitVarDecl(param_var_decl, node_desc) && res;
}

bool  ClangToDot::VisitEnumConstantDecl(clang::EnumConstantDecl * enum_constant_decl, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("EnumConstantDecl");

    node_desc.successors.push_back(std::pair<std::string, std::string>("init_expr", Traverse(enum_constant_decl->getInitExpr())));

    return VisitValueDecl(enum_constant_decl, node_desc) && res;
}

bool ClangToDot::VisitTranslationUnitDecl(clang::TranslationUnitDecl * translation_unit_decl, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("TranslationUnitDecl");

    clang::DeclContext::decl_iterator it;
    unsigned cnt = 0;
    for (it = translation_unit_decl->decls_begin(); it != translation_unit_decl->decls_end(); it++) {
        std::ostringstream oss;
        oss << "child[" << cnt++ << "]";
#ifdef SHORT_CUT_BUILTIN
        if (cnt < 6) continue;
#endif
        node_desc.successors.push_back(std::pair<std::string, std::string>(oss.str(), Traverse(*it)));
    }

    node_desc.successors.push_back(std::pair<std::string, std::string>("anonymous_namespace",  Traverse(translation_unit_decl->getAnonymousNamespace())));

    return VisitDecl(translation_unit_decl, node_desc) && res;
}
