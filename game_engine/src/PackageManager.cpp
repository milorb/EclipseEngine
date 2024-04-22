#include "PackageManager.hpp"

void PackageManager::load_packages() {
    
    const std::string config_path = "resources/packages.config";
    
    if (!std::filesystem::exists(config_path)) {
        std::cout << "no packages.config" << std::endl;
        return;
    }
    
    rapidjson::Document packages_config;
    Utilities::read_json_file(config_path, packages_config);
    
    const rapidjson::Value& package_array = packages_config["packages"];
    
    for (auto& package_info : package_array.GetArray()) {
        Package new_package;
        if (package_info.HasMember("name")) {
            new_package.name = package_info["name"].GetString();
            new_package.id = next_package_id;
            next_package_id++;
        } else {
            std::cout << "error: must specify package name";
            std::exit(0);
        }
        
        if (package_info.HasMember("url")) {
            new_package.url = package_info["url"].GetString();
        } else {
            std::cout << "error: must specify package url";
            std::exit(0);
        }
        
        git_package(new_package);
        populate_package_comps(new_package);
        packages.emplace(new_package.id, new_package);
    }
}

void PackageManager::git_package(Package& new_package) {
    
    // add the submodule if it hasn't been yet
    std::string potential_path = "resources/packages/" + new_package.name;
    
    if (!std::filesystem::exists(potential_path)) {
        
        std::string clone_command = "cd resources/packages && \
                                    git submodule add -f "
        + new_package.url;
        
        int clone_result = std::system(clone_command.c_str());
        
        if (clone_result != 0) {
            std::cout << "error: failed to clone package repository";
            std::cout << "double check url: " + new_package.url;
            std::exit(0);
        }
        else {
            char delim = '/';
            size_t repo_name_idx =
            new_package.url.find_last_of(delim);
            
            std::string original_repo_name =
            new_package.url.substr(repo_name_idx + 1);
            
            size_t git_idx = original_repo_name.find(".git");
            original_repo_name.erase(git_idx, original_repo_name.length());
            
            std::string rename_command = "cd resources/packages && mv "
            + original_repo_name
            + " "
            + new_package.name;
            
            std::system(rename_command.c_str());
        }
        // git pull
        std::system("cd resources/packages && git pull origin");
    }
}

void PackageManager::populate_package_comps(Package &package) {
    
    std::string package_dir = "resources/packages/" 
                            + package.name
                            + "/components.list";
    
    if (!std::filesystem::exists(package_dir)) {
        std::cout << "package repository must contain \
                        a JSON titled components.list, \
                        listing the name of each component";
        exit(0);
    }
    
    rapidjson::Document components;
    Utilities::read_json_file(package_dir, components);
    
    for (auto& component_name : components.GetArray()) {
        package.components.insert(component_name.GetString());
    }
}

std::string PackageManager::get_package_filepath(const std::string& component_name) {
    int p_id = -1;
    for (int i = 0; i < next_package_id; ++i) {
        
        if (packages[i].components.find(component_name)
            != packages[i].components.end()) {
            
            p_id = i;
        }
    }
    
    if (p_id == -1) {
        return "";
    }
    
    std::string component_path = "resources/packages/" 
                                + packages[p_id].name
                                + "/"
                                + component_name
                                + ".lua";
    
    return component_path;
}
