#include "TemplateDB.hpp"

void TemplateDB::load_template(std::string template_name) {
    
    std::string template_path = "resources/actor_templates/" + template_name + ".template";
    if (!std::filesystem::exists(template_path)) {
        std::cout << "error: template " << template_name << " is missing";
        std::exit(0);
    }
    
    rapidjson::Document value;
    Utilities::read_json_file(template_path, value);
    
    templates.emplace(template_name, std::make_shared<Actor>(Actor(-1)));
    std::shared_ptr<Actor> cur_actor = templates[template_name];
    
    if (value.HasMember("name")) {
        cur_actor->name = value["name"].GetString();
    }

    //load an actor's components
    if (value.HasMember("components")) {
        if (!value["components"].IsObject()) {
            std::cout << "error: components isn't object";
            std::exit(0);
        }
        ComponentDB::load_components(cur_actor, value["components"]);
    }
}
