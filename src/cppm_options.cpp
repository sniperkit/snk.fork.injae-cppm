#include"cppm.h"
#include"cppm_options.h"
#include"cmake/find_package.h"
#include"cmake/cmake_option.h"
#include"cmake/generator.h"
#include"options/init.h"
#include"options/install.h"
#include"url.h"

#include<range/v3/core.hpp>
#include<range/v3/numeric/accumulate.hpp>
#include<boost/regex.hpp>

#include<string>
#include<iterator>

CppmOptions::CppmOptions(int argc, char* argv[]) : Option("Cppm Options", argc, argv) {
    desc_.add_options()
        ("help"      , "produce a help message"     )
        ("version,v" , "Display the version number" )
        ;
    visible_option_.add(desc_);
    desc_.add(nieel::make_command_desc());
}

void CppmOptions::run() {
         if(vm_.count("help")   ) _help(); 
    else if(vm_.count("command")) { auto cmd = vm_["command"].as<std::string>();
        // don't use Project config 
             if(cmd == "hint")       _get_cmake_lib_hint();
        else if(cmd == "init")       _init();
        
        // use Project config
        else if(cmd == "build")      _build();
        else if(cmd == "run")        _run();
        else if(cmd == "install")    _install();
        else if(cmd == "thirdparty") _show_thirdparties();
        else                         _user_command(cmd.c_str());
    }
    else if(vm_.count("version")) _version();
}

void CppmOptions::_user_command(std::string_view cmd) {
    Cppm::instance()->parse_project_config();
    auto project = Cppm::instance()->project();
    auto subargs = ranges::accumulate(get_subarg(), std::string{});
    for(auto& command : subcommand_) {
         if(command.first == cmd) {
             system(("cd "+ project.path + " && " + command.second + " " + subargs).c_str());
             continue;
         }
    }
}

void CppmOptions::registe_subcommand(std::pair<std::string, std::string> command) {
    subcommand_.insert(command);
}

void CppmOptions::_help() {
   std::cout << "Usage: regex [options]\n"
             << visible_option_
             << std::endl;
}

void CppmOptions::_version() {
   Cppm::instance()->parse_project_config();
   std::cout << "Version: "<< Cppm::instance()->project().version << std::endl;
}

void CppmOptions::_build() {
    using namespace cmake::option;
    Cppm::instance()->parse_project_config();
    auto subargs = get_subarg();
    std::ofstream file (Cppm::instance()->project().path + "/CMakeLists.txt"); file.is_open();
    file << cmake::make_default_project(Cppm::instance()->project()); file.close();
    
    auto project = Cppm::instance()->project();
    std::string cmd = "cd " + project.bin 
                    + " && cmake" + builder(project) + compiler(project) + " .. "
                    + " && " + build(project);
    for(auto subarg : subargs) {
        cmd += subarg;    
    }
    
    system(cmd.c_str()); 
}

void CppmOptions::_run() {
    Cppm::instance()->parse_project_config();
    auto project = Cppm::instance()->project();
    auto subargs = ranges::accumulate(get_subarg(), std::string{});
    std::string cmd = "cd " + project.bin + " && ./" + project.name +" " + subargs;
    system(cmd.c_str());
}

void CppmOptions::_show_thirdparties() {
    Cppm::instance()->parse_project_config();
    auto thirdparties = Cppm::instance()->thirdparties();
    for(auto thirdparty : thirdparties) {
        std::cout << "[" + thirdparty.name << "]\n"
                  << "-build-type: " + thirdparty.build_type << "\n"
                  << "-version: "+ thirdparty.version << "\n"
                  ;
    }
}

void CppmOptions::_install() {
    Cppm::instance()->parse_project_config();
    make_sub_command<option::Install>().run();
}

void CppmOptions::_get_cmake_lib_hint() {
    cppm::Thirdparty thirdparty;
    thirdparty.name = get_subarg()[0];
   	 
    for(auto lib : cmake::get_find_package_list()) {
        if(!(lib.name == thirdparty.name)) continue;
        cmake::get_package_config_hint(lib);
    }
}

void CppmOptions::_make_cmake_find_lib_file() {
     
}

void CppmOptions::_init() {
     make_sub_command<option::Init>().run();
}