
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
            NodeDescriptor();

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

        NodeDescriptor & getDescriptor(std::string);

    public:
        ClangToDot(clang::CompilerInstance * compiler_instance, Language language_);

        virtual ~ClangToDot();

  /* ASTConsumer's methods overload */

        virtual void HandleTranslationUnit(clang::ASTContext & ast_context);

  /* Traverse methods */

        virtual std::string Traverse(clang::Decl * decl);
        virtual std::string Traverse(clang::Stmt * stmt);
        virtual std::string Traverse(const clang::Type * type);
//        virtual void TraverseForDeclContext(clang::DeclContext * decl_context);

  /* Visit methods */

        virtual bool VisitDecl(clang::Decl * decl, std::string & res);
    //      virtual bool VisitAccessSpecDecl(clang::AccessSpecDecl * access_spec_decl, std::string & res);
    //      virtual bool VisitBlockDecl(clang::BlockDecl * block_decl, std::string & res);
    //      virtual bool VisitFileScopeAsmDecl(clang::FileScopeAsmDecl * file_scope_asm_decl, std::string & res);
    //      virtual bool VisitFriendDecl(clang::FriendDecl * friend_decl, std::string & res);
    //      virtual bool VisitFriendTemplateDecl(clang::FriendTemplateDecl * friend_template_decl, std::string & res);
    //      virtual bool VisitNamedDecl(clang::NamedDecl * named_decl, std::string & res);
    //          virtual bool VisitLabelDecl(clang::LabelDecl * label_decl, std::string & res);
    //          virtual bool VisitNamespaceDecl(clang::NamespaceDecl * namespace_decl, std::string & res);
//              virtual bool VisitObjCCompatibleAliasDecl(clang::ObjCCompatibleAliasDecl * objc_compatible_alias_decl)
//              virtual bool VisitObjCContainerDecl(clang::ObjCContainerDecl * objc_container_decl, std::string & res);
//                  virtual bool VisitObjCCategoryDecl(clang::ObjCCategoryDecl * objc_categoty_decl, std::string & res);
//                  virtual bool VisitObjCImplDecl(clang::ObjCImplDecl * objc_impl_decl, std::string & res);
//                      virtual bool VisitObjCCategoryImplDecl(clang::ObjCCategoryImplDecl * objc_category_impl_decl, std::string & res);
//                      virtual bool VisitObjCImplementationDecl(clang::ObjCImplementationDecl * objc_implementation_decl, std::string & res);
//                  virtual bool VisitObjCInterfaceDecl(clang::ObjCInterfaceDecl * objc_interface_decl, std::string & res);
//                  virtual bool VisitObjCProtocolDecl(clang::ObjCProtocolDecl * objc_protocol_decl, std::string & res);
//              virtual bool VisitObjCMethodDecl(clang::ObjCMethodDecl * objc_method_decl, std::string & res);
//              virtual bool VisitObjCPropertyDecl(clang::ObjCPropertyDecl * objc_property_decl, std::string & res);
    //          virtual bool VisitTemplateDecl(clang::TemplateDecl * template_decl, std::string & res);
    //              virtual bool VisitRedeclarableTemplateDecl(clang::RedeclarableTemplateDecl * redeclarable_template_decl, std::string & res);
    //                  virtual bool VisitClassTemplateDecl(clang::ClassTemplateDecl * class_template_decl, std::string & res);
    //                  virtual bool VisitFunctionTemplateDecl(clang::FunctionTemplateDecl * function_template_decl, std::string & res);
    //                  virtual bool VisitTypeAliasTemplateDecl(clang::TypeAliasTemplateDecl * type_alias_template_decl, std::string & res);
    //              virtual bool VisitTemplateTemplateParmDecl(clang::TemplateTemplateParmDecl * template_template_parm_decl, std::string & res);
    //          virtual bool VisitTypeDecl(clang::TypeDecl * type_decl, std::string & res);
    //              virtual bool VisitTagDecl(clang::TagDecl * tag_decl, std::string & res);
                        virtual bool VisitRecordDecl(clang::RecordDecl * record_decl, std::string & res);
                            virtual bool VisitCXXRecordDecl(clang::CXXRecordDecl * cxx_record_decl, std::string & res);
    //                          virtual bool VisitClassTemplateSpecializationDecl(clang::ClassTemplateSpecializationDecl * class_tpl_spec_decl, std::string & res);
    //                              virtual bool VisitClassTemplatePartialSpecializationDecl(clang::ClassTemplatePartialSpecializationDecl * class_tpl_part_spec_decl, std::string & res);
                        virtual bool VisitEnumDecl(clang::EnumDecl * enum_decl, std::string & res); 
    //              virtual bool VisitTemplateTypeParmDecl(clang::TemplateTypeParmDecl * template_type_parm_decl, std::string & res);
    //              virtual bool VisitTypedefNameDecl(clang::TypedefNameDecl * typedef_name_decl, std::string & res);
                        virtual bool VisitTypedefDecl(clang::TypedefDecl * typedef_decl, std::string & res);
    //                  virtual bool VisitTypeAliasDecl(clang::TypeAliasDecl * type_alias_decl, std::string & res);
    //          virtual bool VisitValueDecl(clang::ValueDecl * value_decl, std::string & res);
    //              virtual bool VisitDeclaratorDecl(clang::DeclaratorDecl * declarator_decl, std::string & res);
                        virtual bool VisitFieldDecl(clang::FieldDecl * field_decl, std::string & res);
//                          virtual bool VisitObjCAtDefsFieldDecl(clang::ObjCAtDefsFieldDecl * objc_at_defs_field_decl, std::string & res);
//                          virtual bool VisitObjCIvarDecl(clang::ObjCIvarDecl * objc_ivar_decl, std::string & res);
                        virtual bool VisitFunctionDecl(clang::FunctionDecl * function_decl, std::string & res);
    //                      virtual bool VisitCXXMethodDecl(clang::CXXMethodDecl * cxx_method_decl, std::string & res);
    //                          virtual bool VisitCXXConstructorDecl(clang::CXXConstructorDecl * cxx_constructor_decl, std::string & res);
    //                          virtual bool VisitCXXConversionDecl(clang::CXXConversionDecl * cxx_conversion_decl, std::string & res);
    //                          virtual bool VisitCXXDestructorDecl(clang::CXXDestructorDecl * cxx_destructor_decl, std::string & res);
    //                  virtual bool VisitNonTypeTemplateParmDecl(clang::NonTypeTemplateParmDecl * non_type_template_param_decl, std::string & res);
                        virtual bool VisitVarDecl(clang::VarDecl * var_decl, std::string & res);
    //                      virtual bool VisitImplicitParamDecl(clang::ImplicitParamDecl * implicit_param_decl, std::string & res);
                            virtual bool VisitParmVarDecl(clang::ParmVarDecl * param_var_decl, std::string & res);
                    virtual bool VisitEnumConstantDecl(clang::EnumConstantDecl * enum_constant_decl, std::string & res);
    //              virtual bool VisitIndirectFieldDecl(clang::IndirectFieldDecl * indirect_field_decl, std::string & res);
            virtual bool VisitTranslationUnitDecl(clang::TranslationUnitDecl * translation_unit_decl, std::string & res);

        virtual bool VisitStmt(clang::Stmt * stmt, std::string & res);
    //      virtual bool VisitAsmStmt(clang::AsmStmt * asm_stmt, std::string & res);
            virtual bool VisitBreakStmt(clang::BreakStmt * break_stmt, std::string & res);
            virtual bool VisitCompoundStmt(clang::CompoundStmt * compound_stmt, std::string & res);
            virtual bool VisitContinueStmt(clang::ContinueStmt * continue_stmt, std::string & res);
    //      virtual bool VisitCXXCatchStmt(clang::CXXCatchStmt * cxx_catch_stmt, std::string & res);
    //      virtual bool VisitCXXForRangeStmt(clang::CXXForRangeStmt * cxx_for_range_stmt, std::string & res);
    //      virtual bool VisitCXXTryStmt(clang::CXXTryStmt * cxx_try_stmt, std::string & res);
            virtual bool VisitDeclStmt(clang::DeclStmt * decl_stmt, std::string & res);
            virtual bool VisitDoStmt(clang::DoStmt * do_stmt, std::string & res);
            virtual bool VisitExpr(clang::Expr * expr, std::string & res);
    //          virtual bool VisitAbstractConditionalOperator(clang::AbstractConditionalOperator * abstract_conditional_operator, std::string & res);
    //              virtual bool VisitBinaryConditionalOperator(clang::BinaryConditionalOperator * binary_conditionnal_operator, std::string & res);
                    virtual bool VisitConditionalOperator(clang::ConditionalOperator * conditional_operator, std::string & res);
    //          virtual bool VisitAddrLabelExpr(clang::AddrLabelExpr * addr_label_expr, std::string & res);
                virtual bool VisitArraySubscriptExpr(clang::ArraySubscriptExpr * array_subscript_expr, std::string & res);
    //          virtual bool VisitArrayTypeTraitExpr(clang::ArrayTypeTraitExpr * array_type_trait_expr, std::string & res);
    //          virtual bool VisitAsTypeExpr(clang::AsTypeExpr * as_type_expr, std::string & res);
                virtual bool VisitBinaryOperator(clang::BinaryOperator * binary_operator, std::string & res);
    //              virtual bool VisitCompoundAssignOperator(clang::CompoundAssignOperator * compound_assign_operator, std::string & res);
    //          virtual bool VisitBinaryTypeTraitExpr(clang::BinaryTypeTraitExpr * binary_type_trait_expr, std::string & res);
    //          virtual bool VisitBlockDeclRefExpr(clang::BlockDeclRefExpr * block_decl_ref_expr, std::string & res);
    //          virtual bool VisitBlockExpr(clang::BlockExpr * block_expr, std::string & res);
                virtual bool VisitCallExpr(clang::CallExpr * call_expr, std::string & res);
    //              virtual bool VisitCUDAKernelCallExpr(clang::CUDAKernelCallExpr * cuda_kernel_call_expr, std::string & res);
    //              virtual bool VisitCXXMemberCallExpr(clang::CXXMemberCallExpr * cxx_member_call_expr, std::string & res);
    //              virtual bool VisitCXXOperatorCallExpr(clang::CXXOperatorCallExpr * cxx_operator_call_expr, std::string & res);
                virtual bool VisitCastExpr(clang::CastExpr * cast_expr, std::string & res);
                    virtual bool VisitExplicitCastExpr(clang::ExplicitCastExpr * explicit_cast_expr, std::string & res);
                        virtual bool VisitCStyleCastExpr(clang::CStyleCastExpr * c_style_cast, std::string & res);
    //                  virtual bool VisitCXXFunctionalCastExpr(clang::CXXFunctionalCastExpr * cxx_functional_cast_expr, std::string & res);
    //                  virtual bool VisitCXXNamedCastExpr(clang::CXXNamedCastExpr * cxx_named_cast_expr, std::string & res);
    //                      virtual bool VisitCXXConstCastExpr(clang::CXXConstCastExpr * cxx_const_cast_expr, std::string & res);
    //                      virtual bool VisitCXXDynamicCastExpr(clang::CXXDynamicCastExpr * cxx_dynamic_cast_expr, std::string & res);
    //                      virtual bool VisitCXXReinterpretCastExpr(clang::CXXReinterpretCastExpr * cxx_reinterpret_cast_expr, std::string & res);
    //                      virtual bool VisitCXXStaticCastExpr(clang::CXXStaticCastExpr * cxx_static_cast_expr, std::string & res);
                    virtual bool VisitImplicitCastExpr(clang::ImplicitCastExpr * implicit_cast_expr, std::string & res);
                virtual bool VisitCharacterLiteral(clang::CharacterLiteral * character_literal, std::string & res);
    //          virtual bool VisitChooseExpr(clang::ChooseExpr * choose_expr, std::string & res);
                virtual bool VisitCompoundLiteralExpr(clang::CompoundLiteralExpr * compound_literal, std::string & res);
    //          virtual bool VisitCXXBindTemporaryExpr(clang::CXXBindTemporaryExpr * cxx_bind_temporary_expr, std::string & res);
    //          virtual bool VisitCXXBoolLiteralExpr(clang::CXXBoolLiteralExpr * cxx_bool_literal_expr, std::string & res);
    //          virtual bool VisitCXXConstructExpr(clang::CXXConstructExpr * cxx_constructor_expr, std::string & res);
    //          virtual bool VisitCXXTemporaryObjectExpr(clang::CXXTemporaryObjectExpr * cxx_temporary_object_expr, std::string & res);
    //          virtual bool VisitCXXDefaultArgExpr(clang::CXXDefaultArgExpr * cxx_default_arg_expr, std::string & res);
    //          virtual bool VisitCXXDeleteExpr(clang::CXXDeleteExpr * cxx_delete_expr, std::string & res);
    //          virtual bool VisitCXXDependentScopeMemberExpr(clang::CXXDependentScopeMemberExpr * cxx_dependent_scope_member_expr, std::string & res);
    //          virtual bool VisitCXXNewExpr(clang::CXXNewExpr * cxx_new_expr, std::string & res);
    //          virtual bool VisitCXXNoexceptExpr(clang::CXXNoexceptExpr * cxx_noexecept_expr, std::string & res);
    //          virtual bool VisitCXXNullPtrLiteralExpr(clang::CXXNullPtrLiteralExpr * cxx_null_ptr_literal_expr, std::string & res);
    //          virtual bool VisitCXXPseudoDestructorExpr(clang::CXXPseudoDestructorExpr * cxx_pseudo_destructor_expr, std::string & res);
    //          virtual bool VisitCXXScalarValueInitExpr(clang::CXXScalarValueInitExpr * cxx_scalar_value_init_expr, std::string & res);
    //          virtual bool VisitCXXThisExpr(clang::CXXThisExpr * cxx_this_expr, std::string & res);
    //          virtual bool VisitCXXThrowExpr(clang::CXXThrowExpr * cxx_throw_expr, std::string & res);
    //          virtual bool VisitCXXTypeidExpr(clang::CXXTypeidExpr * cxx_typeid_expr, std::string & res);
    //          virtual bool VisitCXXUnresolvedConstructExpr(clang::CXXUnresolvedConstructExpr * cxx_unresolved_construct_expr, std::string & res);
    //          virtual bool VisitCXXUuidofExpr(clang::CXXUuidofExpr * cxx_uuidof_expr, std::string & res);
                virtual bool VisitDeclRefExpr(clang::DeclRefExpr * decl_ref_expr, std::string & res);
    //          virtual bool VisitDependentScopeDeclRefExpr(clang::DependentScopeDeclRefExpr * dependent_scope_decl_ref_expr, std::string & res);
                virtual bool VisitDesignatedInitExpr(clang::DesignatedInitExpr * designated_init_expr, std::string & res);
    //          virtual bool VisitExpressionTraitExpr(clang::ExpressionTraitExpr * expression_trait_expr, std::string & res);
    //          virtual bool VisitExprWithCleanups(clang::ExprWithCleanups * expr_with_cleanups);
                        virtual bool VisitExtVectorElementExpr(clang::ExtVectorElementExpr * ext_vector_element_expr, std::string & res);
                        virtual bool VisitFloatingLiteral(clang::FloatingLiteral * floating_literal, std::string & res);
            //          virtual bool VisitGenericSelectionExpr(clang::GenericSelectionExpr * generic_selection_expr, std::string & res);
            //          virtual bool VisitGNUNullExpr(clang::GNUNullExpr * gnu_null_expr, std::string & res);
                        virtual bool VisitImaginaryLiteral(clang::ImaginaryLiteral * imaginary_literal, std::string & res);
            //          virtual bool VisitImplicitValueInitExpr(clang::ImplicitValueInitExpr * implicit_value_init_expr, std::string & res);
                        virtual bool VisitInitListExpr(clang::InitListExpr * init_list_expr, std::string & res);
                        virtual bool VisitIntegerLiteral(clang::IntegerLiteral * integer_literal, std::string & res);
                        virtual bool VisitMemberExpr(clang::MemberExpr * member_expr, std::string & res);
        //              virtual bool VisitObjCEncodeExpr(clang::ObjCEncodeExpr * objc_encoder_expr, std::string & res);
        //              virtual bool VisitObjCIsaExpr(clang::ObjCIsaExpr * objc_isa_expr, std::string & res);
        //              virtual bool VisitObjCIvarRefExpr(clang::ObjCIvarRefExpr * objc_ivar_ref_expr, std::string & res);
        //              virtual bool VisitObjCMessageExpr(clang::ObjCMessageExpr * objc_message_expr, std::string & res);
        //              virtual bool VisitObjCPropertyRefExpr(clang::ObjCPropertyRefExpr * objc_propery_ref_expr, std::string & res);
        //              virtual bool VisitObjCProtocolExpr(clang::ObjCProtocolExpr * objc_protocol_expr, std::string & res);
        //              virtual bool VisitObjCSelectorExpr(clang::ObjCSelectorExpr * objc_selector_expr, std::string & res);
        //              virtual bool VisitObjCStringLiteral(clang::ObjCStringLiteral * objc_string_literal, std::string & res);
            //          virtual bool VisitOffsetOfExpr(clang::OffsetOfExpr * offset_of_expr, std::string & res);
            //          virtual bool VisitOpaqueValueExpr(clang::OpaqueValueExpr * opaque_value_expr, std::string & res);
            //          virtual bool VisitOverloadExpr(clang::OverloadExpr * overload_expr, std::string & res);
            //              virtual bool VisitUnresolvedLookupExpr(clang::UnresolvedLookupExpr * unresolved_lookup_expr, std::string & res);
            //              virtual bool VisitUnresolvedMemberExpr(clang::UnresolvedMemberExpr * unresolved_member_expr, std::string & res);
            //          virtual bool VisitPackExpansionExpr(clang::PackExpansionExpr * pack_expansion_expr, std::string & res);
                        virtual bool VisitParenExpr(clang::ParenExpr * paren_expr, std::string & res);
            //          virtual bool VisitParenListExpr(clang::ParenListExpr * paren_list_expr, std::string & res);
                        virtual bool VisitPredefinedExpr(clang::PredefinedExpr * predefined_expr, std::string & res);
            //          virtual bool VisitShuffleVectorExpr(clang::ShuffleVectorExpr * shuffle_vector_expr, std::string & res);
            //          virtual bool VisitSizeOfPackExpr(clang::SizeOfPackExpr * size_of_pack_expr, std::string & res);
                        virtual bool VisitStmtExpr(clang::StmtExpr * stmt_expr, std::string & res);
                        virtual bool VisitStringLiteral(clang::StringLiteral * string_literal, std::string & res);
            //          virtual bool VisitSubstNonTypeTemplateParmPackExpr(clang::SubstNonTypeTemplateParmPackExpr * subst_non_type_template_parm_pack_expr, std::string & res);
                        virtual bool VisitUnaryExprOrTypeTraitExpr(clang::UnaryExprOrTypeTraitExpr * unary_expr_or_type_trait_expr, std::string & res);
                        virtual bool VisitUnaryOperator(clang::UnaryOperator * unary_operator, std::string & res);
            //          virtual bool VisitUnaryTypeTraitExpr(clang::UnaryTypeTraitExpr * unary_type_trait_expr, std::string & res);
                        virtual bool VisitVAArgExpr(clang::VAArgExpr * va_arg_expr, std::string & res);
                    virtual bool VisitForStmt(clang::ForStmt * for_stmt, std::string & res);
                    virtual bool VisitGotoStmt(clang::GotoStmt * goto_stmt, std::string & res);
                    virtual bool VisitIfStmt(clang::IfStmt * if_stmt, std::string & res);
            //      virtual bool VisitIndirectGotoStmt(clang::IndirectGotoStmt * indirect_goto_stmt, std::string & res);
                    virtual bool VisitLabelStmt(clang::LabelStmt * label_stmt, std::string & res);
                    virtual bool VisitNullStmt(clang::NullStmt * null_stmt, std::string & res);
        //          virtual bool VisitObjCAtCatchStmt(clang::ObjCAtCatchStmt * objc_catch_stmt, std::string & res);
        //          virtual bool VisitObjCAtFinallyStmt(clang::ObjCAtFinallyStmt * objc_finally_stmt, std::string & res);
        //          virtual bool VisitObjCAtSynchronizedStmt(clang::ObjCAtSynchronizedStmt * objc_synchronized_stmt, std::string & res);
        //          virtual bool VisitObjCAtThrowStmt(clang::ObjCAtThrowStmt * objc_throw_stmt, std::string & res);
        //          virtual bool VisitObjCAtTryStmt(clang::ObjCAtTryStmt * objc_try_stmt, std::string & res);
        //          virtual bool VisitObjCAtForCollectionStmt(clang::ObjCAtForCollectionStmt * objc_for_collection_stmt, std::string & res);
                    virtual bool VisitReturnStmt(clang::ReturnStmt * return_stmt, std::string & res);
        //          virtual bool VisitSEHExceptStmt(clang::SEHExceptStmt * seh_except_stmt, std::string & res);
        //          virtual bool VisitSEHFinallyStmt(clang::SEHFinallyStmt * seh_finally_stmt, std::string & res);
        //          virtual bool VisitSEHTryStmt(clang::SEHTryStmt * seh_try_stmt, std::string & res);
            //      virtual bool VisitSwitchCase(clang::SwitchCase * switch_case);
                        virtual bool VisitCaseStmt(clang::CaseStmt * case_stmt, std::string & res);
                        virtual bool VisitDefaultStmt(clang::DefaultStmt * default_stmt, std::string & res);
                    virtual bool VisitSwitchStmt(clang::SwitchStmt * switch_stmt, std::string & res);
                    virtual bool VisitWhileStmt(clang::WhileStmt * while_stmt, std::string & res);

                virtual bool VisitType(clang::Type * type);
            //      virtual bool VisitArrayType(clang::ArrayType * array_type);
                        virtual bool VisitConstantArrayType(clang::ConstantArrayType * constant_array_type);
            //          virtual bool VisitDependentSizedArrayType(clang::DependentSizedArrayType * dependent_sized_array_type);
                        virtual bool VisitIncompleteArrayType(clang::IncompleteArrayType * incomplete_array_type);
            //          virtual bool VisitVariableArrayType(clang::VariableArrayType * variable_array_type);
                    virtual bool VisitAttributedType(clang::AttributedType * attributed_type);
            //      virtual bool VisitAutoType(clang::AutoType * auto_type);
            //      virtual bool VisitBlockPointerType(clang::BlockPointerType * block_pointer_type);
                    virtual bool VisitBuiltinType(clang::BuiltinType * builtin_type);
                    virtual bool VisitComplexType(clang::ComplexType * complex_type);
            //      virtual bool VisitDecltypeType(clang::DecltypeType * decltype_type);
            //          virtual bool VisitDependentDecltypeType(clang::DependentDecltypeType * dependent_decltype_type);
            //      virtual bool VisitDependentSizedExtVectorType(clang::DependentSizedExtVectorType * dependent_sized_ext_vector_type);
            //      virtual bool VisitFunctionType(clang::FunctionType * function_type);
                        virtual bool VisitFunctionNoProtoType(clang::FunctionNoProtoType * function_no_proto_type);
                        virtual bool VisitFunctionProtoType(clang::FunctionProtoType * function_proass_symo_type);
            //      virtual bool VisitInjectedClassNameType(clang::InjectedClassNameType * injected_class_name_type);
            //      virtual bool VisitLocInfoType(clang::LocInfoType * loc_info_type);
            //      virtual bool VisitMemberPointerType(clang::MemberPointerType * member_pointer_type);
        //          virtual bool VisitObjCObjectPointerType(clang::ObjCObjectPointerType * objc_object_pointer_type);
        //          virtual bool VisitObjCObjectType(clang::ObjCObjectType * objc_object_type);
        //              virtual bool VisitObjCInterfaceType(clang::ObjCInterfaceType * objc_interface_type);
        //              virtual bool VisitObjCObjectTypeImpl(clang::ObjCObjectTypeImpl * objc_object_type_impl);
            //      virtual bool VisitPackExpansionType(clang::PackExpansionType * pack_expansion_type);
                    virtual bool VisitParenType(clang::ParenType * paren_type);
                    virtual bool VisitPointerType(clang::PointerType * pointer_type);
            //      virtual bool VisitReferenceType(clang::ReferenceType * reference_type);
            //          virtual bool VisitLValueReferenceType(clang::LValueReferenceType * lvalue_reference_type);
            //          virtual bool VisitRValueReferenceType(clang::RValueReferenceType * rvalue_reference_type);
            //      virtual bool VisitSubstTemplateTypeParmPackType(clang::SubstTemplateTypeParmPackType * subst_template_type_parm_pack_type);
            //      virtual bool VisitSubstTemplateTypeParmType(clang::SubstTemplateTypeParmType * subst_template_type_parm_type);
            //      virtual bool VisitTagType(clang::TagType * tag_type);
                        virtual bool VisitEnumType(clang::EnumType * enum_type);
                        virtual bool VisitRecordType(clang::RecordType * record_type);
            //      virtual bool VisitTemplateSpecializationType(clang::TemplateSpecializationType * template_specialization_type);
            //      virtual bool VisitTemplateTypeParmType(clang::TemplateTypeParmType * template_type_parm_type);
                    virtual bool VisitTypedefType(clang::TypedefType * typedef_type);
            //      virtual bool VisitTypeOfExprType(clang::TypeOfExprType * type_of_expr_type);
            //          virtual bool VisitDependentTypeOfExprType(clang::DependentTypeOfExprType * dependent_type_of_expr_type);
            //      virtual bool VisitTypeOfType(clang::TypeOfType * type_of_type);
            //      virtual bool VisitTypeWithKeyword(clang::TypeWithKeyword * type_with_keyword);
            //          virtual bool VisitDependentNameType(clang::DependentNameType * dependent_name_type);
            //          virtual bool VisitDependentTemplateSpecializationType(clang::DependentTemplateSpecializationType * dependent_template_specialization_type);
                        virtual bool VisitElaboratedType(clang::ElaboratedType * elaborated_type);
            //      virtual bool VisitUnaryTransformType(clang::UnaryTransformType * unary_transform_type);
            //      virtual bool VisitUnresolvedUsingType(clang::UnresolvedUsingType * unresolved_using_type);
                    virtual bool VisitVectorType(clang::VectorType * vector_type);
            //          virtual bool VisitExtVectorType(clang::ExtVectorType * ext_vector_type);

        };

#endif /* _CLANG_TO_DOT_H_ */

