

#include "clang-to-dot.hpp"

#include <iostream>
#include <cassert>

#define DEBUG_VISITOR 0
#define DEBUG_SOURCE_LOCATION 0
#define USE_CLANG_HACK 0
#define DEBUG_SYMBOL_TABLE_LOOKUP 0
#define DEBUG_TRAVERSE_DECL 0
#define DEBUG_ARGS 0
#define DEBUG_ENUM_DECL 0
#define DEBUG_VISIT_STMT 0

int main(int argc, char ** argv) {
  // 0 - Analyse Cmd Line

    std::vector<std::string> inc_dirs_list;
    std::vector<std::string> define_list;
    std::vector<std::string> inc_list;
    std::string input_file;

    for (int i = 0; i < argc; i++) {
        std::string current_arg(argv[i]);
        if (current_arg.find("-I") == 0) {
            if (current_arg.length() > 2) {
                inc_dirs_list.push_back(current_arg.substr(2));
            }
            else {
                i++;
                if (i < argc)
                    inc_dirs_list.push_back(current_arg);
                else
                    break;
            }
        }
        else if (current_arg.find("-D") == 0) {
            if (current_arg.length() > 2) {
                define_list.push_back(current_arg.substr(2));
            }
            else {
                i++;
                if (i < argc)
                    define_list.push_back(current_arg);
                else
                    break;
            }
        }
        else if (current_arg.find("-c") == 0) {}
        else if (current_arg.find("-o") == 0) {
            if (current_arg.length() == 2) {
                i++;
                if (i >= argc) break;
            }
        }
        else {
            // TODO -include
#if DEBUG_ARGS
            std::cerr << "argv[" << i << "] = " << current_arg << " is neither define or include dir. Use it as input file."  << std::endl;
#endif
            input_file = current_arg;
        }
    }

    ClangToDot::Language language = ClangToDot::unknown;

    size_t last_period = input_file.find_last_of(".");
    std::string extention(input_file.substr(last_period + 1));

    if (extention == "c") {
        language = ClangToDot::C;
    }
    else if (extention == "C" || extention == "cxx" || extention == "cpp" || extention == "cc") {
        language = ClangToDot::CPLUSPLUS;
    }
    else if (extention == "objc") {
        language = ClangToDot::OBJC;
    }
    else if (extention == "cu") {
        language = ClangToDot::CUDA;
    }
    else if (extention == "ocl" || extention == "cl") {
        language = ClangToDot::OPENCL;
    }

    assert(language != ClangToDot::unknown);

    unsigned cnt = define_list.size() + inc_dirs_list.size() + inc_list.size();
    char ** args = new char*[cnt];
    std::vector<std::string>::iterator it_str;
    unsigned i = 0;
    for (it_str = define_list.begin(); it_str != define_list.end(); it_str++) {
        args[i] = new char[it_str->size() + 3];
        args[i][0] = '-';
        args[i][1] = 'D';
        strcpy(&(args[i][2]), it_str->c_str());
#if DEBUG_ARGS
        std::cerr << "args[" << i << "] = " << args[i] << std::endl;
#endif
        i++;
    }
    for (it_str = inc_dirs_list.begin(); it_str != inc_dirs_list.end(); it_str++) {
        args[i] = new char[it_str->size() + 3];
        args[i][0] = '-';
        args[i][1] = 'I';
        strcpy(&(args[i][2]), it_str->c_str());
#if DEBUG_ARGS
        std::cerr << "args[" << i << "] = " << args[i] << std::endl;
#endif
        i++;
    }
    for (it_str = inc_list.begin(); it_str != inc_list.end(); it_str++) {
        args[i] = new char[it_str->size() + 9];
        args[i][0] = '-'; args[i][1] = 'i'; args[i][2] = 'n'; args[i][3] = 'c';
        args[i][4] = 'l'; args[i][5] = 'u'; args[i][6] = 'd'; args[i][7] = 'e';
        strcpy(&(args[i][8]), it_str->c_str());
#if DEBUG_ARGS
        std::cerr << "args[" << i << "] = " << args[i] << std::endl;
#endif
        i++;
    }


  // 2 - Create a compiler instance

    clang::CompilerInstance * compiler_instance = new clang::CompilerInstance();

    clang::TextDiagnosticPrinter * diag_printer = new clang::TextDiagnosticPrinter(llvm::errs(), clang::DiagnosticOptions());
    compiler_instance->createDiagnostics(argc, argv, diag_printer, true, false);

    clang::CompilerInvocation * invocation = new clang::CompilerInvocation();
    clang::CompilerInvocation::CreateFromArgs(*invocation, args, &(args[cnt]), compiler_instance->getDiagnostics());
    compiler_instance->setInvocation(invocation);

    clang::LangOptions & lang_opts = compiler_instance->getLangOpts();

    switch (language) {
        case ClangToDot::C:
//          compiler_instance->getInvocation().setLangDefaults(lang_opts, clang::IK_C, );
            break;
        case ClangToDot::CPLUSPLUS:
            lang_opts.CPlusPlus = 1;
//          compiler_instance->getInvocation().setLangDefaults(lang_opts, clang::IK_CXX, );
            break;
        case ClangToDot::CUDA:
            lang_opts.CUDA = 1;
//          lang_opts.CPlusPlus = 1;
//          compiler_instance->getInvocation().setLangDefaults(lang_opts, clang::IK_CUDA,   clang::LangStandard::lang_cuda);
            break;
        case ClangToDot::OPENCL:
            lang_opts.OpenCL = 1;
//          compiler_instance->getInvocation().setLangDefaults(lang_opts, clang::IK_OpenCL, clang::LangStandard::lang_opencl);
            break;
        case ClangToDot::OBJC:
            assert(!"Objective-C is not supported by ROSE Compiler.");
//          compiler_instance->getInvocation().setLangDefaults(lang_opts, clang::IK_, );
        default:
            assert(false);
    }

    clang::TargetOptions target_options;
    target_options.Triple = llvm::sys::getHostTriple();
    clang::TargetInfo * target_info = clang::TargetInfo::CreateTargetInfo(compiler_instance->getDiagnostics(), target_options);
    compiler_instance->setTarget(target_info);

    compiler_instance->createFileManager();
    compiler_instance->createSourceManager(compiler_instance->getFileManager());

    const clang::FileEntry * input_file_entry = compiler_instance->getFileManager().getFile(input_file);
    compiler_instance->getSourceManager().createMainFileID(input_file_entry);

    if (!compiler_instance->hasPreprocessor()) compiler_instance->createPreprocessor();

    if (!compiler_instance->hasASTContext()) compiler_instance->createASTContext();

    ClangToDot translator(compiler_instance, language);
    compiler_instance->setASTConsumer(&translator);   

    if (!compiler_instance->hasSema()) compiler_instance->createSema(clang::TU_Complete, NULL);

    assert (compiler_instance->hasDiagnostics());
    assert (compiler_instance->hasTarget());
    assert (compiler_instance->hasFileManager());
    assert (compiler_instance->hasSourceManager());
    assert (compiler_instance->hasPreprocessor());
    assert (compiler_instance->hasASTContext());
    assert (compiler_instance->hasSema());

  // 3 - Translate

    compiler_instance->getDiagnosticClient().BeginSourceFile(compiler_instance->getLangOpts(), &(compiler_instance->getPreprocessor()));
    clang::ParseAST(compiler_instance->getPreprocessor(), &translator, compiler_instance->getASTContext());
    compiler_instance->getDiagnosticClient().EndSourceFile();

    return 0;
}

ClangToDot::ClangToDot(clang::CompilerInstance * compiler_instance, Language language_) :
    clang::ASTConsumer(),
    language(language_)
{}

ClangToDot::~ClangToDot() {}

/* (protected) Helper methods */

/* Overload of ASTConsumer::HandleTranslationUnit, it is the "entry point" */

void ClangToDot::HandleTranslationUnit(clang::ASTContext & ast_context) {
    Traverse(ast_context.getTranslationUnitDecl());
}

/* Traverse({Decl|Stmt|Type} *) methods */

std::string ClangToDot::Traverse(clang::Decl * decl) {
    if (decl == NULL)
        return NULL;

    std::map<clang::Decl *, std::string>::iterator it = p_decl_translation_map.find(decl);
    if (it != p_decl_translation_map.end()) {
#if DEBUG_TRAVERSE_DECL
        std::cerr << "Traverse Decl : " << decl << " ";
        if (clang::NamedDecl::classof(decl)) {
            std::cerr << ": " << ((clang::NamedDecl *)decl)->getNameAsString() << ") ";
        }
        std::cerr << " already visited : node = " << it->second << std::endl;
#endif
        return it->second;
    }

    std::string result;
    bool ret_status = false;

    switch (decl->getKind()) {
        case clang::Decl::TranslationUnit:
            ret_status = VisitTranslationUnitDecl((clang::TranslationUnitDecl *)decl, result);
            break;
        case clang::Decl::Typedef:
            ret_status = VisitTypedefDecl((clang::TypedefDecl *)decl, result);
            break;
        case clang::Decl::Var:
            ret_status = VisitVarDecl((clang::VarDecl *)decl, result);
            break;
        case clang::Decl::Function:
            ret_status = VisitFunctionDecl((clang::FunctionDecl *)decl, result);
            break;
        case clang::Decl::ParmVar:
            ret_status = VisitParmVarDecl((clang::ParmVarDecl *)decl, result);
            break;
        case clang::Decl::CXXRecord:
          ret_status = VisitCXXRecordDecl((clang::CXXRecordDecl *)decl, result);
          break;
        case clang::Decl::Record:
            ret_status = VisitRecordDecl((clang::RecordDecl *)decl, result);
            break;
        case clang::Decl::Field:
            ret_status = VisitFieldDecl((clang::FieldDecl *)decl, result);
            break;
        case clang::Decl::Enum:
            ret_status = VisitEnumDecl((clang::EnumDecl *)decl, result);
            break;
        case clang::Decl::EnumConstant:
            ret_status = VisitEnumConstantDecl((clang::EnumConstantDecl *)decl, result);
            break;
        // TODO cases
        default:
            std::cerr << "Unknown declacaration kind: " << decl->getDeclKindName() << " !" << std::endl;
            assert(false);
    }

    assert(ret_status != false);
    assert(result != "");

    p_decl_translation_map.insert(std::pair<clang::Decl *, std::string>(decl, result));

#if DEBUG_TRAVERSE_DECL
    std::cerr << "Traverse(clang::Decl : " << decl << " ";
    if (clang::NamedDecl::classof(decl)) {
        std::cerr << ": " << ((clang::NamedDecl *)decl)->getNameAsString() << ") ";
    }
    std::cerr << " visit done : node = " << result << std::endl;
#endif

    return result;
}

std::string ClangToDot::Traverse(clang::Stmt * stmt) {
    if (stmt == NULL)
        return NULL;

    std::map<clang::Stmt *, std::string>::iterator it = p_stmt_translation_map.find(stmt);
    if (it != p_stmt_translation_map.end())
        return it->second; 

    std::string result;
    bool ret_status = false;

    switch (stmt->getStmtClass()) {
        case clang::Stmt::InitListExprClass:
            ret_status = VisitInitListExpr((clang::InitListExpr *)stmt, result);
            break;
        case clang::Stmt::DesignatedInitExprClass:
            ret_status = VisitDesignatedInitExpr((clang::DesignatedInitExpr *)stmt, result);
            break;
        case clang::Stmt::IntegerLiteralClass:
            ret_status = VisitIntegerLiteral((clang::IntegerLiteral *)stmt, result);
            break;
        case clang::Stmt::FloatingLiteralClass:
            ret_status = VisitFloatingLiteral((clang::FloatingLiteral *)stmt, result);
            break;
        case clang::Stmt::ImaginaryLiteralClass:
            ret_status = VisitImaginaryLiteral((clang::ImaginaryLiteral *)stmt, result);
            break;
        case clang::Stmt::CompoundLiteralExprClass:
            ret_status = VisitCompoundLiteralExpr((clang::CompoundLiteralExpr *)stmt, result);
            break;
        case clang::Stmt::ImplicitCastExprClass:
            ret_status = VisitImplicitCastExpr((clang::ImplicitCastExpr *)stmt, result);
            break;
        case clang::Stmt::CharacterLiteralClass:
            ret_status = VisitCharacterLiteral((clang::CharacterLiteral *)stmt, result);
            break;
        case clang::Stmt::ParenExprClass:
            ret_status = VisitParenExpr((clang::ParenExpr *)stmt, result);
            break;
        case clang::Stmt::PredefinedExprClass:
            ret_status = VisitPredefinedExpr((clang::PredefinedExpr *)stmt, result);
            break;
        case clang::Stmt::StmtExprClass:
            ret_status = VisitStmtExpr((clang::StmtExpr *)stmt, result);
            break;
        case clang::Stmt::StringLiteralClass:
            ret_status = VisitStringLiteral((clang::StringLiteral *)stmt, result);
            break;
        case clang::Stmt::UnaryExprOrTypeTraitExprClass:
            ret_status = VisitUnaryExprOrTypeTraitExpr((clang::UnaryExprOrTypeTraitExpr *)stmt, result);
            break;
        case clang::Stmt::ExtVectorElementExprClass:
            ret_status = VisitExtVectorElementExpr((clang::ExtVectorElementExpr *)stmt, result);
            break;
        case clang::Stmt::BreakStmtClass:
            ret_status = VisitBreakStmt((clang::BreakStmt *)stmt, result);
            break;
        case clang::Stmt::CompoundStmtClass:
            ret_status = VisitCompoundStmt((clang::CompoundStmt *)stmt, result);
            break;
        case clang::Stmt::ContinueStmtClass:
            ret_status = VisitContinueStmt((clang::ContinueStmt *)stmt, result);
            break;
        case clang::Stmt::DeclStmtClass:
            ret_status = VisitDeclStmt((clang::DeclStmt *)stmt, result);
            break;
        case clang::Stmt::CallExprClass:
            ret_status = VisitCallExpr((clang::CallExpr *)stmt, result);
            break;
        case clang::Stmt::CStyleCastExprClass:
            ret_status = VisitCStyleCastExpr((clang::CStyleCastExpr *)stmt, result);
            break;
        case clang::Stmt::DeclRefExprClass:
            ret_status = VisitDeclRefExpr((clang::DeclRefExpr *)stmt, result);
            break;
        case clang::Stmt::UnaryOperatorClass:
            ret_status = VisitUnaryOperator((clang::UnaryOperator *)stmt, result);
            break;
        case clang::Stmt::VAArgExprClass:
            ret_status = VisitVAArgExpr((clang::VAArgExpr *)stmt, result);
            break;
        case clang::Stmt::ForStmtClass:
            ret_status = VisitForStmt((clang::ForStmt *)stmt, result);
            break;
        case clang::Stmt::IfStmtClass:
            ret_status = VisitIfStmt((clang::IfStmt *)stmt, result);
            break;
        case clang::Stmt::DoStmtClass:
            ret_status = VisitDoStmt((clang::DoStmt *)stmt, result);
            break;
        case clang::Stmt::ReturnStmtClass:
            ret_status = VisitReturnStmt((clang::ReturnStmt *)stmt, result);
            break;
        case clang::Stmt::BinaryOperatorClass:
        case clang::Stmt::CompoundAssignOperatorClass:
            ret_status = VisitBinaryOperator((clang::BinaryOperator *)stmt, result);
            break;
        case clang::Stmt::ConditionalOperatorClass:
            ret_status = VisitConditionalOperator((clang::ConditionalOperator *)stmt, result);
            break;
        case clang::Stmt::ArraySubscriptExprClass:
            ret_status = VisitArraySubscriptExpr((clang::ArraySubscriptExpr *)stmt, result);
            break;
        case clang::Stmt::MemberExprClass:
            ret_status = VisitMemberExpr((clang::MemberExpr *)stmt, result);
            break;
        case clang::Stmt::LabelStmtClass:
            ret_status = VisitLabelStmt((clang::LabelStmt *)stmt, result);
            break;
        case clang::Stmt::NullStmtClass:
            ret_status = VisitNullStmt((clang::NullStmt *)stmt, result);
            break;
        case clang::Stmt::GotoStmtClass:
            ret_status = VisitGotoStmt((clang::GotoStmt *)stmt, result);
            break;
        case clang::Stmt::WhileStmtClass:
            ret_status = VisitWhileStmt((clang::WhileStmt *)stmt, result);
            break;
        case clang::Stmt::CaseStmtClass:
            ret_status = VisitCaseStmt((clang::CaseStmt *)stmt, result);
            break;
        case clang::Stmt::DefaultStmtClass:
            ret_status = VisitDefaultStmt((clang::DefaultStmt *)stmt, result);
            break;
        case clang::Stmt::SwitchStmtClass:
            ret_status = VisitSwitchStmt((clang::SwitchStmt *)stmt, result);
            break;
//        case clang::Stmt::ImplicitValueInitExprClass: break; // FIXME
        // TODO
        default:
            std::cerr << "Unknown statement kind: " << stmt->getStmtClassName() << " !" << std::endl;
            assert(false);
    }

    assert(ret_status != false);
    assert(result != "");

    p_stmt_translation_map.insert(std::pair<clang::Stmt *, std::string>(stmt, result));

    return result;
}

void ClangToDot::Traverse(const clang::Type * type) {
    if (type == NULL)
        return NULL;

    std::map<const clang::Type *, std::string>::iterator it = p_type_translation_map.find(type);
    if (it != p_type_translation_map.end()) 
         return it->second;

    std::string result = NULL;
    bool ret_status = false;

    switch (type->getTypeClass()) {
        case clang::Type::Builtin:
            ret_status = VisitBuiltinType((clang::BuiltinType *)type, result);
            break;
        case clang::Type::Complex:
            ret_status = VisitComplexType((clang::ComplexType *)type, result);
            break;
        case clang::Type::Pointer:
            ret_status = VisitPointerType((clang::PointerType *)type, result);
            break;
        case clang::Type::ConstantArray:
            ret_status = VisitConstantArrayType((clang::ConstantArrayType *)type, result);
            break;
       case clang::Type::IncompleteArray:
            ret_status = VisitIncompleteArrayType((clang::IncompleteArrayType *)type, result);
            break;
        case clang::Type::Paren:
            ret_status = VisitParenType((clang::ParenType *)type, result);
            break;
        case clang::Type::FunctionProto:
            ret_status = VisitFunctionProtoType((clang::FunctionProtoType *)type, result);
            break;
        case clang::Type::FunctionNoProto:
            ret_status = VisitFunctionNoProtoType((clang::FunctionNoProtoType *)type, result);
            break;
        case clang::Type::Elaborated:
            ret_status = VisitElaboratedType((clang::ElaboratedType *)type, result);
            break;
        case clang::Type::Record:
            ret_status = VisitRecordType((clang::RecordType *)type, result);
            break;
        case clang::Type::Enum:
            ret_status = VisitEnumType((clang::EnumType *)type, result);
            break;
        case clang::Type::Typedef:
            ret_status = VisitTypedefType((clang::TypedefType *)type, result);
            break;
        case clang::Type::ExtVector:
        case clang::Type::Vector:
            ret_status = VisitVectorType((clang::VectorType *)type, result);
            break;
        // TODO cases
        default:
            std::cerr << "Unknown type kind " << type->getTypeClassName() << " !" << std::endl;
            assert(false);
    }

    assert(ret_status != false);
    assert(result != "");

    p_type_translation_map.insert(std::pair<const clang::Type *, std::string>(type, result));

    return result;
}

void ClangToDot::TraverseForDeclContext(clang::DeclContext * decl_context) {
    return Traverse((clang::Decl*)decl_context);
}

/**********************/
/* Visit Declarations */
/**********************/

bool ClangToDot::VisitDecl(clang::Decl * decl, std::string & name) {
    // TODO

    return true;
}

bool ClangToDot::VisitRecordDecl(clang::RecordDecl * record_decl, std::string & name) {
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

    return VisitDecl(record_decl, name) && res;
}

bool ClangToDot::VisitCXXRecordDecl(clang::CXXRecordDecl * cxx_record_decl, std::string & name) {
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

    clang::CXXRecordDecl::vbase_class_iterator it_vbase;
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

    CXXDestructorDecl * destructor = cxx_record_decl->getDestructor();

    return VisitDecl(cxx_record_decl, name) && res;
}

bool ClangToDot::VisitEnumDecl(clang::EnumDecl * enum_decl, std::string & name) {
    bool res = true;

    enum_decl->getNameAsString();

    clang::EnumDecl * prev_enum_decl = enum_decl->getPreviousDeclaration();

    clang::EnumDecl::enumerator_iterator it;
    for (it = enum_decl->enumerator_begin(); it != enum_decl->enumerator_end(); it++) {
        // TODO
    }

    return VisitDecl(enum_decl, name) && res;
}

bool ClangToDot::VisitTypedefDecl(clang::TypedefDecl * typedef_decl, std::string & name) {
    bool res = true;

    typedef_decl->getNameAsString();

    typedef_decl->getUnderlyingType();

    return VisitDecl(typedef_decl, name) && res;
}

bool ClangToDot::VisitFieldDecl(clang::FieldDecl * field_decl, std::string & name) {
    bool res = true;
    
    field_decl->getNameAsString();

    field_decl->getType();

    clang::Expr * init_expr = field_decl->getInClassInitializer();

    return VisitDecl(field_decl, name) && res; 
}

bool ClangToDot::VisitFunctionDecl(clang::FunctionDecl * function_decl, std::string & name) {
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

    return VisitDecl(function_decl, name) && res;
}

bool ClangToDot::VisitVarDecl(clang::VarDecl * var_decl, std::string & name) {
    bool res = true;

    var_decl->getNameAsString();

    var_decl->getType();

    clang::Expr * init_expr = var_decl->getInit();

    return VisitDecl(var_decl, name) && res;
}

bool ClangToDot::VisitParmVarDecl(clang::ParmVarDecl * param_var_decl, std::string & name) {
    bool res = true;

    param_var_decl->getNameAsString();

    param_var_decl->getOriginalType();

    if (param_var_decl->hasDefaultArg()) {
        param_var_decl->getDefaultArg();
    }

    return VisitDecl(param_var_decl, name) && res;
}

bool  ClangToDot::VisitEnumConstantDecl(clang::EnumConstantDecl * enum_constant_decl, std::string & name) {
    bool res = true;

    enum_constant_decl->getNameAsString();

    enum_constant_decl->getType();

    if (enum_constant_decl->getInitExpr() != NULL) {
        enum_constant_decl->getInitExpr();
    }

    return VisitDecl(enum_constant_decl, name) && res;
}

bool ClangToDot::VisitTranslationUnitDecl(clang::TranslationUnitDecl * translation_unit_decl, std::string & name) {
    bool res = true;

    clang::DeclContext::decl_iterator it;
    for (it = translation_unit_decl->decls_begin(); it != translation_unit_decl->decls_end(); it++) {
        *it;
    }

    return VisitDecl(translation_unit_decl, name) && res;
}

/********************/
/* Visit Statements */
/********************/

bool ClangToDot::VisitStmt(clang::Stmt * stmt) {
    if (*node == NULL) {
        std::cerr << "Runtime error: No Sage node associated with the Statement..." << std::endl;
        return false;
    }

    // TODO Is there anything else todo?

    if (
        isSgLocatedNode(*node) != NULL &&
        (
            isSgLocatedNode(*node)->get_file_info() == NULL ||
            !(isSgLocatedNode(*node)->get_file_info()->isCompilerGenerated())
        )
    ) {
        applySourceRange(*node, stmt->getSourceRange());
    }

    return true;
}

bool ClangToDot::VisitBreakStmt(clang::BreakStmt * break_stmt) {
    *node = SageBuilder::buildBreakStmt();
    return VisitStmt(break_stmt, node);
}

bool ClangToDot::VisitCompoundStmt(clang::CompoundStmt * compound_stmt) {
#if DEBUG_VISIT_STMT
    std::cerr << "ClangToDot::VisitCompoundStmt" << std::endl;
#endif

    bool res = true;

    SgBasicBlock * block = SageBuilder::buildBasicBlock();

    block->set_parent(SageBuilder::topScopeStack());

    SageBuilder::pushScopeStack(block);

    clang::CompoundStmt::body_iterator it;
    for (it = compound_stmt->body_begin(); it != compound_stmt->body_end(); it++) {
        SgNode * tmp_node = Traverse(*it);

#if DEBUG_VISIT_STMT
        if (tmp_node != NULL)
          std::cerr << "In VisitCompoundStmt : child is " << tmp_node->class_name() << std::endl;
        else
          std::cerr << "In VisitCompoundStmt : child is NULL" << std::endl;
#endif

        SgClassDeclaration * class_decl = isSgClassDeclaration(tmp_node);
        if (class_decl != NULL && (class_decl->get_name() == "" || class_decl->get_isUnNamed())) continue;
        SgEnumDeclaration * enum_decl = isSgEnumDeclaration(tmp_node);
        if (enum_decl != NULL && (enum_decl->get_name() == "" || enum_decl->get_isUnNamed())) continue;
#if DEBUG_VISIT_STMT
        else if (enum_decl != NULL)
          std::cerr << "enum_decl = " << enum_decl << " >> name: " << enum_decl->get_name() << std::endl;
#endif

        SgStatement * stmt  = isSgStatement(tmp_node);
        SgExpression * expr = isSgExpression(tmp_node);
        if (tmp_node != NULL && stmt == NULL && expr == NULL) {
            std::cerr << "Runtime error: tmp_node != NULL && stmt == NULL && expr == NULL" << std::endl;
            res = false;
        }
        else if (stmt != NULL) {
            block->append_statement(stmt);
        }
        else if (expr != NULL) {
            SgExprStatement * expr_stmt = SageBuilder::buildExprStatement(expr);
            block->append_statement(expr_stmt);
        }
    }

    SageBuilder::popScopeStack();

    *node = block;

    return VisitStmt(compound_stmt, node) && res;
}

bool ClangToDot::VisitContinueStmt(clang::ContinueStmt * continue_stmt) {
    *node = SageBuilder::buildContinueStmt();
    return VisitStmt(continue_stmt, node);
}

bool ClangToDot::VisitDeclStmt(clang::DeclStmt * decl_stmt) {
    bool res = true;

    if (decl_stmt->isSingleDecl()) {
        *node = Traverse(decl_stmt->getSingleDecl());
    }
    else {
        std::vector<SgNode *> tmp_decls;
        SgDeclarationStatement * decl;
        clang::DeclStmt::decl_iterator it;

        SgScopeStatement * scope = SageBuilder::topScopeStack();

        for (it = decl_stmt->decl_begin(); it != decl_stmt->decl_end(); it++)
            tmp_decls.push_back(Traverse(*it));
        for (unsigned i = 0; i < tmp_decls.size() - 1; i++) {
            decl = isSgDeclarationStatement(tmp_decls[i]);
            if (tmp_decls[i] != NULL && decl == NULL) {
                std::cerr << "Runtime error: tmp_decls[i] != NULL && decl == NULL" << std::endl;
                res = false;
                continue;
            }
            else {
              SgClassDeclaration * class_decl = isSgClassDeclaration(decl);
              if (class_decl != NULL && (class_decl->get_name() == "" || class_decl->get_isUnNamed())) continue;
              SgEnumDeclaration * enum_decl = isSgEnumDeclaration(decl);
              if (enum_decl != NULL && (enum_decl->get_name() == "" || enum_decl->get_isUnNamed())) continue;
            }
            scope->append_statement(decl);
            decl->set_parent(scope);
        }
        decl = isSgDeclarationStatement(tmp_decls[tmp_decls.size() - 1]);
        if (tmp_decls[tmp_decls.size() - 1] != NULL && decl == NULL) {
            std::cerr << "Runtime error: tmp_decls[tmp_decls.size() - 1] != NULL && decl == NULL" << std::endl;
            res = false;
        }
        *node = decl;
    }

    return res;
}

bool ClangToDot::VisitDoStmt(clang::DoStmt * do_stmt) {
    SgNode * tmp_cond = Traverse(do_stmt->getCond());
    SgExpression * cond = isSgExpression(tmp_cond);
    ROSE_ASSERT(cond != NULL);

    SgStatement * expr_stmt = SageBuilder::buildExprStatement(cond);

    ROSE_ASSERT(expr_stmt != NULL);

    SgDoWhileStmt * sg_do_stmt = SageBuilder::buildDoWhileStmt_nfi(expr_stmt, NULL);

    sg_do_stmt->set_condition(expr_stmt);

    cond->set_parent(expr_stmt);
    expr_stmt->set_parent(sg_do_stmt);

    SageBuilder::pushScopeStack(sg_do_stmt);

    SgNode * tmp_body = Traverse(do_stmt->getBody());
    SgStatement * body = isSgStatement(tmp_body);
    SgExpression * expr = isSgExpression(tmp_body);
    if (expr != NULL) {
        body =  SageBuilder::buildExprStatement(expr);
        applySourceRange(body, do_stmt->getBody()->getSourceRange());
    }
    ROSE_ASSERT(body != NULL);

    body->set_parent(sg_do_stmt);

    SageBuilder::popScopeStack();

    sg_do_stmt->set_body(body);

    *node = sg_do_stmt;

    return VisitStmt(do_stmt, node); 
}

bool ClangToDot::VisitExpr(clang::Expr * expr) {
     // TODO Is there anything to be done? (maybe in relation with typing?)

     return VisitStmt(expr, node);
}

bool ClangToDot::VisitConditionalOperator(clang::ConditionalOperator * conditional_operator) {
    bool res = true;

    SgNode * tmp_cond  = Traverse(conditional_operator->getCond());
    SgExpression * cond_expr = isSgExpression(tmp_cond);
    ROSE_ASSERT(cond_expr);
    SgNode * tmp_true  = Traverse(conditional_operator->getTrueExpr());
    SgExpression * true_expr = isSgExpression(tmp_true);
    ROSE_ASSERT(true_expr);
    SgNode * tmp_false = Traverse(conditional_operator->getFalseExpr());
    SgExpression * false_expr = isSgExpression(tmp_false);
    ROSE_ASSERT(false_expr);

    *node = SageBuilder::buildConditionalExp(cond_expr, true_expr, false_expr);

    return VisitExpr(conditional_operator, node) && res;
}

bool ClangToDot::VisitBinaryOperator(clang::BinaryOperator * binary_operator) {
    bool res = true;

    SgNode * tmp_lhs = Traverse(binary_operator->getLHS());
    SgExpression * lhs = isSgExpression(tmp_lhs);
    if (tmp_lhs != NULL && lhs == NULL) {
        std::cerr << "Runtime error: tmp_lhs != NULL && lhs == NULL" << std::endl;
        res = false;
    }

    SgNode * tmp_rhs = Traverse(binary_operator->getRHS());
    SgExpression * rhs = isSgExpression(tmp_rhs);
    if (tmp_rhs != NULL && rhs == NULL) {
        std::cerr << "Runtime error: tmp_rhs != NULL && rhs == NULL" << std::endl;
        res = false;
    }

    switch (binary_operator->getOpcode()) {
        case clang::BO_PtrMemD:   ROSE_ASSERT(!"clang::BO_PtrMemD:");//*node = SageBuilder::build(lhs, rhs); break;
        case clang::BO_PtrMemI:   ROSE_ASSERT(!"clang::BO_PtrMemI:");//*node = SageBuilder::build(lhs, rhs); break;
        case clang::BO_Mul:       *node = SageBuilder::buildMultiplyOp(lhs, rhs); break;
        case clang::BO_Div:       *node = SageBuilder::buildDivideOp(lhs, rhs); break;
        case clang::BO_Rem:       *node = SageBuilder::buildModOp(lhs, rhs); break;
        case clang::BO_Add:       *node = SageBuilder::buildAddOp(lhs, rhs); break;
        case clang::BO_Sub:       *node = SageBuilder::buildSubtractOp(lhs, rhs); break;
        case clang::BO_Shl:       *node = SageBuilder::buildLshiftOp(lhs, rhs); break;
        case clang::BO_Shr:       *node = SageBuilder::buildRshiftOp(lhs, rhs); break;
        case clang::BO_LT:        *node = SageBuilder::buildLessThanOp(lhs, rhs); break;
        case clang::BO_GT:        *node = SageBuilder::buildGreaterThanOp(lhs, rhs); break;
        case clang::BO_LE:        *node = SageBuilder::buildLessOrEqualOp(lhs, rhs); break;
        case clang::BO_GE:        *node = SageBuilder::buildGreaterOrEqualOp(lhs, rhs); break;
        case clang::BO_EQ:        *node = SageBuilder::buildEqualityOp(lhs, rhs); break;
        case clang::BO_NE:        *node = SageBuilder::buildNotEqualOp(lhs, rhs); break;
        case clang::BO_And:       *node = SageBuilder::buildBitAndOp(lhs, rhs); break;
        case clang::BO_Xor:       *node = SageBuilder::buildBitXorOp(lhs, rhs); break;
        case clang::BO_Or:        *node = SageBuilder::buildBitOrOp(lhs, rhs); break;
        case clang::BO_LAnd:      *node = SageBuilder::buildAndOp(lhs, rhs); break;
        case clang::BO_LOr:       *node = SageBuilder::buildOrOp(lhs, rhs); break;
        case clang::BO_Assign:    *node = SageBuilder::buildAssignOp(lhs, rhs); break;
        case clang::BO_MulAssign: *node = SageBuilder::buildMultAssignOp(lhs, rhs); break;
        case clang::BO_DivAssign: *node = SageBuilder::buildDivAssignOp(lhs, rhs); break;
        case clang::BO_RemAssign: *node = SageBuilder::buildModAssignOp(lhs, rhs); break;
        case clang::BO_AddAssign: *node = SageBuilder::buildPlusAssignOp(lhs, rhs); break;
        case clang::BO_SubAssign: *node = SageBuilder::buildMinusAssignOp(lhs, rhs); break;
        case clang::BO_ShlAssign: *node = SageBuilder::buildLshiftAssignOp(lhs, rhs); break;
        case clang::BO_ShrAssign: *node = SageBuilder::buildRshiftAssignOp(lhs, rhs); break;
        case clang::BO_AndAssign: *node = SageBuilder::buildAndAssignOp(lhs, rhs); break;
        case clang::BO_XorAssign: *node = SageBuilder::buildXorAssignOp(lhs, rhs); break;
        case clang::BO_OrAssign:  *node = SageBuilder::buildIorAssignOp(lhs, rhs); break;
        case clang::BO_Comma:     *node = SageBuilder::buildCommaOpExp(lhs, rhs); break;
        default:
            std::cerr << "Unknown opcode for binary operator: " << binary_operator->getOpcodeStr() << std::endl;
            res = false;
    }

    return VisitExpr(binary_operator, node) && res;
}

bool ClangToDot::VisitArraySubscriptExpr(clang::ArraySubscriptExpr * array_subscript_expr) {
    bool res = true;

    SgNode * tmp_base = Traverse(array_subscript_expr->getBase());
    SgExpression * base = isSgExpression(tmp_base);
    if (tmp_base != NULL && base == NULL) {
        std::cerr << "Runtime error: tmp_base != NULL && base == NULL" << std::endl;
        res = false;
    }

    SgNode * tmp_idx = Traverse(array_subscript_expr->getIdx());
    SgExpression * idx = isSgExpression(tmp_idx);
    if (tmp_idx != NULL && idx == NULL) {
        std::cerr << "Runtime error: tmp_idx != NULL && idx == NULL" << std::endl;
        res = false;
    }

    *node = SageBuilder::buildPntrArrRefExp(base, idx);

    return VisitExpr(array_subscript_expr, node) && res;
}

bool ClangToDot::VisitCallExpr(clang::CallExpr * call_expr) {
    bool res = true;

    SgNode * tmp_expr = Traverse(call_expr->getCallee());
    SgExpression * expr = isSgExpression(tmp_expr);
    if (tmp_expr != NULL && expr == NULL) {
        std::cerr << "Runtime error: tmp_expr != NULL && expr == NULLL" << std::endl;
        res = false;
    }

    SgExprListExp * param_list = SageBuilder::buildExprListExp_nfi();
        applySourceRange(param_list, call_expr->getSourceRange());

    clang::CallExpr::arg_iterator it;
    for (it = call_expr->arg_begin(); it != call_expr->arg_end(); ++it) {
        SgNode * tmp_expr = Traverse(*it);
        SgExpression * expr = isSgExpression(tmp_expr);
        if (tmp_expr != NULL && expr == NULL) {
            std::cerr << "Runtime error: tmp_expr != NULL && expr == NULL" << std::endl;
            res = false;
            continue;
        }
        param_list->append_expression(expr);
    }

    *node = SageBuilder::buildFunctionCallExp_nfi(expr, param_list);

    return VisitExpr(call_expr, node) && res;
}

bool ClangToDot::VisitCStyleCastExpr(clang::CStyleCastExpr * c_style_cast) {
    bool res = true;

    SgNode * tmp_expr = Traverse(c_style_cast->getSubExpr());
    SgExpression * expr = isSgExpression(tmp_expr);

    ROSE_ASSERT(expr);

    SgType * type = buildTypeFromQualifiedType(c_style_cast->getTypeAsWritten());

    *node = SageBuilder::buildCastExp(expr, type, SgCastExp::e_C_style_cast);

    return VisitExpr(c_style_cast, node) && res;
}

bool ClangToDot::VisitImplicitCastExpr(clang::ImplicitCastExpr * implicit_cast_expr) {
    SgNode * tmp_expr = Traverse(implicit_cast_expr->getSubExpr());
    SgExpression * expr = isSgExpression(tmp_expr);
    
    ROSE_ASSERT(expr != NULL);
/*
    FIXME why not? I dont remember why i commented it... :)

    SgType * type = buildTypeFromQualifiedType(implicit_cast_expr->getType());
    SgCastExp * res = SageBuilder::buildCastExp(expr, type);
    setCompilerGeneratedFileInfo(res);

    *node = res;
*/

    *node = expr;

    return VisitExpr(implicit_cast_expr, node);
}

bool ClangToDot::VisitCharacterLiteral(clang::CharacterLiteral * character_literal) {
    *node = SageBuilder::buildCharVal(character_literal->getValue());

    return VisitExpr(character_literal, node);
}

bool ClangToDot::VisitCompoundLiteralExpr(clang::CompoundLiteralExpr * compound_literal) {
    SgNode * tmp_node = Traverse(compound_literal->getInitializer());
    SgExprListExp * expr = isSgExprListExp(tmp_node);

    ROSE_ASSERT(expr != NULL);

    SgType * type = buildTypeFromQualifiedType(compound_literal->getType());

    ROSE_ASSERT(type != NULL);

    *node = SageBuilder::buildCompoundInitializer_nfi(expr, type);

    return VisitExpr(compound_literal, node);
}

bool ClangToDot::VisitDeclRefExpr(clang::DeclRefExpr * decl_ref_expr) {
    bool res = true;

    //SgNode * tmp_node = Traverse(decl_ref_expr->getDecl());
    // DONE: Do not use Traverse(...) as the declaration can not be complete (recursive functions)
    //       Instead use SymbolTable from ROSE as the symbol should be ready (cannot have a reference before the declaration)
    // FIXME: This fix will not work for C++ (methods/fields can be use before they are declared...)
    // FIXME: I feel like it could work now, we will see ....

    SgSymbol * sym = GetSymbolFromSymbolTable(decl_ref_expr->getDecl());

    if (sym == NULL) {
        SgNode * tmp_decl = Traverse(decl_ref_expr->getDecl());

        if (tmp_decl != NULL) {
            sym = GetSymbolFromSymbolTable(decl_ref_expr->getDecl());
        }
        // FIXME hack Traverse have added the symbol but we cannot find it (probably: problem with type and function lookup)
        if (sym == NULL && isSgFunctionDeclaration(tmp_decl) != NULL) {
            sym = new SgFunctionSymbol(isSgFunctionDeclaration(tmp_decl));
            sym->set_parent(tmp_decl);
        }
        
    }

    if (sym != NULL) { // Not else if it was NULL we have try to traverse it....
        SgVariableSymbol  * var_sym  = isSgVariableSymbol(sym);
        SgFunctionSymbol  * func_sym = isSgFunctionSymbol(sym);
        SgEnumFieldSymbol * enum_sym = isSgEnumFieldSymbol(sym);

        if (var_sym != NULL) {
            *node = SageBuilder::buildVarRefExp(var_sym);
        }
        else if (func_sym != NULL) {
            *node = SageBuilder::buildFunctionRefExp(func_sym);
        }
        else if (enum_sym != NULL) {
            SgEnumDeclaration * enum_decl = isSgEnumDeclaration(enum_sym->get_declaration()->get_parent());
            ROSE_ASSERT(enum_decl != NULL);
            SgName name = enum_sym->get_name();
            *node = SageBuilder::buildEnumVal_nfi(0, enum_decl, name);
        }
        else if (sym != NULL) {
            std::cerr << "Runtime error: Unknown type of symbol for a declaration reference." << std::endl;
            std::cerr << "    sym->class_name() = " << sym->class_name()  << std::endl;
            ROSE_ASSERT(false);
        }
    }
    else {
         std::cerr << "Runtime error: Cannot find the symbol for a declaration reference (even after trying to buil th declaration)" << std::endl;
         ROSE_ASSERT(false);
    }

    return VisitExpr(decl_ref_expr, node) && res;
}

bool ClangToDot::VisitDesignatedInitExpr(clang::DesignatedInitExpr * designated_init_expr) {
    SgInitializer * init = NULL;    
    {
        SgNode * tmp_expr = Traverse(designated_init_expr->getInit());
        SgExpression * expr = isSgExpression(tmp_expr);
        ROSE_ASSERT(expr != NULL);
        SgExprListExp * expr_list_exp = isSgExprListExp(expr);
        if (expr_list_exp != NULL) {
            // FIXME get the type right...
            init = SageBuilder::buildAggregateInitializer_nfi(expr_list_exp, NULL);
        }
        else {
            init = SageBuilder::buildAssignInitializer_nfi(expr, expr->get_type());
        }
        ROSE_ASSERT(init != NULL);
        applySourceRange(init, designated_init_expr->getInit()->getSourceRange());
    }

    SgExprListExp * expr_list_exp = SageBuilder::buildExprListExp_nfi();
    clang::DesignatedInitExpr::designators_iterator it;
    for (it = designated_init_expr->designators_begin(); it != designated_init_expr->designators_end(); it++) {
        SgExpression * expr = NULL;
        if (it->isFieldDesignator()) {
            SgSymbol * symbol = GetSymbolFromSymbolTable(it->getField());
            SgVariableSymbol * var_sym = isSgVariableSymbol(symbol);
            ROSE_ASSERT(var_sym != NULL);
            expr = SageBuilder::buildVarRefExp_nfi(var_sym);
            applySourceRange(expr, it->getSourceRange());
        }
        else if (it->isArrayDesignator()) {
            SgNode * tmp_expr = Traverse(designated_init_expr->getArrayIndex(*it));
            expr = isSgExpression(tmp_expr);
            ROSE_ASSERT(expr != NULL);
        }
        else if (it->isArrayRangeDesignator()) {
            ROSE_ASSERT(!"I don't believe range designator initializer are supported by ROSE...");    
        }
        else ROSE_ASSERT(false);

        ROSE_ASSERT(expr != NULL);

        expr->set_parent(expr_list_exp);
        expr_list_exp->append_expression(expr);
    }

    applySourceRange(expr_list_exp, designated_init_expr->getDesignatorsSourceRange());

    SgDesignatedInitializer * design_init = new SgDesignatedInitializer(expr_list_exp, init);
    expr_list_exp->set_parent(design_init);
    init->set_parent(design_init);

    *node = design_init;

    return VisitExpr(designated_init_expr, node);
}

bool ClangToDot::VisitExtVectorElementExpr(clang::ExtVectorElementExpr * ext_vector_element_expr) {
    SgNode * tmp_base = Traverse(ext_vector_element_expr->getBase());
    SgExpression * base = isSgExpression(tmp_base);

    ROSE_ASSERT(base != NULL);

    SgType * type = buildTypeFromQualifiedType(ext_vector_element_expr->getType());

    clang::IdentifierInfo & ident_info = ext_vector_element_expr->getAccessor();
    std::string ident = ident_info.getName().str();

    SgScopeStatement * scope = SageBuilder::ScopeStack.front();
    SgGlobal * global = isSgGlobal(scope);
    ROSE_ASSERT(global != NULL);

  // Build Manually a SgVarRefExp to have the same Accessor (text version) TODO ExtVectorAccessor and ExtVectorType
    SgInitializedName * init_name = SageBuilder::buildInitializedName(ident, SageBuilder::buildVoidType(), NULL);
    setCompilerGeneratedFileInfo(init_name);
    init_name->set_scope(global);
    SgVariableSymbol * var_symbol = new SgVariableSymbol(init_name);
    SgVarRefExp * pseudo_field = new SgVarRefExp(var_symbol);
    setCompilerGeneratedFileInfo(pseudo_field, true);
    init_name->set_parent(pseudo_field);

    SgExpression * res = NULL;
    if (ext_vector_element_expr->isArrow())
        res = SageBuilder::buildArrowExp(base, pseudo_field);
    else
        res = SageBuilder::buildDotExp(base, pseudo_field);

    ROSE_ASSERT(res != NULL);

    *node = res;

   return VisitExpr(ext_vector_element_expr, node);
}

bool ClangToDot::VisitFloatingLiteral(clang::FloatingLiteral * floating_literal) {
    unsigned int precision =  llvm::APFloat::semanticsPrecision(floating_literal->getValue().getSemantics());
    if (precision == 24)
        *node = SageBuilder::buildFloatVal(floating_literal->getValue().convertToFloat());
    else if (precision == 53)
        *node = SageBuilder::buildDoubleVal(floating_literal->getValue().convertToDouble());
    else
        ROSE_ASSERT(!"In VisitFloatingLiteral: Unsupported float size");

    return VisitExpr(floating_literal, node);
}

bool ClangToDot::VisitImaginaryLiteral(clang::ImaginaryLiteral * imaginary_literal) {
    SgNode * tmp_imag_val = Traverse(imaginary_literal->getSubExpr());
    SgValueExp * imag_val = isSgValueExp(tmp_imag_val);
    ROSE_ASSERT(imag_val != NULL);

    SgComplexVal * comp_val = new SgComplexVal(NULL, imag_val, imag_val->get_type(), "");

    *node = comp_val;

    return VisitExpr(imaginary_literal, node);
}

bool ClangToDot::VisitInitListExpr(clang::InitListExpr * init_list_expr) {
    // We use the syntactic version of the initializer if it exists
    if (init_list_expr->getSyntacticForm() != NULL) return VisitInitListExpr(init_list_expr->getSyntacticForm(), node);

    SgExprListExp * expr_list_expr = SageBuilder::buildExprListExp_nfi();

    clang::InitListExpr::iterator it;
    for (it = init_list_expr->begin(); it != init_list_expr->end(); it++) {
        SgNode * tmp_expr = Traverse(*it);
        SgExpression * expr = isSgExpression(tmp_expr);
        ROSE_ASSERT(expr != NULL);
        expr_list_expr->append_expression(expr);
    }

    *node = expr_list_expr;

    return VisitExpr(init_list_expr, node);
}

bool ClangToDot::VisitIntegerLiteral(clang::IntegerLiteral * integer_literal) {
    *node = SageBuilder::buildIntVal(integer_literal->getValue().getSExtValue());

    return VisitExpr(integer_literal, node);
}

bool ClangToDot::VisitMemberExpr(clang::MemberExpr * member_expr) {
    bool res = true;

    SgNode * tmp_base = Traverse(member_expr->getBase());
    SgExpression * base = isSgExpression(tmp_base);
    ROSE_ASSERT(base != NULL);

    SgSymbol * sym = GetSymbolFromSymbolTable(member_expr->getMemberDecl());

    SgVariableSymbol * var_sym  = isSgVariableSymbol(sym);
    SgMemberFunctionSymbol * func_sym = isSgMemberFunctionSymbol(sym);

    SgExpression * sg_member_expr = NULL;

    bool successful_cast = var_sym || func_sym;
    if (sym != NULL && !successful_cast) {
        std::cerr << "Runtime error: Unknown type of symbol for a member reference." << std::endl;
        std::cerr << "    sym->class_name() = " << sym->class_name()  << std::endl;
        res = false;
    }
    else if (var_sym != NULL) {
        sg_member_expr = SageBuilder::buildVarRefExp(var_sym);
    }
    else if (func_sym != NULL) { // C++
        sg_member_expr = SageBuilder::buildMemberFunctionRefExp_nfi(func_sym, false, false); // FIXME 2nd and 3rd params ?
    }

    ROSE_ASSERT(sg_member_expr != NULL);

    // TODO (C++) member_expr->getQualifier() : for 'a->Base::foo'

    if (member_expr->isArrow())
        *node = SageBuilder::buildArrowExp(base, sg_member_expr);
    else
        *node = SageBuilder::buildDotExp(base, sg_member_expr);

    return VisitExpr(member_expr, node) && res;
}

bool ClangToDot::VisitParenExpr(clang::ParenExpr * paren_expr) {
    bool res = true;

    SgNode * tmp_subexpr = Traverse(paren_expr->getSubExpr());
    SgExpression * subexpr = isSgExpression(tmp_subexpr);
    if (tmp_subexpr != NULL && subexpr == NULL) {
        std::cerr << "Runtime error: tmp_subexpr != NULL && subexpr == NULL" << std::endl;
        res = false;
    }

    // bypass ParenExpr, their is nothing equivalent in SageIII
    *node = subexpr;

    return VisitExpr(paren_expr, node) && res;
}

bool ClangToDot::VisitPredefinedExpr(clang::PredefinedExpr * predefined_expr) {
    // FIXME It's get tricky here: PredefinedExpr represent compiler generateed variables
    //    I choose to attach those variables on demand in the function definition scope 

  // Traverse the scope's stack to find the last function definition:

    SgFunctionDefinition * func_def = NULL;
    std::list<SgScopeStatement *>::reverse_iterator it = SageBuilder::ScopeStack.rbegin();
    while (it != SageBuilder::ScopeStack.rend() && func_def == NULL) {
        func_def = isSgFunctionDefinition(*it);
        it++;
    }
    ROSE_ASSERT(func_def != NULL);

  // Determine the name of the variable

    SgName name;

    switch (predefined_expr->getIdentType()) {
        case clang::PredefinedExpr::Func:
            name = "__func__";
            break;
        case clang::PredefinedExpr::Function:
            name = "__FUNCTION__";
            break;
        case clang::PredefinedExpr::PrettyFunction:
            name = "__PRETTY_FUNCTION__";
            break;
        case clang::PredefinedExpr::PrettyFunctionNoVirtual:
            ROSE_ASSERT(false);
            break;
    }

  // Retrieve the associate symbol if it exists

    SgVariableSymbol * symbol = func_def->lookup_variable_symbol(name);

  // Else, build a compiler generated initialized name for this variable in the function defintion scope.

    if (symbol == NULL) {
        SgInitializedName * init_name = SageBuilder::buildInitializedName_nfi(name, SageBuilder::buildPointerType(SageBuilder::buildCharType()), NULL);

        init_name->set_parent(func_def);
        init_name->set_scope(func_def);

        Sg_File_Info * start_fi = Sg_File_Info::generateDefaultFileInfoForCompilerGeneratedNode();
        start_fi->setCompilerGenerated();
        init_name->set_startOfConstruct(start_fi);

        Sg_File_Info * end_fi   = Sg_File_Info::generateDefaultFileInfoForCompilerGeneratedNode();
        end_fi->setCompilerGenerated();
        init_name->set_endOfConstruct(end_fi);

        symbol = new SgVariableSymbol(init_name);

        func_def->insert_symbol(name, symbol);
    }
    ROSE_ASSERT(symbol != NULL);

  // Finally build the variable reference

    *node = SageBuilder::buildVarRefExp_nfi(symbol);

    return true;
}

bool ClangToDot::VisitStmtExpr(clang::StmtExpr * stmt_expr) {
    bool res = true;

    SgNode * tmp_substmt = Traverse(stmt_expr->getSubStmt());
    SgStatement * substmt = isSgStatement(tmp_substmt);
    if (tmp_substmt != NULL && substmt == NULL) {
        std::cerr << "Runtime error: tmp_substmt != NULL && substmt == NULL" << std::endl;
        res = false;
    }

    *node = new SgStatementExpression(substmt);

    return VisitExpr(stmt_expr, node) && res;
}

bool ClangToDot::VisitStringLiteral(clang::StringLiteral * string_literal) {
    std::string tmp = string_literal->getString().str();
    const char * raw_str = tmp.c_str();

    unsigned i = 0;
    unsigned l = 0;
    while (raw_str[i] != '\0') {
        if (
            raw_str[i] == '\\' ||
            raw_str[i] == '\n' ||
            raw_str[i] == '\r' ||
            raw_str[i] == '"')
        {
            l++;
        }
        l++;
        i++;
    }
    l++;

    char * str = (char *)malloc(l * sizeof(char));
    i = 0;
    unsigned cnt = 0;

    while (raw_str[i] != '\0') {
        switch (raw_str[i]) {
            case '\\':
                str[cnt++] = '\\';
                str[cnt++] = '\\';
                break;
            case '\n':
                str[cnt++] = '\\';
                str[cnt++] = 'n';
                break;
            case '\r':
                str[cnt++] = '\\';
                str[cnt++] = 'r';
                break;
            case '"':
                str[cnt++] = '\\';
                str[cnt++] = '"';
                break;
            default:
                str[cnt++] = raw_str[i];
        }
        i++;
    }
    str[cnt] = '\0';

    ROSE_ASSERT(l==cnt+1);

    *node = SageBuilder::buildStringVal(str);

    return VisitExpr(string_literal, node);
}

bool ClangToDot::VisitUnaryExprOrTypeTraitExpr(clang::UnaryExprOrTypeTraitExpr * unary_expr_or_type_trait_expr) {
    bool res = true;

    SgExpression * expr = NULL;
    SgType * type = NULL;

    if (unary_expr_or_type_trait_expr->isArgumentType()) {
        type = buildTypeFromQualifiedType(unary_expr_or_type_trait_expr->getArgumentType());
    }
    else {
        SgNode * tmp_expr = Traverse(unary_expr_or_type_trait_expr->getArgumentExpr());
        expr = isSgExpression(tmp_expr);

        if (tmp_expr != NULL && expr == NULL) {
            std::cerr << "Runtime error: tmp_expr != NULL && expr == NULL" << std::endl;
            res = false;
        }
    }

    switch (unary_expr_or_type_trait_expr->getKind()) {
        case clang::UETT_SizeOf:
            if (type != NULL) *node = SageBuilder::buildSizeOfOp_nfi(type);
            else if (expr != NULL) *node = SageBuilder::buildSizeOfOp_nfi(expr);
            else res = false;
            break;
        case clang::UETT_AlignOf:
            ROSE_ASSERT(!"C/C++  - AlignOf is not supported!");
        case clang::UETT_VecStep:
            ROSE_ASSERT(!"OpenCL - VecStep is not supported!");
    }

    return VisitStmt(unary_expr_or_type_trait_expr, node) && res;
}

bool ClangToDot::VisitUnaryOperator(clang::UnaryOperator * unary_operator) {
    bool res = true;

    SgNode * tmp_subexpr = Traverse(unary_operator->getSubExpr());
    SgExpression * subexpr = isSgExpression(tmp_subexpr);
    if (tmp_subexpr != NULL && subexpr == NULL) {
        std::cerr << "Runtime error: tmp_subexpr != NULL && subexpr == NULL" << std::endl;
        res = false;
    }

    switch (unary_operator->getOpcode()) {
        case clang::UO_PostInc:
            *node = SageBuilder::buildPlusPlusOp(subexpr, SgUnaryOp::postfix);
            break;
        case clang::UO_PostDec:
            *node = SageBuilder::buildMinusMinusOp(subexpr, SgUnaryOp::postfix);
            break;
        case clang::UO_PreInc:
            *node = SageBuilder::buildPlusPlusOp(subexpr, SgUnaryOp::prefix);
            break;
        case clang::UO_PreDec:
            *node = SageBuilder::buildMinusMinusOp(subexpr, SgUnaryOp::prefix);
            break;
        case clang::UO_AddrOf:
            *node = SageBuilder::buildAddressOfOp(subexpr);
            break;
        case clang::UO_Deref:
            *node = SageBuilder::buildPointerDerefExp(subexpr);
            break;
        case clang::UO_Plus:
            *node = SageBuilder::buildUnaryAddOp(subexpr);
            break;
        case clang::UO_Minus:
            *node = SageBuilder::buildMinusOp(subexpr);
            break;
        case clang::UO_Not:
            *node = SageBuilder::buildNotOp(subexpr);
            break;
        case clang::UO_LNot:
            *node = SageBuilder::buildBitComplementOp(subexpr);
            break;
        case clang::UO_Real:
            *node = SageBuilder::buildImagPartOp(subexpr);
            break;
        case clang::UO_Imag:
            *node = SageBuilder::buildRealPartOp(subexpr);
            break;
        case clang::UO_Extension:
            *node = subexpr;
            break;
        default:
            std::cerr << "Runtime error: Unknown unary operator." << std::endl;
            res = false;
    }

    return VisitExpr(unary_operator, node) && res;
}

bool ClangToDot::VisitVAArgExpr(clang::VAArgExpr * va_arg_expr) {
    SgNode * tmp_expr = Traverse(va_arg_expr->getSubExpr());
    SgExpression * expr = isSgExpression(tmp_expr);
    ROSE_ASSERT(expr != NULL);

    *node = SageBuilder::buildVarArgOp_nfi(expr, expr->get_type());

    return VisitExpr(va_arg_expr, node);
}

bool ClangToDot::VisitForStmt(clang::ForStmt * for_stmt) {
    bool res = true;

    SgForStatement * sg_for_stmt = SageBuilder::buildForStatement_nfi((SgForInitStatement *)NULL, NULL, NULL, NULL);

    SageBuilder::pushScopeStack(sg_for_stmt);

  // Initialization

    SgForInitStatement * for_init_stmt = NULL;

    {
        SgStatementPtrList for_init_stmt_list;
        SgNode * tmp_init = Traverse(for_stmt->getInit());
        SgStatement * init_stmt = isSgStatement(tmp_init);
        SgExpression * init_expr = isSgExpression(tmp_init);
        if (tmp_init != NULL && init_stmt == NULL && init_expr == NULL) {
            std::cerr << "Runtime error: tmp_init != NULL && init_stmt == NULL && init_expr == NULL (" << tmp_init->class_name() << ")" << std::endl;
            res = false;
        }
        else if (init_expr != NULL) {
            init_stmt = SageBuilder::buildExprStatement(init_expr);
            applySourceRange(init_stmt, for_stmt->getInit()->getSourceRange());
        }
        if (init_stmt != NULL)
            for_init_stmt_list.push_back(init_stmt);
        for_init_stmt = SageBuilder::buildForInitStatement_nfi(for_init_stmt_list);
        if (for_stmt->getInit() != NULL)
            applySourceRange(for_init_stmt, for_stmt->getInit()->getSourceRange());
        else
            setCompilerGeneratedFileInfo(for_init_stmt, true);
    }

  // Condition

    SgStatement * cond_stmt = NULL;

    {
        SgNode * tmp_cond = Traverse(for_stmt->getCond());
        SgExpression * cond = isSgExpression(tmp_cond);
        if (tmp_cond != NULL && cond == NULL) {
            std::cerr << "Runtime error: tmp_cond != NULL && cond == NULL" << std::endl;
            res = false;
        }
        if (cond != NULL) { 
            cond_stmt = SageBuilder::buildExprStatement(cond);
            applySourceRange(cond_stmt, for_stmt->getCond()->getSourceRange());
        }
        else {
            cond_stmt = SageBuilder::buildNullStatement_nfi();
            setCompilerGeneratedFileInfo(cond_stmt);
        }
    }

  // Increment

    SgExpression * inc = NULL;

    {
        SgNode * tmp_inc  = Traverse(for_stmt->getInc());
        inc = isSgExpression(tmp_inc);
        if (tmp_inc != NULL && inc == NULL) {
            std::cerr << "Runtime error: tmp_inc != NULL && inc == NULL" << std::endl;
            res = false;
        }
        if (inc == NULL) {
            inc = SageBuilder::buildNullExpression_nfi();
            setCompilerGeneratedFileInfo(inc);
        }
    }

  // Body

    SgStatement * body = NULL;

    {
        SgNode * tmp_body = Traverse(for_stmt->getBody());
        body = isSgStatement(tmp_body);
        if (body == NULL) {
            SgExpression * body_expr = isSgExpression(tmp_body);
            if (body_expr != NULL) {
                body = SageBuilder::buildExprStatement(body_expr);
                applySourceRange(body, for_stmt->getBody()->getSourceRange());
            }
        }
        if (tmp_body != NULL && body == NULL) {
            std::cerr << "Runtime error: tmp_body != NULL && body == NULL" << std::endl;
            res = false;
        }
        if (body == NULL) {
            body = SageBuilder::buildNullStatement_nfi();
            setCompilerGeneratedFileInfo(body);
        }
    }

    SageBuilder::popScopeStack();

  // Attach sub trees to the for statement

    for_init_stmt->set_parent(sg_for_stmt);
    if (sg_for_stmt->get_for_init_stmt() != NULL)
        SageInterface::deleteAST(sg_for_stmt->get_for_init_stmt());
    sg_for_stmt->set_for_init_stmt(for_init_stmt);

    if (cond_stmt != NULL) {
        cond_stmt->set_parent(sg_for_stmt);
        sg_for_stmt->set_test(cond_stmt);
    }

    if (inc != NULL) {
        inc->set_parent(sg_for_stmt);
        sg_for_stmt->set_increment(inc);
    }

    if (body != NULL) {
        body->set_parent(sg_for_stmt);
        sg_for_stmt->set_loop_body(body);
    }

    *node = sg_for_stmt;

    return VisitStmt(for_stmt, node) && res;
}

bool ClangToDot::VisitGotoStmt(clang::GotoStmt * goto_stmt) {
    bool res = true;
/*
    SgSymbol * tmp_sym = GetSymbolFromSymbolTable(goto_stmt->getLabel());
    SgLabelSymbol * sym = isSgLabelSymbol(tmp_sym);
    if (sym == NULL) {
        std::cerr << "Runtime error: Cannot find the symbol for the label: \"" << goto_stmt->getLabel()->getStmt()->getName() << "\"." << std::endl;
        res = false;
    }
    else {
        *node = SageBuilder::buildGotoStatement(sym->get_declaration());
    }
*/

    SgNode * tmp_label = Traverse(goto_stmt->getLabel()->getStmt());
    SgLabelStatement * label_stmt = isSgLabelStatement(tmp_label);
    if (label_stmt == NULL) {
        std::cerr << "Runtime Error: Cannot find the label: \"" << goto_stmt->getLabel()->getStmt()->getName() << "\"." << std::endl;
        res = false;
    }
    else {
        *node = SageBuilder::buildGotoStatement(label_stmt);
    }

    return VisitStmt(goto_stmt, node) && res;
}

bool ClangToDot::VisitIfStmt(clang::IfStmt * if_stmt) {
    bool res = true;

    // TODO if_stmt->getConditionVariable() appears when a variable is declared in the condition...

    *node = SageBuilder::buildIfStmt_nfi(NULL, NULL, NULL);

    SageBuilder::pushScopeStack(isSgScopeStatement(*node));

    SgNode * tmp_cond = Traverse(if_stmt->getCond());
    SgExpression * cond_expr = isSgExpression(tmp_cond);
    SgStatement * cond_stmt = SageBuilder::buildExprStatement(cond_expr);
    applySourceRange(cond_stmt, if_stmt->getCond()->getSourceRange());

    SgNode * tmp_then = Traverse(if_stmt->getThen());
    SgStatement * then_stmt = isSgStatement(tmp_then);
    if (then_stmt == NULL) {
        SgExpression * then_expr = isSgExpression(tmp_then);
        ROSE_ASSERT(then_expr != NULL);
        then_stmt = SageBuilder::buildExprStatement(then_expr);
    }
    applySourceRange(then_stmt, if_stmt->getThen()->getSourceRange());

    SgNode * tmp_else = Traverse(if_stmt->getElse());
    SgStatement * else_stmt = isSgStatement(tmp_else);
    if (else_stmt == NULL) {
        SgExpression * else_expr = isSgExpression(tmp_else);
        if (else_expr != NULL)
            else_stmt = SageBuilder::buildExprStatement(else_expr);
    }
    if (else_stmt != NULL) applySourceRange(else_stmt, if_stmt->getElse()->getSourceRange());

    SageBuilder::popScopeStack();

    cond_stmt->set_parent(*node);
    isSgIfStmt(*node)->set_conditional(cond_stmt);

    then_stmt->set_parent(*node);
    isSgIfStmt(*node)->set_true_body(then_stmt);
    if (else_stmt != NULL) {
      else_stmt->set_parent(*node);
      isSgIfStmt(*node)->set_false_body(else_stmt);
    }

    return VisitStmt(if_stmt, node) && res;
}

bool ClangToDot::VisitLabelStmt(clang::LabelStmt * label_stmt) {
    bool res = true;

    SgName name(label_stmt->getName());

    SgNode * tmp_sub_stmt = Traverse(label_stmt->getSubStmt());
    SgStatement * sg_sub_stmt = isSgStatement(tmp_sub_stmt);
    if (sg_sub_stmt == NULL) {
        SgExpression * sg_sub_expr = isSgExpression(tmp_sub_stmt);
        ROSE_ASSERT(sg_sub_expr != NULL);
        sg_sub_stmt = SageBuilder::buildExprStatement(sg_sub_expr);
    }

    ROSE_ASSERT(sg_sub_stmt != NULL);

    *node = SageBuilder::buildLabelStatement_nfi(name, sg_sub_stmt, SageBuilder::topScopeStack());

    SgLabelStatement * sg_label_stmt = isSgLabelStatement(*node);
    SgFunctionDefinition * label_scope = NULL;
    std::list<SgScopeStatement *>::reverse_iterator it = SageBuilder::ScopeStack.rbegin();
    while (it != SageBuilder::ScopeStack.rend() && label_scope == NULL) {
        label_scope = isSgFunctionDefinition(*it);
        it++;
    }
    if (label_scope == NULL) {
         std::cerr << "Runtime error: Cannot find a surrounding function definition for the label statement: \"" << name << "\"." << std::endl;
         res = false;
    }
    else {
        sg_label_stmt->set_scope(label_scope);
        SgLabelSymbol* label_sym = new SgLabelSymbol(sg_label_stmt);
        label_scope->insert_symbol(label_sym->get_name(), label_sym);
    }

    return VisitStmt(label_stmt, node) && res;
}

bool ClangToDot::VisitNullStmt(clang::NullStmt * null_stmt) {
    *node = SageBuilder::buildNullStatement_nfi();
    return VisitStmt(null_stmt, node);
}

bool ClangToDot::VisitReturnStmt(clang::ReturnStmt * return_stmt) {
    bool res = true;

    SgNode * tmp_expr = Traverse(return_stmt->getRetValue());
    SgExpression * expr = isSgExpression(tmp_expr);
    if (tmp_expr != NULL && expr == NULL) {
        std::cerr << "Runtime error: tmp_expr != NULL && expr == NULL" << std::endl;
        res = false;
    }
    *node = SageBuilder::buildReturnStmt(expr);

    return VisitStmt(return_stmt, node) && res;
}

bool ClangToDot::VisitCaseStmt(clang::CaseStmt * case_stmt) {
    SgNode * tmp_stmt = Traverse(case_stmt->getSubStmt());
    SgStatement * stmt = isSgStatement(tmp_stmt);
    SgExpression * expr = isSgExpression(tmp_stmt);
    if (expr != NULL) {
        stmt = SageBuilder::buildExprStatement(expr);
        applySourceRange(stmt, case_stmt->getSubStmt()->getSourceRange());
    }
    ROSE_ASSERT(stmt != NULL);

    SgNode * tmp_lhs = Traverse(case_stmt->getLHS());
    SgExpression * lhs = isSgExpression(tmp_lhs);
    ROSE_ASSERT(lhs != NULL);

/*  FIXME GNU extension not-handled by ROSE
    SgNode * tmp_rhs = Traverse(case_stmt->getRHS());
    SgExpression * rhs = isSgExpression(tmp_rhs);
    ROSE_ASSERT(rhs != NULL);
*/
    ROSE_ASSERT(case_stmt->getRHS() == NULL);

    *node = SageBuilder::buildCaseOptionStmt_nfi(lhs, stmt);

    return VisitStmt(case_stmt, node);
}

bool ClangToDot::VisitDefaultStmt(clang::DefaultStmt * default_stmt) {
    SgNode * tmp_stmt = Traverse(default_stmt->getSubStmt());
    SgStatement * stmt = isSgStatement(tmp_stmt);

    *node = SageBuilder::buildDefaultOptionStmt_nfi(stmt);

    return VisitStmt(default_stmt, node);
}

bool ClangToDot::VisitSwitchStmt(clang::SwitchStmt * switch_stmt) {
    SgNode * tmp_cond = Traverse(switch_stmt->getCond());
    SgExpression * cond = isSgExpression(tmp_cond);
    ROSE_ASSERT(cond != NULL);
    
    SgStatement * expr_stmt = SageBuilder::buildExprStatement(cond);
        applySourceRange(expr_stmt, switch_stmt->getCond()->getSourceRange());

    SgSwitchStatement * sg_switch_stmt = SageBuilder::buildSwitchStatement_nfi(expr_stmt, NULL);

    cond->set_parent(expr_stmt);
    expr_stmt->set_parent(sg_switch_stmt);

    SageBuilder::pushScopeStack(sg_switch_stmt);

    SgNode * tmp_body = Traverse(switch_stmt->getBody());
    SgStatement * body = isSgStatement(tmp_body);
    ROSE_ASSERT(body != NULL);

    SageBuilder::popScopeStack();

    sg_switch_stmt->set_body(body);

    *node = sg_switch_stmt;

    return VisitStmt(switch_stmt, node);
}

bool ClangToDot::VisitWhileStmt(clang::WhileStmt * while_stmt) {
    SgNode * tmp_cond = Traverse(while_stmt->getCond());
    SgExpression * cond = isSgExpression(tmp_cond);
    ROSE_ASSERT(cond != NULL);

    SgStatement * expr_stmt = SageBuilder::buildExprStatement(cond);

    SgWhileStmt * sg_while_stmt = SageBuilder::buildWhileStmt_nfi(expr_stmt, NULL);

    cond->set_parent(expr_stmt);
    expr_stmt->set_parent(sg_while_stmt);

    SageBuilder::pushScopeStack(sg_while_stmt);

    SgNode * tmp_body = Traverse(while_stmt->getBody());
    SgStatement * body = isSgStatement(tmp_body);
    SgExpression * expr = isSgExpression(tmp_body);
    if (expr != NULL) {
        body =  SageBuilder::buildExprStatement(expr);
        applySourceRange(body, while_stmt->getBody()->getSourceRange());
    }
    ROSE_ASSERT(body != NULL);

    body->set_parent(sg_while_stmt);

    SageBuilder::popScopeStack();

    sg_while_stmt->set_body(body);

    *node = sg_while_stmt;

    return VisitStmt(while_stmt, node);
}

/***************/
/* Visit Types */
/***************/

bool ClangToDot::VisitType(clang::Type * type) {
    if (*node == NULL) {
        std::cerr << "Runtime error: No Sage node associated with the type..." << std::endl;
        return false;
    }
/*
    std::cerr << "Dump type " << type->getTypeClassName() << "(" << type << "): ";
    type->dump();
    std::cerr << std::endl;
*/
    // TODO

    return true;
}

bool ClangToDot::VisitConstantArrayType(clang::ConstantArrayType * constant_array_type) {
    SgType * type = buildTypeFromQualifiedType(constant_array_type->getElementType());

    // TODO clang::ArrayType::ArraySizeModifier

    SgExpression * expr = SageBuilder::buildIntVal(constant_array_type->getSize().getSExtValue());

    *node = SageBuilder::buildArrayType(type, expr);

    return VisitType(constant_array_type, node);
}

bool ClangToDot::VisitIncompleteArrayType(clang::IncompleteArrayType * incomplete_array_type) {
    SgType * type = buildTypeFromQualifiedType(incomplete_array_type->getElementType());

    // TODO clang::ArrayType::ArraySizeModifier

    *node = SageBuilder::buildArrayType(type);

    return VisitType(incomplete_array_type, node);
}

bool ClangToDot::VisitAttributedType(clang::AttributedType * attributed_type) {
    SgType * type = buildTypeFromQualifiedType(attributed_type->getModifiedType());

    SgModifierType * modified_type = SgModifierType::createType(type);
    SgTypeModifier & sg_modifer = modified_type->get_typeModifier();

    switch (attributed_type->getAttrKind()) {
        case clang::AttributedType::attr_noreturn:             sg_modifer.setGnuAttributeNoReturn();      break;
        case clang::AttributedType::attr_cdecl:                sg_modifer.setGnuAttributeCdecl();         break;
        case clang::AttributedType::attr_stdcall:              sg_modifer.setGnuAttributeStdcall();       break;

        case clang::AttributedType::attr_address_space:
            std::cerr << "Unsupported attribute attr_address_space" << std::endl; ROSE_ASSERT(false);
        case clang::AttributedType::attr_regparm:
            std::cerr << "Unsupported attribute attr_regparm" << std::endl; ROSE_ASSERT(false);
        case clang::AttributedType::attr_vector_size:
            std::cerr << "Unsupported attribute attr_vector_size" << std::endl; ROSE_ASSERT(false);
        case clang::AttributedType::attr_neon_vector_type:
            std::cerr << "Unsupported attribute attr_neon_vector_type" << std::endl; ROSE_ASSERT(false);
        case clang::AttributedType::attr_neon_polyvector_type:
            std::cerr << "Unsupported attribute attr_neon_polyvector_type" << std::endl; ROSE_ASSERT(false);
        case clang::AttributedType::attr_objc_gc:
            std::cerr << "Unsupported attribute attr_objc_gc" << std::endl; ROSE_ASSERT(false);
        case clang::AttributedType::attr_objc_ownership:
            std::cerr << "Unsupported attribute attr_objc_ownership" << std::endl; ROSE_ASSERT(false);
        case clang::AttributedType::attr_pcs:
            std::cerr << "Unsupported attribute attr_pcs" << std::endl; ROSE_ASSERT(false);
        case clang::AttributedType::attr_fastcall:
            std::cerr << "Unsupported attribute attr_fastcall" << std::endl; ROSE_ASSERT(false);
        case clang::AttributedType::attr_thiscall:
            std::cerr << "Unsupported attribute attr_thiscall" << std::endl; ROSE_ASSERT(false);
        case clang::AttributedType::attr_pascal:
            std::cerr << "Unsupported attribute attr_pascal" << std::endl; ROSE_ASSERT(false);
        default:
            std::cerr << "Unknown attribute" << std::endl; ROSE_ASSERT(false);
    } 

    *node = SgModifierType::insertModifierTypeIntoTypeTable(modified_type);;

    return VisitType(attributed_type, node);
}

bool ClangToDot::VisitBuiltinType(clang::BuiltinType * builtin_type) {
    switch (builtin_type->getKind()) {
        case clang::BuiltinType::Void:       *node = SageBuilder::buildVoidType();             break;
        case clang::BuiltinType::Bool:       *node = SageBuilder::buildBoolType();             break;
        case clang::BuiltinType::Short:      *node = SageBuilder::buildShortType();            break;
        case clang::BuiltinType::Int:        *node = SageBuilder::buildIntType();              break;
        case clang::BuiltinType::Long:       *node = SageBuilder::buildLongType();             break;
        case clang::BuiltinType::LongLong:   *node = SageBuilder::buildLongLongType();         break;
        case clang::BuiltinType::Float:      *node = SageBuilder::buildFloatType();            break;
        case clang::BuiltinType::Double:     *node = SageBuilder::buildDoubleType();           break;
        case clang::BuiltinType::LongDouble: *node = SageBuilder::buildLongDoubleType();       break;

        case clang::BuiltinType::Char_S:     *node = SageBuilder::buildCharType();             break;

        case clang::BuiltinType::UInt:       *node = SageBuilder::buildUnsignedIntType();      break;
        case clang::BuiltinType::UChar:      *node = SageBuilder::buildUnsignedCharType();     break;
        case clang::BuiltinType::SChar:      *node = SageBuilder::buildSignedCharType();       break;
        case clang::BuiltinType::UShort:     *node = SageBuilder::buildUnsignedShortType();    break;
        case clang::BuiltinType::ULong:      *node = SageBuilder::buildUnsignedLongType();     break;
        case clang::BuiltinType::ULongLong:  *node = SageBuilder::buildUnsignedLongLongType(); break;
/*
        case clang::BuiltinType::NullPtr:    *node = SageBuilder::build(); break;
*/
        // TODO ROSE type ?
        case clang::BuiltinType::UInt128:    *node = SageBuilder::buildUnsignedLongLongType(); break;
        case clang::BuiltinType::Int128:     *node = SageBuilder::buildLongLongType();         break;
 
        case clang::BuiltinType::Char_U:    std::cerr << "Char_U    -> "; break;
        case clang::BuiltinType::WChar_U:   std::cerr << "WChar_U   -> "; break;
        case clang::BuiltinType::Char16:    std::cerr << "Char16    -> "; break;
        case clang::BuiltinType::Char32:    std::cerr << "Char32    -> "; break;
        case clang::BuiltinType::WChar_S:   std::cerr << "WChar_S   -> "; break;


        case clang::BuiltinType::ObjCId:
        case clang::BuiltinType::ObjCClass:
        case clang::BuiltinType::ObjCSel:
        case clang::BuiltinType::Dependent:
        case clang::BuiltinType::Overload:
        case clang::BuiltinType::BoundMember:
        case clang::BuiltinType::UnknownAny:
        default:
            std::cerr << "Unknown builtin type: " << builtin_type->getName(p_compiler_instance->getLangOpts()) << " !" << std::endl;
            exit(-1);
    }

    return VisitType(builtin_type, node);
}

bool ClangToDot::VisitComplexType(clang::ComplexType * complex_type) {
    bool res = true;

    SgType * type = buildTypeFromQualifiedType(complex_type->getElementType());

    *node = SageBuilder::buildComplexType(type);

    return VisitType(complex_type, node) && res;
}

bool ClangToDot::VisitFunctionNoProtoType(clang::FunctionNoProtoType * function_no_proto_type) {
    bool res = true;

    SgFunctionParameterTypeList * param_type_list = new SgFunctionParameterTypeList();

    SgType * ret_type = buildTypeFromQualifiedType(function_no_proto_type->getResultType()); 

    *node = SageBuilder::buildFunctionType(ret_type, param_type_list);

    return VisitType(function_no_proto_type, node) && res;
}

bool ClangToDot::VisitFunctionProtoType(clang::FunctionProtoType * function_proto_type) {
    bool res = true;
    SgFunctionParameterTypeList * param_type_list = new SgFunctionParameterTypeList();
    for (unsigned i = 0; i < function_proto_type->getNumArgs(); i++) {
        SgType * param_type = buildTypeFromQualifiedType(function_proto_type->getArgType(i));

        param_type_list->append_argument(param_type);
    }

    if (function_proto_type->isVariadic()) {
        param_type_list->append_argument(SgTypeEllipse::createType());
    }

    SgType * ret_type = buildTypeFromQualifiedType(function_proto_type->getResultType());

    SgFunctionType * func_type = SageBuilder::buildFunctionType(ret_type, param_type_list);
    if (function_proto_type->isVariadic()) func_type->set_has_ellipses(1);

    *node = func_type;

    return VisitType(function_proto_type, node) && res;
}

bool ClangToDot::VisitParenType(clang::ParenType * paren_type) {
    *node = buildTypeFromQualifiedType(paren_type->getInnerType());

    return VisitType(paren_type, node);
}

bool ClangToDot::VisitPointerType(clang::PointerType * pointer_type) {
    SgType * type = buildTypeFromQualifiedType(pointer_type->getPointeeType());

    *node = SageBuilder::buildPointerType(type);

    return VisitType(pointer_type, node);
}

bool ClangToDot::VisitEnumType(clang::EnumType * enum_type) {
    SgSymbol * sym = GetSymbolFromSymbolTable(enum_type->getDecl());

    SgEnumSymbol * enum_sym = isSgEnumSymbol(sym);

    if (enum_sym == NULL) {
        SgNode * tmp_decl = Traverse(enum_type->getDecl());
        SgEnumDeclaration * sg_decl = isSgEnumDeclaration(tmp_decl);

        ROSE_ASSERT(sg_decl != NULL);

        *node = sg_decl->get_type();
    }
    else {
        *node = enum_sym->get_type();
    }

    if (isSgEnumType(*node) != NULL) {
        if (enum_sym == NULL) {
            p_enum_type_decl_first_see_in_type.insert(std::pair<SgEnumType *, bool>(isSgEnumType(*node), true));
        }
        else
            p_enum_type_decl_first_see_in_type.insert(std::pair<SgEnumType *, bool>(isSgEnumType(*node), false));
    }

    return VisitType(enum_type, node);
}

bool ClangToDot::VisitRecordType(clang::RecordType * record_type) {
    SgSymbol * sym = GetSymbolFromSymbolTable(record_type->getDecl());

    SgClassSymbol * class_sym = isSgClassSymbol(sym);

    if (class_sym == NULL) {
        SgNode * tmp_decl = Traverse(record_type->getDecl());
        SgClassDeclaration * sg_decl = isSgClassDeclaration(tmp_decl);

        ROSE_ASSERT(sg_decl != NULL);

        *node = sg_decl->get_type();
    }
    else {
        *node = class_sym->get_type();
    }

    if (isSgClassType(*node) != NULL) {
        if (class_sym == NULL) {
            p_class_type_decl_first_see_in_type.insert(std::pair<SgClassType *, bool>(isSgClassType(*node), true));
            isSgNamedType(*node)->set_autonomous_declaration(true);
        }
        else
            p_class_type_decl_first_see_in_type.insert(std::pair<SgClassType *, bool>(isSgClassType(*node), false));
    }

    return VisitType(record_type, node);
}

bool ClangToDot::VisitTypedefType(clang::TypedefType * typedef_type) {
    bool res = true;

    SgSymbol * sym = GetSymbolFromSymbolTable(typedef_type->getDecl());
    SgTypedefSymbol * tdef_sym = isSgTypedefSymbol(sym);

    if (tdef_sym == NULL) {
        std::cerr << "Runtime Error: Cannot find a typedef symbol for the TypedefType." << std::endl;
        res = false;
    }

    *node = tdef_sym->get_type();

   return VisitType(typedef_type, node) && res;
}

bool ClangToDot::VisitElaboratedType(clang::ElaboratedType * elaborated_type) {
    SgType * type = buildTypeFromQualifiedType(elaborated_type->getNamedType());

    // FIXME clang::ElaboratedType contains the "sugar" of a type reference (eg, "struct A" or "M::N::A"), it should be pass down to ROSE

    *node = type;

    return VisitType(elaborated_type, node);
}

bool ClangToDot::VisitVectorType(clang::VectorType * vector_type) {
    SgType * type = buildTypeFromQualifiedType(vector_type->getElementType());

    SgModifierType * modified_type = new SgModifierType(type);
    SgTypeModifier & sg_modifer = modified_type->get_typeModifier();

    sg_modifer.setVectorType();
    sg_modifer.set_vector_size(vector_type->getNumElements());

    *node = SgModifierType::insertModifierTypeIntoTypeTable(modified_type);

    return VisitType(vector_type, node);
}

