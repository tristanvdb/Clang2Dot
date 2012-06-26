
#include "clang-to-dot.hpp"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

/***********************/
/* Traverse Statements */
/***********************/

std::string ClangToDot::Traverse(clang::Stmt * stmt) {
    if (stmt == NULL)
        return "";

    // Look for previous translation
    std::map<clang::Stmt *, std::string>::iterator it = p_stmt_translation_map.find(stmt);
    if (it != p_stmt_translation_map.end())
        return it->second; 

    // If first time, create a new entry
    std::string node_ident = genNextIdent();
    p_stmt_translation_map.insert(std::pair<clang::Stmt *, std::string>(stmt, node_ident));
    NodeDescriptor & node_desc = p_node_desc.insert(std::pair<std::string, NodeDescriptor>(node_ident, NodeDescriptor(node_ident))).first->second;

    bool ret_status = false;

    switch (stmt->getStmtClass()) {
     // Stmt
        case clang::Stmt::AsmStmtClass:
            assert(DEBUG_TODO == 0); // TODO
            break;
        case clang::Stmt::BreakStmtClass:
            ret_status = VisitBreakStmt((clang::BreakStmt *)stmt, node_desc);
            break;
        case clang::Stmt::CompoundStmtClass:
            ret_status = VisitCompoundStmt((clang::CompoundStmt *)stmt, node_desc);
            break;
        case clang::Stmt::ContinueStmtClass:
            ret_status = VisitContinueStmt((clang::ContinueStmt *)stmt, node_desc);
            break;
        case clang::Stmt::CXXCatchStmtClass:
        case clang::Stmt::CXXForRangeStmtClass:
        case clang::Stmt::CXXTryStmtClass:
            assert(DEBUG_TODO == 0); // TODO
            break;
        case clang::Stmt::DeclStmtClass:
            ret_status = VisitDeclStmt((clang::DeclStmt *)stmt, node_desc);
            break;
        case clang::Stmt::DoStmtClass:
            ret_status = VisitDoStmt((clang::DoStmt *)stmt, node_desc);
            break;
     // Expr
     // AbstractConditionalOperator
        case clang::Stmt::BinaryConditionalOperatorClass:
            assert(DEBUG_TODO == 0); // TODO
            break;
        case clang::Stmt::ConditionalOperatorClass:
            ret_status = VisitConditionalOperator((clang::ConditionalOperator *)stmt, node_desc);
            break;
        case clang::Stmt::AddrLabelExprClass:
            assert(DEBUG_TODO == 0); // TODO
            break;
        case clang::Stmt::ArraySubscriptExprClass:
            ret_status = VisitArraySubscriptExpr((clang::ArraySubscriptExpr *)stmt, node_desc);
            break;
        case clang::Stmt::ArrayTypeTraitExprClass:
        case clang::Stmt::AsTypeExprClass:
        case clang::Stmt::AtomicExprClass:
            assert(DEBUG_TODO == 0); // TODO
            break;
        case clang::Stmt::BinaryOperatorClass:
            ret_status = VisitBinaryOperator((clang::BinaryOperator *)stmt, node_desc);
            break;
        case clang::Stmt::CompoundAssignOperatorClass:
            ret_status = VisitCompoundAssignOperator((clang::CompoundAssignOperator *)stmt, node_desc);
            break;
        case clang::Stmt::BinaryTypeTraitExprClass:
        case clang::Stmt::BlockDeclRefExprClass:
        case clang::Stmt::BlockExprClass:
            assert(DEBUG_TODO == 0); // TODO
            break;
        case clang::Stmt::CallExprClass:
            ret_status = VisitCallExpr((clang::CallExpr *)stmt, node_desc);
            break;
        case clang::Stmt::CUDAKernelCallExprClass:
        case clang::Stmt::CXXMemberCallExprClass:
        case clang::Stmt::CXXOperatorCallExprClass:
            assert(DEBUG_TODO == 0); // TODO
            break;
     // CastExpr
     // ExplicitCastExpr
        case clang::Stmt::CStyleCastExprClass:
            ret_status = VisitCStyleCastExpr((clang::CStyleCastExpr *)stmt, node_desc);
            break;
        case clang::Stmt::CXXFunctionalCastExprClass:
     // CXXNamedCastExpr
        case clang::Stmt::CXXConstCastExprClass:
        case clang::Stmt::CXXDynamicCastExprClass:
        case clang::Stmt::CXXReinterpretCastExprClass:
        case clang::Stmt::CXXStaticCastExprClass:
     // ObjCBridgedCastExpr 
            assert(DEBUG_TODO == 0); // TODO
            break;
        case clang::Stmt::ImplicitCastExprClass:
            ret_status = VisitImplicitCastExpr((clang::ImplicitCastExpr *)stmt, node_desc);
            break;
        case clang::Stmt::CharacterLiteralClass:
            ret_status = VisitCharacterLiteral((clang::CharacterLiteral *)stmt, node_desc);
            break;
        case clang::Stmt::ChooseExprClass:
            assert(DEBUG_TODO == 0); // TODO
            break;
        case clang::Stmt::CompoundLiteralExprClass:
            ret_status = VisitCompoundLiteralExpr((clang::CompoundLiteralExpr *)stmt, node_desc);
            break;
        case clang::Stmt::CXXBindTemporaryExprClass:
        case clang::Stmt::CXXBoolLiteralExprClass:
            assert(DEBUG_TODO == 0); // TODO
            break;
        case clang::Stmt::CXXConstructExprClass:
            ret_status = VisitCXXConstructExpr((clang::CXXConstructExpr *)stmt, node_desc);
            break;
        case clang::Stmt::CXXTemporaryObjectExprClass:
        case clang::Stmt::CXXDefaultArgExprClass:
        case clang::Stmt::CXXDeleteExprClass:
        case clang::Stmt::CXXDependentScopeMemberExprClass:
        case clang::Stmt::CXXNewExprClass:
        case clang::Stmt::CXXNoexceptExprClass:
        case clang::Stmt::CXXNullPtrLiteralExprClass:
        case clang::Stmt::CXXPseudoDestructorExprClass:
        case clang::Stmt::CXXScalarValueInitExprClass:
        case clang::Stmt::CXXThisExprClass:
        case clang::Stmt::CXXThrowExprClass:
        case clang::Stmt::CXXTypeidExprClass:
        case clang::Stmt::CXXUnresolvedConstructExprClass:
        case clang::Stmt::CXXUuidofExprClass:
            assert(DEBUG_TODO == 0); // TODO
            break;
        case clang::Stmt::DeclRefExprClass:
            ret_status = VisitDeclRefExpr((clang::DeclRefExpr *)stmt, node_desc);
            break;
        case clang::Stmt::DependentScopeDeclRefExprClass:
            assert(DEBUG_TODO == 0); // TODO
            break;
        case clang::Stmt::DesignatedInitExprClass:
            ret_status = VisitDesignatedInitExpr((clang::DesignatedInitExpr *)stmt, node_desc);
            break;
        case clang::Stmt::ExpressionTraitExprClass:
        case clang::Stmt::ExprWithCleanupsClass:
            assert(DEBUG_TODO == 0); // TODO
            break;
        case clang::Stmt::ExtVectorElementExprClass:
            ret_status = VisitExtVectorElementExpr((clang::ExtVectorElementExpr *)stmt, node_desc);
            break;
        case clang::Stmt::FloatingLiteralClass:
            ret_status = VisitFloatingLiteral((clang::FloatingLiteral *)stmt, node_desc);
            break;
        case clang::Stmt::GenericSelectionExprClass:
        case clang::Stmt::GNUNullExprClass:
            assert(DEBUG_TODO == 0); // TODO
            break;
        case clang::Stmt::ImaginaryLiteralClass:
            ret_status = VisitImaginaryLiteral((clang::ImaginaryLiteral *)stmt, node_desc);
            break;
        case clang::Stmt::ImplicitValueInitExprClass:
            ret_status = VisitImplicitValueInitExpr((clang::ImplicitValueInitExpr *)stmt, node_desc);
            break;
        case clang::Stmt::InitListExprClass:
            ret_status = VisitInitListExpr((clang::InitListExpr *)stmt, node_desc);
            break;
        case clang::Stmt::IntegerLiteralClass:
            ret_status = VisitIntegerLiteral((clang::IntegerLiteral *)stmt, node_desc);
            break;
        case clang::Stmt::MaterializeTemporaryExprClass:
            assert(DEBUG_TODO == 0); // TODO
            break;
        case clang::Stmt::MemberExprClass:
            ret_status = VisitMemberExpr((clang::MemberExpr *)stmt, node_desc);
            break;
     // Ojective-C ...
        case clang::Stmt::OffsetOfExprClass:
        case clang::Stmt::OpaqueValueExprClass:
     // OverloadExpr
        case clang::Stmt::UnresolvedLookupExprClass:
        case clang::Stmt::UnresolvedMemberExprClass:
        case clang::Stmt::PackExpansionExprClass:
            assert(DEBUG_TODO == 0); // TODO
            break;
        case clang::Stmt::ParenExprClass:
            ret_status = VisitParenExpr((clang::ParenExpr *)stmt, node_desc);
            break;
        case clang::Stmt::ParenListExprClass:
            assert(DEBUG_TODO == 0); // TODO
            break;
        case clang::Stmt::PredefinedExprClass:
            ret_status = VisitPredefinedExpr((clang::PredefinedExpr *)stmt, node_desc);
            break;
        case clang::Stmt::ShuffleVectorExprClass:
        case clang::Stmt::SizeOfPackExprClass:
            assert(DEBUG_TODO == 0); // TODO
            break;
        case clang::Stmt::StmtExprClass:
            ret_status = VisitStmtExpr((clang::StmtExpr *)stmt, node_desc);
            break;
        case clang::Stmt::StringLiteralClass:
            ret_status = VisitStringLiteral((clang::StringLiteral *)stmt, node_desc);
            break;
        case clang::Stmt::SubstNonTypeTemplateParmExprClass:
        case clang::Stmt::SubstNonTypeTemplateParmPackExprClass:
            assert(DEBUG_TODO == 0); // TODO
            break;
        case clang::Stmt::UnaryExprOrTypeTraitExprClass:
            ret_status = VisitUnaryExprOrTypeTraitExpr((clang::UnaryExprOrTypeTraitExpr *)stmt, node_desc);
            break;
        case clang::Stmt::UnaryOperatorClass:
            ret_status = VisitUnaryOperator((clang::UnaryOperator *)stmt, node_desc);
            break;
        case clang::Stmt::UnaryTypeTraitExprClass:
            assert(DEBUG_TODO == 0); // TODO
            break;
       case clang::Stmt::VAArgExprClass:
            ret_status = VisitVAArgExpr((clang::VAArgExpr *)stmt, node_desc);
            break;
        case clang::Stmt::ForStmtClass:
            ret_status = VisitForStmt((clang::ForStmt *)stmt, node_desc);
            break;
        case clang::Stmt::GotoStmtClass:
            ret_status = VisitGotoStmt((clang::GotoStmt *)stmt, node_desc);
            break;
        case clang::Stmt::IfStmtClass:
            ret_status = VisitIfStmt((clang::IfStmt *)stmt, node_desc);
            break;
        case clang::Stmt::IndirectGotoStmtClass:
            assert(DEBUG_TODO == 0); // TODO
            break;
        case clang::Stmt::LabelStmtClass:
            ret_status = VisitLabelStmt((clang::LabelStmt *)stmt, node_desc);
            break;
        case clang::Stmt::NullStmtClass:
            ret_status = VisitNullStmt((clang::NullStmt *)stmt, node_desc);
            break;
     // Objective-C ...
        case clang::Stmt::ReturnStmtClass:
            ret_status = VisitReturnStmt((clang::ReturnStmt *)stmt, node_desc);
            break;
     // SEH ???
     // SwitchCase
        case clang::Stmt::CaseStmtClass:
            ret_status = VisitCaseStmt((clang::CaseStmt *)stmt, node_desc);
            break;
        case clang::Stmt::DefaultStmtClass:
            ret_status = VisitDefaultStmt((clang::DefaultStmt *)stmt, node_desc);
            break;
        case clang::Stmt::SwitchStmtClass:
            ret_status = VisitSwitchStmt((clang::SwitchStmt *)stmt, node_desc);
            break;
        case clang::Stmt::WhileStmtClass:
            ret_status = VisitWhileStmt((clang::WhileStmt *)stmt, node_desc);
            break;
        default:
            std::cerr << "Unknown statement kind: " << stmt->getStmtClassName() << " !" << std::endl;
            assert(false);
    }

    assert(ret_status != false);

    return node_ident;
}

/********************/
/* Visit Statements */
/********************/

bool ClangToDot::VisitStmt(clang::Stmt * stmt, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("Stmt");

    return res;
}

bool ClangToDot::VisitBreakStmt(clang::BreakStmt * break_stmt, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("BreakStmt"); 

    return VisitStmt(break_stmt, node_desc) && res;
}

bool ClangToDot::VisitCompoundStmt(clang::CompoundStmt * compound_stmt, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("CompoundStmt");

    clang::CompoundStmt::body_iterator it;
    unsigned cnt = 0;
    for (it = compound_stmt->body_begin(); it != compound_stmt->body_end(); it++) {
        std::ostringstream oss;
        oss << "child[" << cnt++ << "]";
        node_desc.successors.push_back(std::pair<std::string, std::string>(oss.str(), Traverse(*it)));
    }

    return VisitStmt(compound_stmt, node_desc) && res;
}

bool ClangToDot::VisitContinueStmt(clang::ContinueStmt * continue_stmt, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("ContinueStmt");

    return VisitStmt(continue_stmt, node_desc) && res;
}

bool ClangToDot::VisitDeclStmt(clang::DeclStmt * decl_stmt, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("DeclStmt");

    if (decl_stmt->isSingleDecl()) {
        node_desc.successors.push_back(std::pair<std::string, std::string>("declaration[0]", Traverse(decl_stmt->getSingleDecl())));
    }
    else {
        clang::DeclStmt::decl_iterator it;
        unsigned cnt = 0;
        for (it = decl_stmt->decl_begin(); it != decl_stmt->decl_end(); it++) {
            std::ostringstream oss;
            oss << "declaration[" << cnt++ << "]";
            node_desc.successors.push_back(std::pair<std::string, std::string>(oss.str(), Traverse(*it)));
        }
    }

    return VisitStmt(decl_stmt, node_desc) && res;
}

bool ClangToDot::VisitDoStmt(clang::DoStmt * do_stmt, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("DoStmt");

    node_desc.successors.push_back(std::pair<std::string, std::string>("condition", Traverse(do_stmt->getCond())));

    node_desc.successors.push_back(std::pair<std::string, std::string>("body", Traverse(do_stmt->getBody())));

    return VisitStmt(do_stmt, node_desc) && res;
}

bool ClangToDot::VisitExpr(clang::Expr * expr, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("Expr");

    node_desc.successors.push_back(std::pair<std::string, std::string>("type", Traverse(expr->getType().getTypePtr())));

    return VisitStmt(expr, node_desc) && res;
}

bool ClangToDot::VisitAbstractConditionalOperator(clang::AbstractConditionalOperator * abstract_conditional_operator, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("AbstractConditionalOperator");

    node_desc.successors.push_back(std::pair<std::string, std::string>("condition", Traverse(abstract_conditional_operator->getCond())));

    node_desc.successors.push_back(std::pair<std::string, std::string>("true_stmt", Traverse(abstract_conditional_operator->getTrueExpr())));

    node_desc.successors.push_back(std::pair<std::string, std::string>("false_stmt", Traverse(abstract_conditional_operator->getFalseExpr())));

    return VisitExpr(abstract_conditional_operator, node_desc) && res;
}

bool ClangToDot::VisitConditionalOperator(clang::ConditionalOperator * conditional_operator, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("ConditionalOperator");

    return VisitAbstractConditionalOperator(conditional_operator, node_desc) && res;
}

bool ClangToDot::VisitArraySubscriptExpr(clang::ArraySubscriptExpr * array_subscript_expr, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("ArraySubscriptExpr");

    node_desc.successors.push_back(std::pair<std::string, std::string>("base", Traverse(array_subscript_expr->getBase())));

    node_desc.successors.push_back(std::pair<std::string, std::string>("index", Traverse(array_subscript_expr->getIdx())));

    return VisitExpr(array_subscript_expr, node_desc) && res;
}

bool ClangToDot::VisitBinaryOperator(clang::BinaryOperator * binary_operator, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("BinaryOperator");

    node_desc.successors.push_back(std::pair<std::string, std::string>("lhs", Traverse(binary_operator->getLHS())));

    node_desc.successors.push_back(std::pair<std::string, std::string>("rhs", Traverse(binary_operator->getRHS())));

    node_desc.attributes.push_back(std::pair<std::string, std::string>("opcode", binary_operator->getOpcodeStr()));

    return VisitExpr(binary_operator, node_desc) && res;
}

bool ClangToDot::VisitCompoundAssignOperator(clang::CompoundAssignOperator * compound_assign_operator, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("CompoundAssignOperator");

    node_desc.successors.push_back(std::pair<std::string, std::string>(
        "computation_lhs_type", Traverse(compound_assign_operator->getComputationLHSType().getTypePtr())
    ));

    node_desc.successors.push_back(std::pair<std::string, std::string>(
        "computation_result_type", Traverse(compound_assign_operator->getComputationResultType().getTypePtr())
    ));

    return VisitBinaryOperator(compound_assign_operator, node_desc) && res;
}

bool ClangToDot::VisitCallExpr(clang::CallExpr * call_expr, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("CallExpr");

    node_desc.successors.push_back(std::pair<std::string, std::string>("callee", Traverse(call_expr->getCallee())));

    node_desc.successors.push_back(std::pair<std::string, std::string>("callee_decl", Traverse(call_expr->getCalleeDecl())));

    node_desc.successors.push_back(std::pair<std::string, std::string>("direct_callee", Traverse(call_expr->getDirectCallee())));

    clang::CallExpr::arg_iterator it;
    unsigned cnt = 0;
    for (it = call_expr->arg_begin(); it != call_expr->arg_end(); ++it) {
        std::ostringstream oss;
        oss << "argument[" << cnt++ << "]";
        node_desc.successors.push_back(std::pair<std::string, std::string>(oss.str(), Traverse(*it)));
    }

    return VisitExpr(call_expr, node_desc) && res;
}

bool ClangToDot::VisitCastExpr(clang::CastExpr * cast, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("CastExpr");

    node_desc.successors.push_back(std::pair<std::string, std::string>("sub_expr", Traverse(cast->getSubExpr())));

    return VisitExpr(cast, node_desc) && res;
}

bool ClangToDot::VisitExplicitCastExpr(clang::ExplicitCastExpr * explicit_cast_expr, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("ExplicitCastExpr");

    node_desc.successors.push_back(std::pair<std::string, std::string>("type_as_written", Traverse(explicit_cast_expr->getTypeAsWritten().getTypePtr())));

    return VisitCastExpr(explicit_cast_expr, node_desc) && res;
}

bool ClangToDot::VisitCStyleCastExpr(clang::CStyleCastExpr * c_style_cast, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("CStyleCastExpr");

    return VisitCastExpr(c_style_cast, node_desc) && res;
}

bool ClangToDot::VisitImplicitCastExpr(clang::ImplicitCastExpr * implicit_cast_expr, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("ImplicitCastExpr");

    return VisitCastExpr(implicit_cast_expr, node_desc) && res;
}

bool ClangToDot::VisitCharacterLiteral(clang::CharacterLiteral * character_literal, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("CharacterLiteral");

    switch (character_literal->getKind()) {
        case clang::CharacterLiteral::Ascii:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("kind", "Ascii"));
            break;
        case clang::CharacterLiteral::Wide:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("kind", "Wide"));
            break;
        case clang::CharacterLiteral::UTF16:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("kind", "UTF16"));
            break;
        case clang::CharacterLiteral::UTF32:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("kind", "UTF32"));
            break;
    }

    std::ostringstream oss;
    oss << std::hex << character_literal->getValue();
    node_desc.attributes.push_back(std::pair<std::string, std::string>("hex_value", oss.str()));

    return VisitExpr(character_literal, node_desc) && res;
}

bool ClangToDot::VisitCompoundLiteralExpr(clang::CompoundLiteralExpr * compound_literal, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("CompoundLiteralExpr");

    node_desc.successors.push_back(std::pair<std::string, std::string>("initializer", Traverse(compound_literal->getInitializer())));

    return VisitExpr(compound_literal, node_desc) && res;
}

bool ClangToDot::VisitCXXConstructExpr(clang::CXXConstructExpr * cxx_constructor_expr, NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("CXXConstructExpr");

    node_desc.successors.push_back(std::pair<std::string, std::string>("constructor", Traverse(cxx_constructor_expr->getConstructor())));

    clang::CXXConstructExpr::arg_iterator it;
    unsigned cnt = 0;
    for (it = cxx_constructor_expr->arg_begin(); it != cxx_constructor_expr->arg_end(); ++it) {
        std::ostringstream oss;
        oss << "argument[" << cnt++ << "]";
        node_desc.successors.push_back(std::pair<std::string, std::string>(oss.str(), Traverse(*it)));
    }

    return VisitExpr(cxx_constructor_expr, node_desc) && res;
}

bool ClangToDot::VisitDeclRefExpr(clang::DeclRefExpr * decl_ref_expr, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("DeclRefExpr");

    node_desc.successors.push_back(std::pair<std::string, std::string>("decl", Traverse(decl_ref_expr->getDecl())));

    return VisitExpr(decl_ref_expr, node_desc) && res;
}

bool ClangToDot::VisitDesignatedInitExpr(clang::DesignatedInitExpr * designated_init_expr, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("DesignatedInitExpr");

    node_desc.successors.push_back(std::pair<std::string, std::string>("init", Traverse(designated_init_expr->getInit())));

    clang::DesignatedInitExpr::designators_iterator it;
    unsigned cnt = 0;
    for (it = designated_init_expr->designators_begin(); it != designated_init_expr->designators_end(); it++) {
        std::ostringstream oss;
        oss << "designator[" << cnt++ << "]";
        if (it->isFieldDesignator()) {
            oss << " field";
            node_desc.successors.push_back(std::pair<std::string, std::string>(oss.str(), Traverse(it->getField())));
        }
        else if (it->isArrayDesignator()) {
            oss << " array";
            node_desc.successors.push_back(std::pair<std::string, std::string>(oss.str(), Traverse(designated_init_expr->getArrayIndex(*it))));
        }
        else if (it->isArrayRangeDesignator()) {
            oss << " range";
            std::ostringstream oss_;
            oss_ << oss.str() << "_end";
            oss << "_start";
            node_desc.successors.push_back(std::pair<std::string, std::string>(oss.str(), Traverse(designated_init_expr->getArrayRangeStart(*it))));
            node_desc.successors.push_back(std::pair<std::string, std::string>(oss_.str(), Traverse(designated_init_expr->getArrayRangeEnd(*it))));
        }
        else assert(false);
    }

    return VisitExpr(designated_init_expr, node_desc) && res;
}

bool ClangToDot::VisitExtVectorElementExpr(clang::ExtVectorElementExpr * ext_vector_element_expr, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("ExtVectorElementExpr");

    node_desc.successors.push_back(std::pair<std::string, std::string>("base", Traverse(ext_vector_element_expr->getBase())));

    if (ext_vector_element_expr->isArrow()) 
        node_desc.attributes.push_back(std::pair<std::string, std::string>("access_operator", "arrow"));
    else 
        node_desc.attributes.push_back(std::pair<std::string, std::string>("access_operator", "dot"));

    clang::IdentifierInfo & ident_info = ext_vector_element_expr->getAccessor();
    std::string ident = ident_info.getName().str();

    node_desc.attributes.push_back(std::pair<std::string, std::string>("accessed_field", ident));

   return VisitExpr(ext_vector_element_expr, node_desc) && res;
}

bool ClangToDot::VisitFloatingLiteral(clang::FloatingLiteral * floating_literal, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("FloatingLiteral");

    // FIXME

    unsigned int precision =  llvm::APFloat::semanticsPrecision(floating_literal->getValue().getSemantics());
    std::ostringstream oss;
    if (precision == 24) {
        node_desc.attributes.push_back(std::pair<std::string, std::string>("precision", "single"));
        oss << floating_literal->getValue().convertToFloat();
    }
    else if (precision == 53) {
        node_desc.attributes.push_back(std::pair<std::string, std::string>("precision", "double"));
        oss << floating_literal->getValue().convertToDouble();
    }
    else
        assert(!"In VisitFloatingLiteral: Unsupported float size");

    node_desc.attributes.push_back(std::pair<std::string, std::string>("value", oss.str()));

    return VisitExpr(floating_literal, node_desc) && res;
}

bool ClangToDot::VisitImaginaryLiteral(clang::ImaginaryLiteral * imaginary_literal, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("ImaginaryLiteral");

    node_desc.successors.push_back(std::pair<std::string, std::string>("sub_expr", Traverse(imaginary_literal->getSubExpr())));

    return VisitExpr(imaginary_literal, node_desc) && res;
}

bool ClangToDot::VisitImplicitValueInitExpr(clang::ImplicitValueInitExpr * implicit_value_init_expr, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("ImplicitValueInitExpr");

    return VisitExpr(implicit_value_init_expr, node_desc) && res;
}

bool ClangToDot::VisitInitListExpr(clang::InitListExpr * init_list_expr, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("InitListExpr");

    clang::InitListExpr::iterator it;
    unsigned cnt = 0;
    for (it = init_list_expr->begin(); it != init_list_expr->end(); it++) {
        std::ostringstream oss;
        oss << "init[" << cnt++ << "]";
        node_desc.successors.push_back(std::pair<std::string, std::string>(oss.str(), Traverse(*it)));
    }

    return VisitExpr(init_list_expr, node_desc) && res;
}

bool ClangToDot::VisitIntegerLiteral(clang::IntegerLiteral * integer_literal, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("IntegerLiteral");

    // FIXME

    std::ostringstream oss;
    oss << std::hex << integer_literal->getValue().getHashValue();
    node_desc.attributes.push_back(std::pair<std::string, std::string>("hex_hash_value", oss.str()));

    return VisitExpr(integer_literal, node_desc) && res;
}

bool ClangToDot::VisitMemberExpr(clang::MemberExpr * member_expr, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("MemberExpr");

    node_desc.successors.push_back(std::pair<std::string, std::string>("base", Traverse(member_expr->getBase())));

    node_desc.successors.push_back(std::pair<std::string, std::string>("member_decl", Traverse(member_expr->getMemberDecl())));

    if (member_expr->isArrow())
        node_desc.attributes.push_back(std::pair<std::string, std::string>("access_operator", "arrow"));
    else
        node_desc.attributes.push_back(std::pair<std::string, std::string>("access_operator", "dot"));

    return VisitExpr(member_expr, node_desc) && res;
}

bool ClangToDot::VisitParenExpr(clang::ParenExpr * paren_expr, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("ParentExpr");

    node_desc.successors.push_back(std::pair<std::string, std::string>("sub_expr", Traverse(paren_expr->getSubExpr())));

    return VisitExpr(paren_expr, node_desc) && res;
}

bool ClangToDot::VisitPredefinedExpr(clang::PredefinedExpr * predefined_expr, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("PredefinedExpr");

    switch (predefined_expr->getIdentType()) {
        case clang::PredefinedExpr::Func:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("ident_type", "func"));
            break;
        case clang::PredefinedExpr::Function:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("ident_type", "function"));
            break;
        case clang::PredefinedExpr::PrettyFunction:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("ident_type", "pretty_function"));
            break;
        case clang::PredefinedExpr::PrettyFunctionNoVirtual:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("ident_type", "pretty_function_no_virtual"));
            break;
    }

    return VisitExpr(predefined_expr, node_desc) && res;
}

bool ClangToDot::VisitStmtExpr(clang::StmtExpr * stmt_expr, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("StmtExpr");

    node_desc.successors.push_back(std::pair<std::string, std::string>("sub_stmt", Traverse(stmt_expr->getSubStmt())));

    return VisitExpr(stmt_expr, node_desc) && res;
}

bool ClangToDot::VisitStringLiteral(clang::StringLiteral * string_literal, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("StringLiteral");

    // TODO coding

    node_desc.attributes.push_back(std::pair<std::string, std::string>("string", string_literal->getString().str()));

    return VisitExpr(string_literal, node_desc) && res;
}

bool ClangToDot::VisitUnaryExprOrTypeTraitExpr(clang::UnaryExprOrTypeTraitExpr * unary_expr_or_type_trait_expr, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("UnaryExprOrTypeTraitExpr");

    if (unary_expr_or_type_trait_expr->isArgumentType()) {
        node_desc.successors.push_back(std::pair<std::string, std::string>(
            "argument_type", Traverse(unary_expr_or_type_trait_expr->getArgumentType().getTypePtr())
        ));
    }
    else {
        node_desc.successors.push_back(std::pair<std::string, std::string>("argument_expr", Traverse(unary_expr_or_type_trait_expr->getArgumentExpr())));
    }

    switch (unary_expr_or_type_trait_expr->getKind()) {
        case clang::UETT_SizeOf:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("kind", "sizeof"));
            break;
        case clang::UETT_AlignOf:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("kind", "alignof"));
            break;
        case clang::UETT_VecStep:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("kind", "vecstep"));
            break;
    }

    return VisitStmt(unary_expr_or_type_trait_expr, node_desc) && res;
}

bool ClangToDot::VisitUnaryOperator(clang::UnaryOperator * unary_operator, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("UnaryOperator");

    node_desc.successors.push_back(std::pair<std::string, std::string>("sub_expr", Traverse(unary_operator->getSubExpr())));

    switch (unary_operator->getOpcode()) {
        case clang::UO_PostInc:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("opcode", "PostInc"));
            break;
        case clang::UO_PostDec:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("opcode", "PostDec"));
            break;
        case clang::UO_PreInc:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("opcode", "PreInc"));
            break;
        case clang::UO_PreDec:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("opcode", "PreDec"));
            break;
        case clang::UO_AddrOf:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("opcode", "AddrOf"));
            break;
        case clang::UO_Deref:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("opcode", "Deref"));
            break;
        case clang::UO_Plus:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("opcode", "Plus"));
            break;
        case clang::UO_Minus:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("opcode", "Minus"));
            break;
        case clang::UO_Not:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("opcode", "Not"));
            break;
        case clang::UO_LNot:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("opcode", "LNot"));
            break;
        case clang::UO_Real:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("opcode", "Real"));
            break;
        case clang::UO_Imag:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("opcode", "Imag"));
            break;
        case clang::UO_Extension:
            node_desc.attributes.push_back(std::pair<std::string, std::string>("opcode", "Extension"));
            break;
    }

    return VisitExpr(unary_operator, node_desc) && res;
}

bool ClangToDot::VisitVAArgExpr(clang::VAArgExpr * va_arg_expr, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("VAArgExpr");

    node_desc.successors.push_back(std::pair<std::string, std::string>("sub_expr", Traverse(va_arg_expr->getSubExpr())));

    return VisitExpr(va_arg_expr, node_desc) && res;
}

bool ClangToDot::VisitForStmt(clang::ForStmt * for_stmt, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("ForStmt");

    node_desc.successors.push_back(std::pair<std::string, std::string>("init", Traverse(for_stmt->getInit())));

    node_desc.successors.push_back(std::pair<std::string, std::string>("cond", Traverse(for_stmt->getCond())));

    node_desc.successors.push_back(std::pair<std::string, std::string>("inc", Traverse(for_stmt->getInc())));

    node_desc.successors.push_back(std::pair<std::string, std::string>("body", Traverse(for_stmt->getBody())));

    return VisitStmt(for_stmt, node_desc) && res;
}

bool ClangToDot::VisitGotoStmt(clang::GotoStmt * goto_stmt, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("GotoStmt");

    node_desc.successors.push_back(std::pair<std::string, std::string>("label", Traverse(goto_stmt->getLabel()->getStmt())));

    return VisitStmt(goto_stmt, node_desc) && res;
}

bool ClangToDot::VisitIfStmt(clang::IfStmt * if_stmt, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("IfStmt");

    node_desc.successors.push_back(std::pair<std::string, std::string>("cond", Traverse(if_stmt->getCond())));

    node_desc.successors.push_back(std::pair<std::string, std::string>("then", Traverse(if_stmt->getThen())));

    node_desc.successors.push_back(std::pair<std::string, std::string>("else", Traverse(if_stmt->getElse())));

    return VisitStmt(if_stmt, node_desc) && res;
}

bool ClangToDot::VisitLabelStmt(clang::LabelStmt * label_stmt, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("LabelStmt");

    node_desc.attributes.push_back(std::pair<std::string, std::string>("name", label_stmt->getName()));

    node_desc.successors.push_back(std::pair<std::string, std::string>("sub_stmt", Traverse(label_stmt->getSubStmt())));

    return VisitStmt(label_stmt, node_desc) && res;
}

bool ClangToDot::VisitNullStmt(clang::NullStmt * null_stmt, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("NullStmt");

    return VisitStmt(null_stmt, node_desc) && res;
}

bool ClangToDot::VisitReturnStmt(clang::ReturnStmt * return_stmt, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("ReturnStmt");

    node_desc.successors.push_back(std::pair<std::string, std::string>("return_value", Traverse(return_stmt->getRetValue())));

    return VisitStmt(return_stmt, node_desc) && res;
}

bool ClangToDot::VisitSwitchCase(clang::SwitchCase * switch_case, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("SwitchCase");

    node_desc.successors.push_back(std::pair<std::string, std::string>("sub_stmt", Traverse(switch_case->getSubStmt())));

    return VisitStmt(switch_case, node_desc) && res;
}

bool ClangToDot::VisitCaseStmt(clang::CaseStmt * case_stmt, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("CaseStmt");

    node_desc.successors.push_back(std::pair<std::string, std::string>("lhs", Traverse(case_stmt->getLHS())));

    node_desc.successors.push_back(std::pair<std::string, std::string>("rhs", Traverse(case_stmt->getRHS())));

    return VisitSwitchCase(case_stmt, node_desc) && res;
}

bool ClangToDot::VisitDefaultStmt(clang::DefaultStmt * default_stmt, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;    

    node_desc.kind_hierarchy.push_back("DefaultStmt");

    return VisitSwitchCase(default_stmt, node_desc) && res;
}

bool ClangToDot::VisitSwitchStmt(clang::SwitchStmt * switch_stmt, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("SwitchStmt");

    node_desc.successors.push_back(std::pair<std::string, std::string>("cond", Traverse(switch_stmt->getCond())));
    
    node_desc.successors.push_back(std::pair<std::string, std::string>("body", Traverse(switch_stmt->getBody())));

    return VisitStmt(switch_stmt, node_desc) && res;
}

bool ClangToDot::VisitWhileStmt(clang::WhileStmt * while_stmt, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("WhileStmt");

    node_desc.successors.push_back(std::pair<std::string, std::string>("cond", Traverse(while_stmt->getCond())));

    node_desc.successors.push_back(std::pair<std::string, std::string>("body", Traverse(while_stmt->getBody())));

    return VisitStmt(while_stmt, node_desc) && res;
}

