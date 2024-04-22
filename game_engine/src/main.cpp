//
//  main.cpp
//  game_engine
//
//  Created by Milo Baumhardt on 1/24/24.
//

#include <stdio.h>
#include <iostream>

#include "Engine.hpp"



int main(int argc, char * argv[]) {
    
    Engine engine;
    engine.game_loop();
    
    return 0;
}
