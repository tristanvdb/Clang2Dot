
#include "clang-to-dot.hpp"

#include <iostream>
#include <fstream>

int main(int argc, char ** argv) {
    std::vector<std::string> inc_dirs_list;
    std::vector<std::string> define_list;
    std::vector<std::string> inc_list;
    std::string input_file;
    std::string output_file;

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
            input_file = current_arg;
        }
    }
    
    if (output_file == "")    
        clang2dot(inc_dirs_list, define_list, inc_list, input_file, std::cout);
    else {
        std::ofstream out;
        out.open(output_file.c_str());
        if (!out.is_open()) {
            std::cerr << "Impossible to open " << output_file << " for writing!" << std::endl;
            exit(-1);
        }
        clang2dot(inc_dirs_list, define_list, inc_list, input_file, out);
        out.close();
    }

    return 0;
}
