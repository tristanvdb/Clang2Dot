
#ifndef _CLANG_TO_DOT_H_
# define _CLANG_TO_DOT_H_

#include "clang/AST/AST.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/Decl.h"
#include "clang/AST/DeclarationName.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/DeclFriend.h"
#include "clang/AST/DeclGroup.h"
#include "clang/AST/DeclObjC.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/AST/DeclVisitor.h"
#include "clang/AST/Expr.h"
#include "clang/AST/ExprCXX.h"
#include "clang/AST/ExprObjC.h"
#include "clang/AST/NestedNameSpecifier.h"
#include "clang/AST/ParentMap.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/Stmt.h"
#include "clang/AST/StmtCXX.h"
#include "clang/AST/StmtObjC.h"
#include "clang/AST/StmtVisitor.h"
#include "clang/AST/TemplateBase.h"
#include "clang/AST/TemplateName.h"
#include "clang/AST/Type.h"
#include "clang/AST/TypeLoc.h"
#include "clang/AST/TypeLocVisitor.h"

#include "clang/Basic/Builtins.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/IdentifierTable.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/LangOptions.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/TargetOptions.h"

#include "clang/FrontendTool/Utils.h"

#include "clang/Frontend/CompilerInvocation.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/DiagnosticOptions.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"
#include "clang/Frontend/TextDiagnosticBuffer.h"

#include "clang/Lex/HeaderSearch.h"
#include "clang/Lex/PPCallbacks.h"
#include "clang/Lex/Preprocessor.h"

#include "clang/Parse/ParseAST.h"

#include "clang/Sema/Sema.h"

#include "llvm/ADT/StringRef.h"

#include "llvm/Config/config.h"

#include "llvm/Support/Host.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/raw_os_ostream.h"

void clang2dot(
    std::vector<std::string> inc_dirs_list,
    std::vector<std::string> define_list,
    std::vector<std::string> inc_list,
    std::string input_file,
    std::ostream & out
);

class ClangToDot : public clang::ASTConsumer {
    public:
        enum Language {
            C,
            CPLUSPLUS,
            OBJC,
            CUDA,
            OPENCL,
            unknown
        };

        struct NodeDescriptor {
            NodeDescriptor(std::string ident_);

            void toDot(std::ostream & out) const;

            std::string ident;

            std::vector<std::string> kind_hierarchy;
            std::vector<std::pair<std::string, std::string> > successors;
            std::vector<std::pair<std::string, std::string> > attributes;
        };

    protected:
        clang::CompilerInstance  * p_compiler_instance;

        std::map<clang::Decl *, std::string> p_decl_translation_map;
        std::map<clang::Stmt *, std::string> p_stmt_translation_map;
        std::map<const clang::Type *, std::string> p_type_translation_map;

        std::map<std::string, NodeDescriptor> p_node_desc;

        Language language;

    public:
        ClangToDot(clang::CompilerInstance * compiler_instance, Language language_);

        virtual ~ClangToDot();

        void toDot(std::ostream & out) const;

  /* ASTConsumer's methods overload */

        virtual void HandleTranslationUnit(clang::ASTContext & ast_context);

  /* Traverse methods */

        virtual std::string Traverse(clang::Decl * decl);
        virtual std::string Traverse(clang::Stmt * stmt);
        virtual std::string Traverse(const clang::Type * type);
//        virtual void TraverseForDeclContext(clang::DeclContext * decl_context);

  /* Visit methods */

        virtual bool VisitDecl(clang::Decl * decl, ClangToDot::NodeDescriptor & node_desc);
    //      virtual bool VisitAccessSpecDecl(clang::AccessSpecDecl * access_spec_decl, ClangToDot::NodeDescriptor & node_desc);
    //      virtual bool VisitBlockDecl(clang::BlockDecl * block_decl, ClangToDot::NodeDescriptor & node_desc);
    //      virtual bool VisitFileScopeAsmDecl(clang::FileScopeAsmDecl * file_scope_asm_decl, ClangToDot::NodeDescriptor & node_desc);
    //      virtual bool VisitFriendDecl(clang::FriendDecl * friend_decl, ClangToDot::NodeDescriptor & node_desc);
    //      virtual bool VisitFriendTemplateDecl(clang::FriendTemplateDecl * friend_template_decl, ClangToDot::NodeDescriptor & node_desc);
    //      virtual bool VisitNamedDecl(clang::NamedDecl * named_decl, ClangToDot::NodeDescriptor & node_desc);
    //          virtual bool VisitLabelDecl(clang::LabelDecl * label_decl, ClangToDot::NodeDescriptor & node_desc);
    //          virtual bool VisitNamespaceDecl(clang::NamespaceDecl * namespace_decl, ClangToDot::NodeDescriptor & node_desc);
//              virtual bool VisitObjCCompatibleAliasDecl(clang::ObjCCompatibleAliasDecl * objc_compatible_alias_decl)
//              virtual bool VisitObjCContainerDecl(clang::ObjCContainerDecl * objc_container_decl, ClangToDot::NodeDescriptor & node_desc);
//                  virtual bool VisitObjCCategoryDecl(clang::ObjCCategoryDecl * objc_categoty_decl, ClangToDot::NodeDescriptor & node_desc);
//                  virtual bool VisitObjCImplDecl(clang::ObjCImplDecl * objc_impl_decl, ClangToDot::NodeDescriptor & node_desc);
//                      virtual bool VisitObjCCategoryImplDecl(clang::ObjCCategoryImplDecl * objc_category_impl_decl, ClangToDot::NodeDescriptor & node_desc);
//                      virtual bool VisitObjCImplementationDecl(clang::ObjCImplementationDecl * objc_implementation_decl, ClangToDot::NodeDescriptor & node_desc);
//                  virtual bool VisitObjCInterfaceDecl(clang::ObjCInterfaceDecl * objc_interface_decl, ClangToDot::NodeDescriptor & node_desc);
//                  virtual bool VisitObjCProtocolDecl(clang::ObjCProtocolDecl * objc_protocol_decl, ClangToDot::NodeDescriptor & node_desc);
//              virtual bool VisitObjCMethodDecl(clang::ObjCMethodDecl * objc_method_decl, ClangToDot::NodeDescriptor & node_desc);
//              virtual bool VisitObjCPropertyDecl(clang::ObjCPropertyDecl * objc_property_decl, ClangToDot::NodeDescriptor & node_desc);
    //          virtual bool VisitTemplateDecl(clang::TemplateDecl * template_decl, ClangToDot::NodeDescriptor & node_desc);
    //              virtual bool VisitRedeclarableTemplateDecl(clang::RedeclarableTemplateDecl * redeclarable_template_decl, ClangToDot::NodeDescriptor & node_desc);
    //                  virtual bool VisitClassTemplateDecl(clang::ClassTemplateDecl * class_template_decl, ClangToDot::NodeDescriptor & node_desc);
    //                  virtual bool VisitFunctionTemplateDecl(clang::FunctionTemplateDecl * function_template_decl, ClangToDot::NodeDescriptor & node_desc);
    //                  virtual bool VisitTypeAliasTemplateDecl(clang::TypeAliasTemplateDecl * type_alias_template_decl, ClangToDot::NodeDescriptor & node_desc);
    //              virtual bool VisitTemplateTemplateParmDecl(clang::TemplateTemplateParmDecl * template_template_parm_decl, ClangToDot::NodeDescriptor & node_desc);
    //          virtual bool VisitTypeDecl(clang::TypeDecl * type_decl, ClangToDot::NodeDescriptor & node_desc);
    //              virtual bool VisitTagDecl(clang::TagDecl * tag_decl, ClangToDot::NodeDescriptor & node_desc);
                        virtual bool VisitRecordDecl(clang::RecordDecl * record_decl, ClangToDot::NodeDescriptor & node_desc);
                            virtual bool VisitCXXRecordDecl(clang::CXXRecordDecl * cxx_record_decl, ClangToDot::NodeDescriptor & node_desc);
    //                          virtual bool VisitClassTemplateSpecializationDecl(clang::ClassTemplateSpecializationDecl * class_tpl_spec_decl, ClangToDot::NodeDescriptor & node_desc);
    //                              virtual bool VisitClassTemplatePartialSpecializationDecl(clang::ClassTemplatePartialSpecializationDecl * class_tpl_part_spec_decl, ClangToDot::NodeDescriptor & node_desc);
                        virtual bool VisitEnumDecl(clang::EnumDecl * enum_decl, ClangToDot::NodeDescriptor & node_desc); 
    //              virtual bool VisitTemplateTypeParmDecl(clang::TemplateTypeParmDecl * template_type_parm_decl, ClangToDot::NodeDescriptor & node_desc);
    //              virtual bool VisitTypedefNameDecl(clang::TypedefNameDecl * typedef_name_decl, ClangToDot::NodeDescriptor & node_desc);
                        virtual bool VisitTypedefDecl(clang::TypedefDecl * typedef_decl, ClangToDot::NodeDescriptor & node_desc);
    //                  virtual bool VisitTypeAliasDecl(clang::TypeAliasDecl * type_alias_decl, ClangToDot::NodeDescriptor & node_desc);
    //          virtual bool VisitValueDecl(clang::ValueDecl * value_decl, ClangToDot::NodeDescriptor & node_desc);
    //              virtual bool VisitDeclaratorDecl(clang::DeclaratorDecl * declarator_decl, ClangToDot::NodeDescriptor & node_desc);
                        virtual bool VisitFieldDecl(clang::FieldDecl * field_decl, ClangToDot::NodeDescriptor & node_desc);
//                          virtual bool VisitObjCAtDefsFieldDecl(clang::ObjCAtDefsFieldDecl * objc_at_defs_field_decl, ClangToDot::NodeDescriptor & node_desc);
//                          virtual bool VisitObjCIvarDecl(clang::ObjCIvarDecl * objc_ivar_decl, ClangToDot::NodeDescriptor & node_desc);
                        virtual bool VisitFunctionDecl(clang::FunctionDecl * function_decl, ClangToDot::NodeDescriptor & node_desc);
    //                      virtual bool VisitCXXMethodDecl(clang::CXXMethodDecl * cxx_method_decl, ClangToDot::NodeDescriptor & node_desc);
    //                          virtual bool VisitCXXConstructorDecl(clang::CXXConstructorDecl * cxx_constructor_decl, ClangToDot::NodeDescriptor & node_desc);
    //                          virtual bool VisitCXXConversionDecl(clang::CXXConversionDecl * cxx_conversion_decl, ClangToDot::NodeDescriptor & node_desc);
    //                          virtual bool VisitCXXDestructorDecl(clang::CXXDestructorDecl * cxx_destructor_decl, ClangToDot::NodeDescriptor & node_desc);
    //                  virtual bool VisitNonTypeTemplateParmDecl(clang::NonTypeTemplateParmDecl * non_type_template_param_decl, ClangToDot::NodeDescriptor & node_desc);
                        virtual bool VisitVarDecl(clang::VarDecl * var_decl, ClangToDot::NodeDescriptor & node_desc);
    //                      virtual bool VisitImplicitParamDecl(clang::ImplicitParamDecl * implicit_param_decl, ClangToDot::NodeDescriptor & node_desc);
                            virtual bool VisitParmVarDecl(clang::ParmVarDecl * param_var_decl, ClangToDot::NodeDescriptor & node_desc);
                    virtual bool VisitEnumConstantDecl(clang::EnumConstantDecl * enum_constant_decl, ClangToDot::NodeDescriptor & node_desc);
    //              virtual bool VisitIndirectFieldDecl(clang::IndirectFieldDecl * indirect_field_decl, ClangToDot::NodeDescriptor & node_desc);
            virtual bool VisitTranslationUnitDecl(clang::TranslationUnitDecl * translation_unit_decl, ClangToDot::NodeDescriptor & node_desc);

        virtual bool VisitStmt(clang::Stmt * stmt, ClangToDot::NodeDescriptor & node_desc);
    //      virtual bool VisitAsmStmt(clang::AsmStmt * asm_stmt, ClangToDot::NodeDescriptor & node_desc);
            virtual bool VisitBreakStmt(clang::BreakStmt * break_stmt, ClangToDot::NodeDescriptor & node_desc);
            virtual bool VisitCompoundStmt(clang::CompoundStmt * compound_stmt, ClangToDot::NodeDescriptor & node_desc);
            virtual bool VisitContinueStmt(clang::ContinueStmt * continue_stmt, ClangToDot::NodeDescriptor & node_desc);
    //      virtual bool VisitCXXCatchStmt(clang::CXXCatchStmt * cxx_catch_stmt, ClangToDot::NodeDescriptor & node_desc);
    //      virtual bool VisitCXXForRangeStmt(clang::CXXForRangeStmt * cxx_for_range_stmt, ClangToDot::NodeDescriptor & node_desc);
    //      virtual bool VisitCXXTryStmt(clang::CXXTryStmt * cxx_try_stmt, ClangToDot::NodeDescriptor & node_desc);
            virtual bool VisitDeclStmt(clang::DeclStmt * decl_stmt, ClangToDot::NodeDescriptor & node_desc);
            virtual bool VisitDoStmt(clang::DoStmt * do_stmt, ClangToDot::NodeDescriptor & node_desc);
            virtual bool VisitExpr(clang::Expr * expr, ClangToDot::NodeDescriptor & node_desc);
    //          virtual bool VisitAbstractConditionalOperator(clang::AbstractConditionalOperator * abstract_conditional_operator, ClangToDot::NodeDescriptor & node_desc);
    //              virtual bool VisitBinaryConditionalOperator(clang::BinaryConditionalOperator * binary_conditionnal_operator, ClangToDot::NodeDescriptor & node_desc);
                    virtual bool VisitConditionalOperator(clang::ConditionalOperator * conditional_operator, ClangToDot::NodeDescriptor & node_desc);
    //          virtual bool VisitAddrLabelExpr(clang::AddrLabelExpr * addr_label_expr, ClangToDot::NodeDescriptor & node_desc);
                virtual bool VisitArraySubscriptExpr(clang::ArraySubscriptExpr * array_subscript_expr, ClangToDot::NodeDescriptor & node_desc);
    //          virtual bool VisitArrayTypeTraitExpr(clang::ArrayTypeTraitExpr * array_type_trait_expr, ClangToDot::NodeDescriptor & node_desc);
    //          virtual bool VisitAsTypeExpr(clang::AsTypeExpr * as_type_expr, ClangToDot::NodeDescriptor & node_desc);
                virtual bool VisitBinaryOperator(clang::BinaryOperator * binary_operator, ClangToDot::NodeDescriptor & node_desc);
    //              virtual bool VisitCompoundAssignOperator(clang::CompoundAssignOperator * compound_assign_operator, ClangToDot::NodeDescriptor & node_desc);
    //          virtual bool VisitBinaryTypeTraitExpr(clang::BinaryTypeTraitExpr * binary_type_trait_expr, ClangToDot::NodeDescriptor & node_desc);
    //          virtual bool VisitBlockDeclRefExpr(clang::BlockDeclRefExpr * block_decl_ref_expr, ClangToDot::NodeDescriptor & node_desc);
    //          virtual bool VisitBlockExpr(clang::BlockExpr * block_expr, ClangToDot::NodeDescriptor & node_desc);
                virtual bool VisitCallExpr(clang::CallExpr * call_expr, ClangToDot::NodeDescriptor & node_desc);
    //              virtual bool VisitCUDAKernelCallExpr(clang::CUDAKernelCallExpr * cuda_kernel_call_expr, ClangToDot::NodeDescriptor & node_desc);
    //              virtual bool VisitCXXMemberCallExpr(clang::CXXMemberCallExpr * cxx_member_call_expr, ClangToDot::NodeDescriptor & node_desc);
    //              virtual bool VisitCXXOperatorCallExpr(clang::CXXOperatorCallExpr * cxx_operator_call_expr, ClangToDot::NodeDescriptor & node_desc);
                virtual bool VisitCastExpr(clang::CastExpr * cast_expr, ClangToDot::NodeDescriptor & node_desc);
                    virtual bool VisitExplicitCastExpr(clang::ExplicitCastExpr * explicit_cast_expr, ClangToDot::NodeDescriptor & node_desc);
                        virtual bool VisitCStyleCastExpr(clang::CStyleCastExpr * c_style_cast, ClangToDot::NodeDescriptor & node_desc);
    //                  virtual bool VisitCXXFunctionalCastExpr(clang::CXXFunctionalCastExpr * cxx_functional_cast_expr, ClangToDot::NodeDescriptor & node_desc);
    //                  virtual bool VisitCXXNamedCastExpr(clang::CXXNamedCastExpr * cxx_named_cast_expr, ClangToDot::NodeDescriptor & node_desc);
    //                      virtual bool VisitCXXConstCastExpr(clang::CXXConstCastExpr * cxx_const_cast_expr, ClangToDot::NodeDescriptor & node_desc);
    //                      virtual bool VisitCXXDynamicCastExpr(clang::CXXDynamicCastExpr * cxx_dynamic_cast_expr, ClangToDot::NodeDescriptor & node_desc);
    //                      virtual bool VisitCXXReinterpretCastExpr(clang::CXXReinterpretCastExpr * cxx_reinterpret_cast_expr, ClangToDot::NodeDescriptor & node_desc);
    //                      virtual bool VisitCXXStaticCastExpr(clang::CXXStaticCastExpr * cxx_static_cast_expr, ClangToDot::NodeDescriptor & node_desc);
                    virtual bool VisitImplicitCastExpr(clang::ImplicitCastExpr * implicit_cast_expr, ClangToDot::NodeDescriptor & node_desc);
                virtual bool VisitCharacterLiteral(clang::CharacterLiteral * character_literal, ClangToDot::NodeDescriptor & node_desc);
    //          virtual bool VisitChooseExpr(clang::ChooseExpr * choose_expr, ClangToDot::NodeDescriptor & node_desc);
                virtual bool VisitCompoundLiteralExpr(clang::CompoundLiteralExpr * compound_literal, ClangToDot::NodeDescriptor & node_desc);
    //          virtual bool VisitCXXBindTemporaryExpr(clang::CXXBindTemporaryExpr * cxx_bind_temporary_expr, ClangToDot::NodeDescriptor & node_desc);
    //          virtual bool VisitCXXBoolLiteralExpr(clang::CXXBoolLiteralExpr * cxx_bool_literal_expr, ClangToDot::NodeDescriptor & node_desc);
    //          virtual bool VisitCXXConstructExpr(clang::CXXConstructExpr * cxx_constructor_expr, ClangToDot::NodeDescriptor & node_desc);
    //          virtual bool VisitCXXTemporaryObjectExpr(clang::CXXTemporaryObjectExpr * cxx_temporary_object_expr, ClangToDot::NodeDescriptor & node_desc);
    //          virtual bool VisitCXXDefaultArgExpr(clang::CXXDefaultArgExpr * cxx_default_arg_expr, ClangToDot::NodeDescriptor & node_desc);
    //          virtual bool VisitCXXDeleteExpr(clang::CXXDeleteExpr * cxx_delete_expr, ClangToDot::NodeDescriptor & node_desc);
    //          virtual bool VisitCXXDependentScopeMemberExpr(clang::CXXDependentScopeMemberExpr * cxx_dependent_scope_member_expr, ClangToDot::NodeDescriptor & node_desc);
    //          virtual bool VisitCXXNewExpr(clang::CXXNewExpr * cxx_new_expr, ClangToDot::NodeDescriptor & node_desc);
    //          virtual bool VisitCXXNoexceptExpr(clang::CXXNoexceptExpr * cxx_noexecept_expr, ClangToDot::NodeDescriptor & node_desc);
    //          virtual bool VisitCXXNullPtrLiteralExpr(clang::CXXNullPtrLiteralExpr * cxx_null_ptr_literal_expr, ClangToDot::NodeDescriptor & node_desc);
    //          virtual bool VisitCXXPseudoDestructorExpr(clang::CXXPseudoDestructorExpr * cxx_pseudo_destructor_expr, ClangToDot::NodeDescriptor & node_desc);
    //          virtual bool VisitCXXScalarValueInitExpr(clang::CXXScalarValueInitExpr * cxx_scalar_value_init_expr, ClangToDot::NodeDescriptor & node_desc);
    //          virtual bool VisitCXXThisExpr(clang::CXXThisExpr * cxx_this_expr, ClangToDot::NodeDescriptor & node_desc);
    //          virtual bool VisitCXXThrowExpr(clang::CXXThrowExpr * cxx_throw_expr, ClangToDot::NodeDescriptor & node_desc);
    //          virtual bool VisitCXXTypeidExpr(clang::CXXTypeidExpr * cxx_typeid_expr, ClangToDot::NodeDescriptor & node_desc);
    //          virtual bool VisitCXXUnresolvedConstructExpr(clang::CXXUnresolvedConstructExpr * cxx_unresolved_construct_expr, ClangToDot::NodeDescriptor & node_desc);
    //          virtual bool VisitCXXUuidofExpr(clang::CXXUuidofExpr * cxx_uuidof_expr, ClangToDot::NodeDescriptor & node_desc);
                virtual bool VisitDeclRefExpr(clang::DeclRefExpr * decl_ref_expr, ClangToDot::NodeDescriptor & node_desc);
    //          virtual bool VisitDependentScopeDeclRefExpr(clang::DependentScopeDeclRefExpr * dependent_scope_decl_ref_expr, ClangToDot::NodeDescriptor & node_desc);
                virtual bool VisitDesignatedInitExpr(clang::DesignatedInitExpr * designated_init_expr, ClangToDot::NodeDescriptor & node_desc);
    //          virtual bool VisitExpressionTraitExpr(clang::ExpressionTraitExpr * expression_trait_expr, ClangToDot::NodeDescriptor & node_desc);
    //          virtual bool VisitExprWithCleanups(clang::ExprWithCleanups * expr_with_cleanups);
                        virtual bool VisitExtVectorElementExpr(clang::ExtVectorElementExpr * ext_vector_element_expr, ClangToDot::NodeDescriptor & node_desc);
                        virtual bool VisitFloatingLiteral(clang::FloatingLiteral * floating_literal, ClangToDot::NodeDescriptor & node_desc);
            //          virtual bool VisitGenericSelectionExpr(clang::GenericSelectionExpr * generic_selection_expr, ClangToDot::NodeDescriptor & node_desc);
            //          virtual bool VisitGNUNullExpr(clang::GNUNullExpr * gnu_null_expr, ClangToDot::NodeDescriptor & node_desc);
                        virtual bool VisitImaginaryLiteral(clang::ImaginaryLiteral * imaginary_literal, ClangToDot::NodeDescriptor & node_desc);
            //          virtual bool VisitImplicitValueInitExpr(clang::ImplicitValueInitExpr * implicit_value_init_expr, ClangToDot::NodeDescriptor & node_desc);
                        virtual bool VisitInitListExpr(clang::InitListExpr * init_list_expr, ClangToDot::NodeDescriptor & node_desc);
                        virtual bool VisitIntegerLiteral(clang::IntegerLiteral * integer_literal, ClangToDot::NodeDescriptor & node_desc);
                        virtual bool VisitMemberExpr(clang::MemberExpr * member_expr, ClangToDot::NodeDescriptor & node_desc);
        //              virtual bool VisitObjCEncodeExpr(clang::ObjCEncodeExpr * objc_encoder_expr, ClangToDot::NodeDescriptor & node_desc);
        //              virtual bool VisitObjCIsaExpr(clang::ObjCIsaExpr * objc_isa_expr, ClangToDot::NodeDescriptor & node_desc);
        //              virtual bool VisitObjCIvarRefExpr(clang::ObjCIvarRefExpr * objc_ivar_ref_expr, ClangToDot::NodeDescriptor & node_desc);
        //              virtual bool VisitObjCMessageExpr(clang::ObjCMessageExpr * objc_message_expr, ClangToDot::NodeDescriptor & node_desc);
        //              virtual bool VisitObjCPropertyRefExpr(clang::ObjCPropertyRefExpr * objc_propery_ref_expr, ClangToDot::NodeDescriptor & node_desc);
        //              virtual bool VisitObjCProtocolExpr(clang::ObjCProtocolExpr * objc_protocol_expr, ClangToDot::NodeDescriptor & node_desc);
        //              virtual bool VisitObjCSelectorExpr(clang::ObjCSelectorExpr * objc_selector_expr, ClangToDot::NodeDescriptor & node_desc);
        //              virtual bool VisitObjCStringLiteral(clang::ObjCStringLiteral * objc_string_literal, ClangToDot::NodeDescriptor & node_desc);
            //          virtual bool VisitOffsetOfExpr(clang::OffsetOfExpr * offset_of_expr, ClangToDot::NodeDescriptor & node_desc);
            //          virtual bool VisitOpaqueValueExpr(clang::OpaqueValueExpr * opaque_value_expr, ClangToDot::NodeDescriptor & node_desc);
            //          virtual bool VisitOverloadExpr(clang::OverloadExpr * overload_expr, ClangToDot::NodeDescriptor & node_desc);
            //              virtual bool VisitUnresolvedLookupExpr(clang::UnresolvedLookupExpr * unresolved_lookup_expr, ClangToDot::NodeDescriptor & node_desc);
            //              virtual bool VisitUnresolvedMemberExpr(clang::UnresolvedMemberExpr * unresolved_member_expr, ClangToDot::NodeDescriptor & node_desc);
            //          virtual bool VisitPackExpansionExpr(clang::PackExpansionExpr * pack_expansion_expr, ClangToDot::NodeDescriptor & node_desc);
                        virtual bool VisitParenExpr(clang::ParenExpr * paren_expr, ClangToDot::NodeDescriptor & node_desc);
            //          virtual bool VisitParenListExpr(clang::ParenListExpr * paren_list_expr, ClangToDot::NodeDescriptor & node_desc);
                        virtual bool VisitPredefinedExpr(clang::PredefinedExpr * predefined_expr, ClangToDot::NodeDescriptor & node_desc);
            //          virtual bool VisitShuffleVectorExpr(clang::ShuffleVectorExpr * shuffle_vector_expr, ClangToDot::NodeDescriptor & node_desc);
            //          virtual bool VisitSizeOfPackExpr(clang::SizeOfPackExpr * size_of_pack_expr, ClangToDot::NodeDescriptor & node_desc);
                        virtual bool VisitStmtExpr(clang::StmtExpr * stmt_expr, ClangToDot::NodeDescriptor & node_desc);
                        virtual bool VisitStringLiteral(clang::StringLiteral * string_literal, ClangToDot::NodeDescriptor & node_desc);
            //          virtual bool VisitSubstNonTypeTemplateParmPackExpr(clang::SubstNonTypeTemplateParmPackExpr * subst_non_type_template_parm_pack_expr, ClangToDot::NodeDescriptor & node_desc);
                        virtual bool VisitUnaryExprOrTypeTraitExpr(clang::UnaryExprOrTypeTraitExpr * unary_expr_or_type_trait_expr, ClangToDot::NodeDescriptor & node_desc);
                        virtual bool VisitUnaryOperator(clang::UnaryOperator * unary_operator, ClangToDot::NodeDescriptor & node_desc);
            //          virtual bool VisitUnaryTypeTraitExpr(clang::UnaryTypeTraitExpr * unary_type_trait_expr, ClangToDot::NodeDescriptor & node_desc);
                        virtual bool VisitVAArgExpr(clang::VAArgExpr * va_arg_expr, ClangToDot::NodeDescriptor & node_desc);
                    virtual bool VisitForStmt(clang::ForStmt * for_stmt, ClangToDot::NodeDescriptor & node_desc);
                    virtual bool VisitGotoStmt(clang::GotoStmt * goto_stmt, ClangToDot::NodeDescriptor & node_desc);
                    virtual bool VisitIfStmt(clang::IfStmt * if_stmt, ClangToDot::NodeDescriptor & node_desc);
            //      virtual bool VisitIndirectGotoStmt(clang::IndirectGotoStmt * indirect_goto_stmt, ClangToDot::NodeDescriptor & node_desc);
                    virtual bool VisitLabelStmt(clang::LabelStmt * label_stmt, ClangToDot::NodeDescriptor & node_desc);
                    virtual bool VisitNullStmt(clang::NullStmt * null_stmt, ClangToDot::NodeDescriptor & node_desc);
        //          virtual bool VisitObjCAtCatchStmt(clang::ObjCAtCatchStmt * objc_catch_stmt, ClangToDot::NodeDescriptor & node_desc);
        //          virtual bool VisitObjCAtFinallyStmt(clang::ObjCAtFinallyStmt * objc_finally_stmt, ClangToDot::NodeDescriptor & node_desc);
        //          virtual bool VisitObjCAtSynchronizedStmt(clang::ObjCAtSynchronizedStmt * objc_synchronized_stmt, ClangToDot::NodeDescriptor & node_desc);
        //          virtual bool VisitObjCAtThrowStmt(clang::ObjCAtThrowStmt * objc_throw_stmt, ClangToDot::NodeDescriptor & node_desc);
        //          virtual bool VisitObjCAtTryStmt(clang::ObjCAtTryStmt * objc_try_stmt, ClangToDot::NodeDescriptor & node_desc);
        //          virtual bool VisitObjCAtForCollectionStmt(clang::ObjCAtForCollectionStmt * objc_for_collection_stmt, ClangToDot::NodeDescriptor & node_desc);
                    virtual bool VisitReturnStmt(clang::ReturnStmt * return_stmt, ClangToDot::NodeDescriptor & node_desc);
        //          virtual bool VisitSEHExceptStmt(clang::SEHExceptStmt * seh_except_stmt, ClangToDot::NodeDescriptor & node_desc);
        //          virtual bool VisitSEHFinallyStmt(clang::SEHFinallyStmt * seh_finally_stmt, ClangToDot::NodeDescriptor & node_desc);
        //          virtual bool VisitSEHTryStmt(clang::SEHTryStmt * seh_try_stmt, ClangToDot::NodeDescriptor & node_desc);
                    virtual bool VisitSwitchCase(clang::SwitchCase * switch_case, ClangToDot::NodeDescriptor & node_desc);
                        virtual bool VisitCaseStmt(clang::CaseStmt * case_stmt, ClangToDot::NodeDescriptor & node_desc);
                        virtual bool VisitDefaultStmt(clang::DefaultStmt * default_stmt, ClangToDot::NodeDescriptor & node_desc);
                    virtual bool VisitSwitchStmt(clang::SwitchStmt * switch_stmt, ClangToDot::NodeDescriptor & node_desc);
                    virtual bool VisitWhileStmt(clang::WhileStmt * while_stmt, ClangToDot::NodeDescriptor & node_desc);

                virtual bool VisitType(clang::Type * type, NodeDescriptor & node_desc);
                    virtual bool VisitArrayType(clang::ArrayType * array_type, NodeDescriptor & node_desc);
                        virtual bool VisitConstantArrayType(clang::ConstantArrayType * constant_array_type, NodeDescriptor & node_desc);
            //          virtual bool VisitDependentSizedArrayType(clang::DependentSizedArrayType * dependent_sized_array_type, NodeDescriptor & node_desc);
                        virtual bool VisitIncompleteArrayType(clang::IncompleteArrayType * incomplete_array_type, NodeDescriptor & node_desc);
            //          virtual bool VisitVariableArrayType(clang::VariableArrayType * variable_array_type, NodeDescriptor & node_desc);
                    virtual bool VisitAttributedType(clang::AttributedType * attributed_type, NodeDescriptor & node_desc);
            //      virtual bool VisitAutoType(clang::AutoType * auto_type, NodeDescriptor & node_desc);
            //      virtual bool VisitBlockPointerType(clang::BlockPointerType * block_pointer_type, NodeDescriptor & node_desc);
                    virtual bool VisitBuiltinType(clang::BuiltinType * builtin_type, NodeDescriptor & node_desc);
                    virtual bool VisitComplexType(clang::ComplexType * complex_type, NodeDescriptor & node_desc);
            //      virtual bool VisitDecltypeType(clang::DecltypeType * decltype_type, NodeDescriptor & node_desc);
            //          virtual bool VisitDependentDecltypeType(clang::DependentDecltypeType * dependent_decltype_type, NodeDescriptor & node_desc);
            //      virtual bool VisitDependentSizedExtVectorType(clang::DependentSizedExtVectorType * dependent_sized_ext_vector_type, NodeDescriptor & node_desc);
                    virtual bool VisitFunctionType(clang::FunctionType * function_type, NodeDescriptor & node_desc);
                        virtual bool VisitFunctionNoProtoType(clang::FunctionNoProtoType * function_no_proto_type, NodeDescriptor & node_desc);
                        virtual bool VisitFunctionProtoType(clang::FunctionProtoType * function_proass_symo_type, NodeDescriptor & node_desc);
            //      virtual bool VisitInjectedClassNameType(clang::InjectedClassNameType * injected_class_name_type, NodeDescriptor & node_desc);
            //      virtual bool VisitLocInfoType(clang::LocInfoType * loc_info_type, NodeDescriptor & node_desc);
            //      virtual bool VisitMemberPointerType(clang::MemberPointerType * member_pointer_type, NodeDescriptor & node_desc);
        //          virtual bool VisitObjCObjectPointerType(clang::ObjCObjectPointerType * objc_object_pointer_type, NodeDescriptor & node_desc);
        //          virtual bool VisitObjCObjectType(clang::ObjCObjectType * objc_object_type, NodeDescriptor & node_desc);
        //              virtual bool VisitObjCInterfaceType(clang::ObjCInterfaceType * objc_interface_type, NodeDescriptor & node_desc);
        //              virtual bool VisitObjCObjectTypeImpl(clang::ObjCObjectTypeImpl * objc_object_type_impl);
            //      virtual bool VisitPackExpansionType(clang::PackExpansionType * pack_expansion_type, NodeDescriptor & node_desc);
                    virtual bool VisitParenType(clang::ParenType * paren_type, NodeDescriptor & node_desc);
                    virtual bool VisitPointerType(clang::PointerType * pointer_type, NodeDescriptor & node_desc);
            //      virtual bool VisitReferenceType(clang::ReferenceType * reference_type, NodeDescriptor & node_desc);
            //          virtual bool VisitLValueReferenceType(clang::LValueReferenceType * lvalue_reference_type, NodeDescriptor & node_desc);
            //          virtual bool VisitRValueReferenceType(clang::RValueReferenceType * rvalue_reference_type, NodeDescriptor & node_desc);
            //      virtual bool VisitSubstTemplateTypeParmPackType(clang::SubstTemplateTypeParmPackType * subst_template_type_parm_pack_type, NodeDescriptor & node_desc);
            //      virtual bool VisitSubstTemplateTypeParmType(clang::SubstTemplateTypeParmType * subst_template_type_parm_type, NodeDescriptor & node_desc);
                    virtual bool VisitTagType(clang::TagType * tag_type, NodeDescriptor & node_desc);
                        virtual bool VisitEnumType(clang::EnumType * enum_type, NodeDescriptor & node_desc);
                        virtual bool VisitRecordType(clang::RecordType * record_type, NodeDescriptor & node_desc);
            //      virtual bool VisitTemplateSpecializationType(clang::TemplateSpecializationType * template_specialization_type, NodeDescriptor & node_desc);
            //      virtual bool VisitTemplateTypeParmType(clang::TemplateTypeParmType * template_type_parm_type, NodeDescriptor & node_desc);
                    virtual bool VisitTypedefType(clang::TypedefType * typedef_type, NodeDescriptor & node_desc);
            //      virtual bool VisitTypeOfExprType(clang::TypeOfExprType * type_of_expr_type, NodeDescriptor & node_desc);
            //          virtual bool VisitDependentTypeOfExprType(clang::DependentTypeOfExprType * dependent_type_of_expr_type, NodeDescriptor & node_desc);
            //      virtual bool VisitTypeOfType(clang::TypeOfType * type_of_type, NodeDescriptor & node_desc);
            //      virtual bool VisitTypeWithKeyword(clang::TypeWithKeyword * type_with_keyword);
            //          virtual bool VisitDependentNameType(clang::DependentNameType * dependent_name_type, NodeDescriptor & node_desc);
            //          virtual bool VisitDependentTemplateSpecializationType(clang::DependentTemplateSpecializationType * dependent_template_specialization_type, NodeDescriptor & node_desc);
                        virtual bool VisitElaboratedType(clang::ElaboratedType * elaborated_type, NodeDescriptor & node_desc);
            //      virtual bool VisitUnaryTransformType(clang::UnaryTransformType * unary_transform_type, NodeDescriptor & node_desc);
            //      virtual bool VisitUnresolvedUsingType(clang::UnresolvedUsingType * unresolved_using_type, NodeDescriptor & node_desc);
                    virtual bool VisitVectorType(clang::VectorType * vector_type, NodeDescriptor & node_desc);
                        virtual bool VisitExtVectorType(clang::ExtVectorType * ext_vector_type, NodeDescriptor & node_desc);

        };

#endif /* _CLANG_TO_DOT_H_ */

