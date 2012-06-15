
#include "clang-to-dot.hpp"

#include <iostream>

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
        case clang::Stmt::InitListExprClass:
            ret_status = VisitInitListExpr((clang::InitListExpr *)stmt, node_desc);
            break;
        case clang::Stmt::DesignatedInitExprClass:
            ret_status = VisitDesignatedInitExpr((clang::DesignatedInitExpr *)stmt, node_desc);
            break;
        case clang::Stmt::IntegerLiteralClass:
            ret_status = VisitIntegerLiteral((clang::IntegerLiteral *)stmt, node_desc);
            break;
        case clang::Stmt::FloatingLiteralClass:
            ret_status = VisitFloatingLiteral((clang::FloatingLiteral *)stmt, node_desc);
            break;
        case clang::Stmt::ImaginaryLiteralClass:
            ret_status = VisitImaginaryLiteral((clang::ImaginaryLiteral *)stmt, node_desc);
            break;
        case clang::Stmt::CompoundLiteralExprClass:
            ret_status = VisitCompoundLiteralExpr((clang::CompoundLiteralExpr *)stmt, node_desc);
            break;
        case clang::Stmt::ImplicitCastExprClass:
            ret_status = VisitImplicitCastExpr((clang::ImplicitCastExpr *)stmt, node_desc);
            break;
        case clang::Stmt::CharacterLiteralClass:
            ret_status = VisitCharacterLiteral((clang::CharacterLiteral *)stmt, node_desc);
            break;
        case clang::Stmt::ParenExprClass:
            ret_status = VisitParenExpr((clang::ParenExpr *)stmt, node_desc);
            break;
        case clang::Stmt::PredefinedExprClass:
            ret_status = VisitPredefinedExpr((clang::PredefinedExpr *)stmt, node_desc);
            break;
        case clang::Stmt::StmtExprClass:
            ret_status = VisitStmtExpr((clang::StmtExpr *)stmt, node_desc);
            break;
        case clang::Stmt::StringLiteralClass:
            ret_status = VisitStringLiteral((clang::StringLiteral *)stmt, node_desc);
            break;
        case clang::Stmt::UnaryExprOrTypeTraitExprClass:
            ret_status = VisitUnaryExprOrTypeTraitExpr((clang::UnaryExprOrTypeTraitExpr *)stmt, node_desc);
            break;
        case clang::Stmt::ExtVectorElementExprClass:
            ret_status = VisitExtVectorElementExpr((clang::ExtVectorElementExpr *)stmt, node_desc);
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
        case clang::Stmt::DeclStmtClass:
            ret_status = VisitDeclStmt((clang::DeclStmt *)stmt, node_desc);
            break;
        case clang::Stmt::CallExprClass:
            ret_status = VisitCallExpr((clang::CallExpr *)stmt, node_desc);
            break;
        case clang::Stmt::CStyleCastExprClass:
            ret_status = VisitCStyleCastExpr((clang::CStyleCastExpr *)stmt, node_desc);
            break;
        case clang::Stmt::DeclRefExprClass:
            ret_status = VisitDeclRefExpr((clang::DeclRefExpr *)stmt, node_desc);
            break;
        case clang::Stmt::UnaryOperatorClass:
            ret_status = VisitUnaryOperator((clang::UnaryOperator *)stmt, node_desc);
            break;
        case clang::Stmt::VAArgExprClass:
            ret_status = VisitVAArgExpr((clang::VAArgExpr *)stmt, node_desc);
            break;
        case clang::Stmt::ForStmtClass:
            ret_status = VisitForStmt((clang::ForStmt *)stmt, node_desc);
            break;
        case clang::Stmt::IfStmtClass:
            ret_status = VisitIfStmt((clang::IfStmt *)stmt, node_desc);
            break;
        case clang::Stmt::DoStmtClass:
            ret_status = VisitDoStmt((clang::DoStmt *)stmt, node_desc);
            break;
        case clang::Stmt::ReturnStmtClass:
            ret_status = VisitReturnStmt((clang::ReturnStmt *)stmt, node_desc);
            break;
        case clang::Stmt::BinaryOperatorClass:
        case clang::Stmt::CompoundAssignOperatorClass:
            ret_status = VisitBinaryOperator((clang::BinaryOperator *)stmt, node_desc);
            break;
        case clang::Stmt::ConditionalOperatorClass:
            ret_status = VisitConditionalOperator((clang::ConditionalOperator *)stmt, node_desc);
            break;
        case clang::Stmt::ArraySubscriptExprClass:
            ret_status = VisitArraySubscriptExpr((clang::ArraySubscriptExpr *)stmt, node_desc);
            break;
        case clang::Stmt::MemberExprClass:
            ret_status = VisitMemberExpr((clang::MemberExpr *)stmt, node_desc);
            break;
        case clang::Stmt::LabelStmtClass:
            ret_status = VisitLabelStmt((clang::LabelStmt *)stmt, node_desc);
            break;
        case clang::Stmt::NullStmtClass:
            ret_status = VisitNullStmt((clang::NullStmt *)stmt, node_desc);
            break;
        case clang::Stmt::GotoStmtClass:
            ret_status = VisitGotoStmt((clang::GotoStmt *)stmt, node_desc);
            break;
        case clang::Stmt::WhileStmtClass:
            ret_status = VisitWhileStmt((clang::WhileStmt *)stmt, node_desc);
            break;
        case clang::Stmt::CaseStmtClass:
            ret_status = VisitCaseStmt((clang::CaseStmt *)stmt, node_desc);
            break;
        case clang::Stmt::DefaultStmtClass:
            ret_status = VisitDefaultStmt((clang::DefaultStmt *)stmt, node_desc);
            break;
        case clang::Stmt::SwitchStmtClass:
            ret_status = VisitSwitchStmt((clang::SwitchStmt *)stmt, node_desc);
            break;
//        case clang::Stmt::ImplicitValueInitExprClass: break; // FIXME
        // TODO
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
    for (it = compound_stmt->body_begin(); it != compound_stmt->body_end(); it++) {
        *it;
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
        decl_stmt->getSingleDecl();
    }
    else {
        clang::DeclStmt::decl_iterator it;
        for (it = decl_stmt->decl_begin(); it != decl_stmt->decl_end(); it++)
            Traverse(*it);
    }

    return VisitStmt(decl_stmt, node_desc) && res;
}

bool ClangToDot::VisitDoStmt(clang::DoStmt * do_stmt, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("DoStmt");

    do_stmt->getCond();

    do_stmt->getBody();

    return VisitStmt(do_stmt, node_desc) && res;
}

bool ClangToDot::VisitExpr(clang::Expr * expr, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("Expr");

    // TODO

    return VisitStmt(expr, node_desc) && res;
}

bool ClangToDot::VisitConditionalOperator(clang::ConditionalOperator * conditional_operator, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("ConditionalOperator");

    conditional_operator->getCond();

    conditional_operator->getTrueExpr();

    conditional_operator->getFalseExpr();

    return VisitExpr(conditional_operator, node_desc) && res;
}

bool ClangToDot::VisitBinaryOperator(clang::BinaryOperator * binary_operator, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("BinaryOperator");

    binary_operator->getLHS();

    binary_operator->getRHS();

    binary_operator->getOpcodeStr();

    return VisitExpr(binary_operator, node_desc) && res;
}

bool ClangToDot::VisitArraySubscriptExpr(clang::ArraySubscriptExpr * array_subscript_expr, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("ArraySubscriptExpr");

    array_subscript_expr->getBase();

    array_subscript_expr->getIdx();

    return VisitExpr(array_subscript_expr, node_desc) && res;
}

bool ClangToDot::VisitCallExpr(clang::CallExpr * call_expr, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("CallExpr");

    call_expr->getCallee();

    clang::CallExpr::arg_iterator it;
    for (it = call_expr->arg_begin(); it != call_expr->arg_end(); ++it) {
        *it;
    }

    return VisitExpr(call_expr, node_desc) && res;
}

bool ClangToDot::VisitCastExpr(clang::CastExpr * cast, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("CastExpr");

    // TODO check 'name' is set

    cast->getSubExpr();

    return VisitExpr(cast, node_desc) && res;
}

bool ClangToDot::VisitExplicitCastExpr(clang::ExplicitCastExpr * explicit_cast_expr, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("ExplicitCastExpr");

    // TODO check 'name' is set

    explicit_cast_expr->getTypeAsWritten();

    return VisitCastExpr(explicit_cast_expr, node_desc) && res;
}

bool ClangToDot::VisitCStyleCastExpr(clang::CStyleCastExpr * c_style_cast, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("CStyleCastExpr");

    // TODO

    return VisitCastExpr(c_style_cast, node_desc) && res;
}

bool ClangToDot::VisitImplicitCastExpr(clang::ImplicitCastExpr * implicit_cast_expr, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("ImplicitCastExpr");

    // TODO

    return VisitCastExpr(implicit_cast_expr, node_desc) && res;
}

bool ClangToDot::VisitCharacterLiteral(clang::CharacterLiteral * character_literal, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("CharacterLiteral");

    character_literal->getValue();

    return VisitExpr(character_literal, node_desc) && res;
}

bool ClangToDot::VisitCompoundLiteralExpr(clang::CompoundLiteralExpr * compound_literal, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("CompoundLiteralExpr");

    compound_literal->getInitializer();

    compound_literal->getType();

    return VisitExpr(compound_literal, node_desc) && res;
}

bool ClangToDot::VisitDeclRefExpr(clang::DeclRefExpr * decl_ref_expr, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("DeclRefExpr");

    decl_ref_expr->getDecl();

    return VisitExpr(decl_ref_expr, node_desc) && res;
}

bool ClangToDot::VisitDesignatedInitExpr(clang::DesignatedInitExpr * designated_init_expr, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("DesignatedInitExpr");

    designated_init_expr->getInit();

    clang::DesignatedInitExpr::designators_iterator it;
    for (it = designated_init_expr->designators_begin(); it != designated_init_expr->designators_end(); it++) {
        if (it->isFieldDesignator()) {
            it->getField();
        }
        else if (it->isArrayDesignator()) {
            designated_init_expr->getArrayIndex(*it);
        }
        else if (it->isArrayRangeDesignator()) {
            // TODO
        }
        else assert(false);
    }

    return VisitExpr(designated_init_expr, node_desc) && res;
}

bool ClangToDot::VisitExtVectorElementExpr(clang::ExtVectorElementExpr * ext_vector_element_expr, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("ExtVectorElementExpr");

    ext_vector_element_expr->getBase();

    ext_vector_element_expr->getType();

    clang::IdentifierInfo & ident_info = ext_vector_element_expr->getAccessor();
    std::string ident = ident_info.getName().str();

    if (ext_vector_element_expr->isArrow()) {
        // TODO
    }
    else {
        // TODO
    }

   return VisitExpr(ext_vector_element_expr, node_desc) && res;
}

bool ClangToDot::VisitFloatingLiteral(clang::FloatingLiteral * floating_literal, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("FloatingLiteral");

    // FIXME

    unsigned int precision =  llvm::APFloat::semanticsPrecision(floating_literal->getValue().getSemantics());
    if (precision == 24)
        floating_literal->getValue().convertToFloat();
    else if (precision == 53)
        floating_literal->getValue().convertToDouble();
    else
        assert(!"In VisitFloatingLiteral: Unsupported float size");

    return VisitExpr(floating_literal, node_desc) && res;
}

bool ClangToDot::VisitImaginaryLiteral(clang::ImaginaryLiteral * imaginary_literal, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("ImaginaryLiteral");

    imaginary_literal->getSubExpr();

    return VisitExpr(imaginary_literal, node_desc) && res;
}

bool ClangToDot::VisitInitListExpr(clang::InitListExpr * init_list_expr, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("InitListExpr");

    init_list_expr->getSyntacticForm();

    clang::InitListExpr::iterator it;
    for (it = init_list_expr->begin(); it != init_list_expr->end(); it++) {
        *it;
    }

    return VisitExpr(init_list_expr, node_desc) && res;
}

bool ClangToDot::VisitIntegerLiteral(clang::IntegerLiteral * integer_literal, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("IntegerLiteral");

    integer_literal->getValue();

    return VisitExpr(integer_literal, node_desc) && res;
}

bool ClangToDot::VisitMemberExpr(clang::MemberExpr * member_expr, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("MemberExpr");

    member_expr->getBase();

    member_expr->getMemberDecl();

    if (member_expr->isArrow()) {}
    else {}

    return VisitExpr(member_expr, node_desc) && res;
}

bool ClangToDot::VisitParenExpr(clang::ParenExpr * paren_expr, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("ParentExpr");

    paren_expr->getSubExpr();

    return VisitExpr(paren_expr, node_desc) && res;
}

bool ClangToDot::VisitPredefinedExpr(clang::PredefinedExpr * predefined_expr, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("PredefinedExpr");

    switch (predefined_expr->getIdentType()) {
        case clang::PredefinedExpr::Func:
//            name = "__func__";
            break;
        case clang::PredefinedExpr::Function:
//            name = "__FUNCTION__";
            break;
        case clang::PredefinedExpr::PrettyFunction:
//            name = "__PRETTY_FUNCTION__";
            break;
        case clang::PredefinedExpr::PrettyFunctionNoVirtual:
            // TODO
            break;
    }

    return VisitExpr(predefined_expr, node_desc) && res;
}

bool ClangToDot::VisitStmtExpr(clang::StmtExpr * stmt_expr, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("StmtExpr");

    stmt_expr->getSubStmt();

    return VisitExpr(stmt_expr, node_desc) && res;
}

bool ClangToDot::VisitStringLiteral(clang::StringLiteral * string_literal, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("StringLiteral");

    string_literal->getString();

    return VisitExpr(string_literal, node_desc) && res;
}

bool ClangToDot::VisitUnaryExprOrTypeTraitExpr(clang::UnaryExprOrTypeTraitExpr * unary_expr_or_type_trait_expr, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("UnaryExprOrTypeTraitExpr");

    if (unary_expr_or_type_trait_expr->isArgumentType()) {
        unary_expr_or_type_trait_expr->getArgumentType();
    }
    else {
        unary_expr_or_type_trait_expr->getArgumentExpr();
    }

    switch (unary_expr_or_type_trait_expr->getKind()) {
        case clang::UETT_SizeOf:
            break;
        case clang::UETT_AlignOf:
            break;
        case clang::UETT_VecStep:
            break;
    }

    return VisitStmt(unary_expr_or_type_trait_expr, node_desc) && res;
}

bool ClangToDot::VisitUnaryOperator(clang::UnaryOperator * unary_operator, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("UnaryOperator");

    unary_operator->getSubExpr();

    switch (unary_operator->getOpcode()) {
        case clang::UO_PostInc:
            break;
        case clang::UO_PostDec:
            break;
        case clang::UO_PreInc:
            break;
        case clang::UO_PreDec:
            break;
        case clang::UO_AddrOf:
            break;
        case clang::UO_Deref:
            break;
        case clang::UO_Plus:
            break;
        case clang::UO_Minus:
            break;
        case clang::UO_Not:
            break;
        case clang::UO_LNot:
            break;
        case clang::UO_Real:
            break;
        case clang::UO_Imag:
            break;
        case clang::UO_Extension:
            break;
    }

    return VisitExpr(unary_operator, node_desc) && res;
}

bool ClangToDot::VisitVAArgExpr(clang::VAArgExpr * va_arg_expr, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("VAArgExpr");

    va_arg_expr->getSubExpr();

    return VisitExpr(va_arg_expr, node_desc) && res;
}

bool ClangToDot::VisitForStmt(clang::ForStmt * for_stmt, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("ForStmt");

    for_stmt->getInit();

    for_stmt->getCond();

    for_stmt->getInc();

    for_stmt->getBody();

    return VisitStmt(for_stmt, node_desc) && res;
}

bool ClangToDot::VisitGotoStmt(clang::GotoStmt * goto_stmt, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("GotoStmt");

    goto_stmt->getLabel()->getStmt();

    return VisitStmt(goto_stmt, node_desc) && res;
}

bool ClangToDot::VisitIfStmt(clang::IfStmt * if_stmt, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("IfStmt");

    if_stmt->getCond();

    if_stmt->getThen();

    if_stmt->getElse();

    return VisitStmt(if_stmt, node_desc) && res;
}

bool ClangToDot::VisitLabelStmt(clang::LabelStmt * label_stmt, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("LabelStmt");

    label_stmt->getName();

    label_stmt->getSubStmt();

    return VisitStmt(label_stmt, node_desc) && res;
}

bool ClangToDot::VisitNullStmt(clang::NullStmt * null_stmt, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("NullStmt");

    // TODO

    return VisitStmt(null_stmt, node_desc) && res;
}

bool ClangToDot::VisitReturnStmt(clang::ReturnStmt * return_stmt, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("ReturnStmt");

    return_stmt->getRetValue();

    return VisitStmt(return_stmt, node_desc) && res;
}

bool ClangToDot::VisitSwitchCase(clang::SwitchCase * switch_case, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("SwitchCase");

    switch_case->getSubStmt();

    return VisitStmt(switch_case, node_desc) && res;
}

bool ClangToDot::VisitCaseStmt(clang::CaseStmt * case_stmt, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("CaseStmt");

    case_stmt->getLHS();

    case_stmt->getRHS();

    return VisitSwitchCase(case_stmt, node_desc) && res;
}

bool ClangToDot::VisitDefaultStmt(clang::DefaultStmt * default_stmt, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;    

    node_desc.kind_hierarchy.push_back("DefaultStmt");

    // TODO

    return VisitSwitchCase(default_stmt, node_desc) && res;
}

bool ClangToDot::VisitSwitchStmt(clang::SwitchStmt * switch_stmt, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("SwitchStmt");

    switch_stmt->getCond();
    
    switch_stmt->getBody();

    return VisitStmt(switch_stmt, node_desc) && res;
}

bool ClangToDot::VisitWhileStmt(clang::WhileStmt * while_stmt, ClangToDot::NodeDescriptor & node_desc) {
    bool res = true;

    node_desc.kind_hierarchy.push_back("WhileStmt");

    while_stmt->getCond();

    while_stmt->getBody();

    return VisitStmt(while_stmt, node_desc) && res;
}

