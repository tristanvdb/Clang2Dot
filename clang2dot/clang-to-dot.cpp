
#include "clang-to-dot.hpp"

#include <iostream>
#include <sstream>
#include <string>

void clang2dot(
    std::vector<std::string> inc_dirs_list,
    std::vector<std::string> define_list,
    std::vector<std::string> inc_list,
    std::string input_file,
    std::ostream & out
) {
  // 0 - Analyse Cmd Line

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

    unsigned cnt = define_list.size() + inc_dirs_list.size() + inc_list.size() + 1;
    char ** args = new char*[cnt];
    args[0] = "Clang2Dot";
    std::vector<std::string>::iterator it_str;
    unsigned i = 1;
    for (it_str = define_list.begin(); it_str != define_list.end(); it_str++) {
        args[i] = new char[it_str->size() + 3];
        args[i] = "-D";
        strcpy(&(args[i][2]), it_str->c_str());
        i++;
    }
    for (it_str = inc_dirs_list.begin(); it_str != inc_dirs_list.end(); it_str++) {
        args[i] = new char[it_str->size() + 3];
        args[i] = "-I";
        strcpy(&(args[i][2]), it_str->c_str());
        i++;
    }
    for (it_str = inc_list.begin(); it_str != inc_list.end(); it_str++) {
        args[i] = new char[it_str->size() + 9];
        args[i] = "-include";
        strcpy(&(args[i][8]), it_str->c_str());
        i++;
    }

  // 2 - Create a compiler instance

    clang::CompilerInstance * compiler_instance = new clang::CompilerInstance();

    clang::TextDiagnosticPrinter * diag_printer = new clang::TextDiagnosticPrinter(llvm::errs(), clang::DiagnosticOptions());
    compiler_instance->createDiagnostics(cnt, args, diag_printer, true, false);

    clang::CompilerInvocation * invocation = new clang::CompilerInvocation();
    clang::CompilerInvocation::CreateFromArgs(*invocation, &(args[1]), &(args[cnt]), compiler_instance->getDiagnostics());
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

  // 4 - Generate Graphviz
  
    translator.toDot(out);
}



/* Constructor, Destructor and common methods for ClangToDot */

ClangToDot::ClangToDot(clang::CompilerInstance * compiler_instance, Language language_) :
    clang::ASTConsumer(),
    p_compiler_instance(compiler_instance),
    p_decl_translation_map(),
    p_stmt_translation_map(),
    p_type_translation_map(),
    p_node_desc(),
    language(language_),
    ident_cnt(0)
{}

ClangToDot::~ClangToDot() {}

std::string ClangToDot::genNextIdent() {
    std::ostringstream oss;
    oss << "ident" << ident_cnt++;
    return oss.str();
}

/* Printer method: output GraphViz format */

void ClangToDot::toDot(std::ostream & out) const {
    out << "digraph {" << std::endl;

    std::map<std::string, NodeDescriptor>::const_iterator it_node;
    for (it_node = p_node_desc.begin(); it_node != p_node_desc.end(); it_node++)
        it_node->second.toDot(out);

    out << "}" << std::endl;
}

/* Overload of ASTConsumer::HandleTranslationUnit, it is the "entry point" */

void ClangToDot::HandleTranslationUnit(clang::ASTContext & ast_context) {
    Traverse(ast_context.getTranslationUnitDecl());
}

/* ClangToDot::NodeDescriptor */

ClangToDot::NodeDescriptor::NodeDescriptor(std::string ident_) :
    ident(ident_),
    kind_hierarchy(),
    successors(),
    attributes()
{}

/*
std::string ident;
std::vector<std::string> kind_hierarchy;
std::vector<std::pair<std::string, std::string> > successors;
std::vector<std::pair<std::string, std::string> > attributes;
*/

void ClangToDot::NodeDescriptor::toDot(std::ostream & out) const {
    std::vector<std::string>::const_iterator str_it;
    std::vector<std::pair<std::string, std::string> >::const_iterator pair_str_it;

    out << "\t" << ident << "[";
        out << "label=\"";
            out << kind_hierarchy.front() << "\\n";
            for (str_it = kind_hierarchy.begin() + 1; str_it != kind_hierarchy.end(); str_it++)
                out << "::" << *str_it;
            out << "\\n\\n";
            for (pair_str_it = attributes.begin(); pair_str_it != attributes.end(); pair_str_it++)
                out << pair_str_it->first << " : " << pair_str_it->second << "\\n";
        out << "\"";
        // TODO color from node type (NIY)
    out << "];" << std::endl;
    for (pair_str_it = successors.begin(); pair_str_it != successors.end(); pair_str_it++) {
        if (pair_str_it->second != "")
            out << "\t\t" << ident << " -> " << pair_str_it->second << "[label=\"" << pair_str_it->first << "\"];" << std::endl;
    }
    out << std::endl;
}

void ClangToDot::VisitTemplateArgument(const clang::TemplateArgument & template_argument, ClangToDot::NodeDescriptor & node_desc, std::string prefix) {
    std::ostringstream oss;
    oss << prefix;
    switch (template_argument.getKind()) {
        case clang::TemplateArgument::Null:
            oss << " null";
            node_desc.attributes.push_back(std::pair<std::string, std::string>(oss.str(), ""));
            break;
        case clang::TemplateArgument::Type:
            oss << " type";
            node_desc.successors.push_back(std::pair<std::string, std::string>(oss.str(), Traverse(template_argument.getAsType().getTypePtr())));
            break;
        case clang::TemplateArgument::Declaration:
            oss << " declaration";
            node_desc.successors.push_back(std::pair<std::string, std::string>(oss.str(), Traverse(template_argument.getAsDecl())));
            break;
        case clang::TemplateArgument::Integral:
            oss << " integral";
            assert(DEBUG_TODO == 0); // TODO
            break;
        case clang::TemplateArgument::Template:
            oss << " template";
            assert(DEBUG_TODO == 0); // TODO
            break;
        case clang::TemplateArgument::TemplateExpansion:
            oss << " template_expansion";
            assert(DEBUG_TODO == 0); // TODO
            break;
        case clang::TemplateArgument::Expression:
            oss << " expression";
            node_desc.successors.push_back(std::pair<std::string, std::string>(oss.str(), Traverse(template_argument.getAsExpr())));
            break;
        case clang::TemplateArgument::Pack:
        {
            clang::TemplateArgument::pack_iterator pack_it;
            unsigned cnt = 0;
            for (pack_it = template_argument.pack_begin(); pack_it != template_argument.pack_end(); pack_it++) {
                oss << " pack[" << cnt++ << "]";
                VisitTemplateArgument(*pack_it, node_desc, oss.str());
            }
            break;
        }
    }
}

void ClangToDot::VisitNestedNameSpecifier(clang::NestedNameSpecifier * nested_name_specifier, ClangToDot::NodeDescriptor & node_desc, std::string prefix) {
    if (nested_name_specifier == NULL) return;

    ClangToDot::VisitNestedNameSpecifier(nested_name_specifier->getPrefix(), node_desc, prefix + " prefix");

    switch (nested_name_specifier->getKind()) {
        case clang::NestedNameSpecifier::Identifier:
            node_desc.attributes.push_back(
                std::pair<std::string, std::string>(prefix + " identifier", nested_name_specifier->getAsIdentifier()->getName().data())
            );
            break;
        case clang::NestedNameSpecifier::Namespace:
            node_desc.successors.push_back(
                std::pair<std::string, std::string>(prefix + " namespace", Traverse(nested_name_specifier->getAsNamespace()))
            );
            break;
        case clang::NestedNameSpecifier::NamespaceAlias:
            node_desc.successors.push_back(
                std::pair<std::string, std::string>(prefix + " namespace_alias", Traverse(nested_name_specifier->getAsNamespaceAlias()))
            );
            break;
        case clang::NestedNameSpecifier::TypeSpec:
            node_desc.successors.push_back(
                std::pair<std::string, std::string>(prefix + " type_specifier", Traverse(nested_name_specifier->getAsType()))
            );
            break;
        case clang::NestedNameSpecifier::TypeSpecWithTemplate:
            node_desc.successors.push_back(
                std::pair<std::string, std::string>(prefix + " type_specifier_with_template", Traverse(nested_name_specifier->getAsType()))
            );
            break;
        case clang::NestedNameSpecifier::Global:
            node_desc.attributes.push_back(std::pair<std::string, std::string>(prefix, "global (::)"));
            break;
    }
}

void ClangToDot::VisitTemplateName(const clang::TemplateName & template_name, ClangToDot::NodeDescriptor & node_desc, std::string prefix) {
    std::ostringstream oss;
    oss << prefix;
    switch (template_name.getKind()) {
        case clang::TemplateName::Template:
            oss << " template";
            node_desc.successors.push_back(std::pair<std::string, std::string>(oss.str(), Traverse(template_name.getAsTemplateDecl())));
            break;
        case clang::TemplateName::OverloadedTemplate:
        {
            clang::OverloadedTemplateStorage * overloaded_template_storage = template_name.getAsOverloadedTemplate();
            assert(DEBUG_TODO == 0); // TODO
            break;
        }
        case clang::TemplateName::QualifiedTemplate:
            oss << " qualified_template";
            VisitNestedNameSpecifier(template_name.getAsQualifiedTemplateName()->getQualifier(), node_desc, oss.str() + "nested_name_specifier");
            node_desc.successors.push_back(
                std::pair<std::string, std::string>(oss.str() + "declaration", Traverse(template_name.getAsQualifiedTemplateName()->getDecl()))
            );
            node_desc.successors.push_back(
                std::pair<std::string, std::string>(oss.str() + "template_declaration", Traverse(template_name.getAsQualifiedTemplateName()->getTemplateDecl()))
            );
            break;
        case clang::TemplateName::DependentTemplate:
            assert(DEBUG_TODO == 0); // TODO
            break;
        case clang::TemplateName::SubstTemplateTemplateParm:
            assert(DEBUG_TODO == 0); // TODO
            break;
        case clang::TemplateName::SubstTemplateTemplateParmPack:
            assert(DEBUG_TODO == 0); // TODO
            break;
    }
}
