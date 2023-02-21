#pragma once
#include <iostream>
#include <vector>

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
}