#ifndef PackageManager_hpp
#define PackageManager_hpp

#include <stdio.h>
#include <string>
#include <vector>
#include <cstdlib>
#include <unordered_map>
#include <set>

#include "EngineUtils.h"

struct Package;

// **PACKAGES MUST HAVE UNIQUE NAMES CURRENTLY**

class PackageManager {
public:
    
    static void load_packages();
    
    static std::string get_package_filepath(const std::string& component_name);
    
private:
    static void populate_package_comps(Package& package);
    
    static void git_package(Package& new_package);
    
    static inline int next_package_id = 0;
    static inline std::unordered_map<int, Package> packages;
};

struct Package {
    int id;
    std::string name;
    std::string url;
    std::set<std::string> components;
};

#endif
