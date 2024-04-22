#ifndef ENGINEUTILS_H
#define ENGINEUTILS_H

#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <stdio.h>
#include <iostream>

#include "glm/glm.hpp"

#include "rapidjson/filereadstream.h"
#include "rapidjson/document.h"

class Utilities {
public:
    static void read_json_file(const std::string& path, rapidjson::Document & out_document)
    {
        FILE* file_pointer = nullptr;
    #ifdef _WIN32
        fopen_s(&file_pointer, path.c_str(), "rb");
    #else
        file_pointer = fopen(path.c_str(), "rb");
    #endif
        char buffer[65536];
        rapidjson::FileReadStream stream(file_pointer, buffer, sizeof(buffer));
        out_document.ParseStream(stream);
        std::fclose(file_pointer);

        if (out_document.HasParseError()) {
            rapidjson::ParseErrorCode errorCode = out_document.GetParseError();
            std::cout << "error parsing json at [" << path << "]" << std::endl;
            exit(0);
        }
    }
    
    static void check_file_path(const std::string path) {
        if (!std::filesystem::exists(path)) {
                std::cout << "error: " << path << " missing";
                std::exit(0);
        }
    }
    
    static uint64_t make_composite_int(glm::ivec2 coords) {
        uint32_t new_x = static_cast<uint32_t>(coords.x);
        uint32_t new_y = static_cast<uint32_t>(coords.y);
        uint64_t new_int = new_x;
        new_int = new_int << 32;
        new_int = new_int | new_y;
        return new_int;
    }
};
#endif /* Utilities_h */

