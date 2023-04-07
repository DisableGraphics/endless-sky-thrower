#pragma once
#include <iostream>
#include <vector>
#include "functions.hpp"

typedef struct
{
    std::string name;
    std::string author;
    
    std::string version;

    std::string description;
    std::string short_description;

    std::string homepage;

    std::string license;

    std::string url;
} Plugin_ID;

namespace global
{
    inline bool lock;
    inline std::vector<Plugin_ID> plugins;
    const std::string config_dir = Functions::get_home_dir() + "/.esthrower/";
}