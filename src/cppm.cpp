#include"cppm.h"
#include<nieel/filesystem.h>
#include<fstream>

Cppm* Cppm::instance() {
    static Cppm instance_; 
    return &instance_;  
} 

Cppm::Cppm() : option_() { }

fs::path Cppm::find_config_file() {
    auto config_file = nieel::reverse_find_file(fs::current_path(), "cppm.yaml");
    if(!config_file) {
        std::cerr << "Can't not find cppm.yaml file\n" 
                  << std::endl;
        exit(1);
    }
    
    return *config_file;
}

void Cppm::make_config_file(cppm::Project& project) {
    std::ofstream config_file(project.path.root + "/cppm.yaml"); config_file.is_open();
    
    auto config = YAML::LoadFile(project.path.root +"/cppm.yaml");
    config["package"]["name"]     = project.package.name;
    config["package"]["version"]  = project.package.version.str();
    config["package"]["type"]     = project.package.type;
    config["package"]["thirdparty-repo"].push_back("https://github.com/INJAE/cppm_packages.git");
    
    config["compiler"]["type"] = project.compiler.type;
        
    config["builder"]["type"]  = project.builder.type;
    config[project.package.type][project.package.name]["version"] = "0.0.0.1";
    if(project.package.type != "binary") config[project.package.type][project.package.name]["type"] = "static";
    config[project.package.type][project.package.name]["source"].push_back("src/.*");
    
    config_file << config; 
    
    config_file.close();
}

void Cppm::parse_project_config() {
    auto configure_file = YAML::LoadFile(find_config_file().c_str());
    project_ = cppm::Project::parse(configure_file);
}

void Cppm::run(int argc, char** argv) {
    option_ = std::make_unique<CppmOptions>(argc, argv);
    option_->regist(); 
    option_->run();
}
