#include "config/binary.h"
#include "config/dependency.h"
#include "cmake/generator.h"
#include <nieel/string.hpp>
#include <nieel/util/hash.hpp>
#include <nieel/filesystem.h>

namespace cppm
{
    Binary Binary::parse(YAML::Node& node, std::string name) {
        using namespace nieel;
        auto root_path = nieel::reverse_find_file(fs::current_path(), "cppm.yaml")->parent_path().string();
        Binary binary;
        binary.name = name;
        for(auto it : node["binary"][binary.name]) {
            auto property = it.first.as<std::string>().c_str(); 
            switch(hash(property))
            {
            case hash("version"):
                binary.version = it.second.as<std::string>();
                break;
            case hash("include"):
                for(auto include : node["binary"][name]["include"]){
                    binary.include.push_back(include.as<std::string>());
                }
                break;
            case hash("source"):
                for(auto source : node["binary"][name]["source"]){
                    auto sources = nieel::find_files(root_path, std::regex(source.as<std::string>()), false);
                    nieel::insert(binary.source, sources);
                }
                break;
            case hash("dependencies"):
                for(auto dependency : node["binary"][name]["dependencies"]){
                    binary.dependencies.emplace_back(cppm::parse_dependency(dependency.as<std::string>()));
                }
                break;
            }
        }
        return binary;
    }
    
    std::vector<Binary> Binary::parse_binaries(YAML::Node& node) {
        std::vector<Binary> binaries;
        for(auto it : node["binary"]) {
            binaries.push_back(Binary::parse(node, it.first.as<std::string>()));
        }
        return binaries;
    }
    
    std::string Binary::cmake_script() {
        using namespace cmake;
        using trans_type = std::vector<std::string>;
        auto project = Cppm::instance()->project();
        if(project.binaries.empty()) return "";
        std::stringstream output("");
        output << set("thirdparty", nieel::str::accumulate(nieel::transform<trans_type>(dependencies, [](auto dep){ return dep.cmake_name;}), std::string{"\n\t"}) + "\n") << endl()
               << set("source"    , nieel::str::accumulate(source, std::string{"\n\t"}) + "\n") << endl()
               << "build_binary(" << name << " \"" << get("source") << "\" \"" << get("thirdparty") << "\")" << endl()
               ;
        return output.str();
    }
}