#ifndef TemplateDB_hpp
#define TemplateDB_hpp

#include <unordered_map>
#include <memory>
#include <stdio.h>
#include <string>
#include <list>

#include "Actor.hpp"
#include "EngineUtils.h"
#include "ImageDB.hpp"
#include "ComponentDB.hpp"

class TemplateDB {
public:
    static inline std::unordered_map<std::string, std::shared_ptr<Actor>> templates;
    static void load_template(std::string file_path);
};

#endif /* TemplateDB_hpp */
