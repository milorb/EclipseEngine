#ifndef Engine_h
#define Engine_h

#include <stdio.h>
#include <string>
#include <iostream>
#include <sstream>
#include <unordered_set>

#include "glm/glm.hpp"
#include "FontDB.hpp"
#include "AudioDB.hpp"
#include "ImageDB.hpp"
#include "SceneDB.hpp"
#include "Renderer.hpp"
#include "ComponentDB.hpp"
#include "Input.hpp"
#include "Event.hpp"
#include "Physics.hpp"
#include "PackageManager.hpp"

class Engine {
public:
    Engine() : change_scene(false), quit(false){};
    
    void game_loop();
    
private:
    void init_game_config();
    void initialize();

    void input();
    void update();
    void render();
    
    rapidjson::Document game_config;
    
    std::string next_scene_name;
    std::string game_title = "";
    std::string user_input;
    
    Renderer Renderer;
    SceneDB CurScene;
    ComponentDB cDB;
    ImageDB Images;
    AudioDB Audio;

    bool change_scene;
    bool quit;
};

#endif /* Engine_h */
