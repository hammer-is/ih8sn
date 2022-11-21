#include <fstream>
#include <iostream>
#include <vector>
#include <regex>
#include <ctime>

#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include "_system_properties.h"
#include "properties.h"

void property_override(std::string name, std::string value, bool add = false) {
    auto pi = (prop_info *) __system_property_find(name.c_str());

    if (pi != nullptr) {
        __system_property_update(pi, value.c_str(), value.length());
    } else if (add) {
        __system_property_add(name.c_str(), name.length(), value.c_str(), value.length());
    }
}

std::string rtrim(const std::string &s) {
    return std::regex_replace(s, std::regex("\\s+$"), std::string(""));
}

std::vector<std::tuple<std::string, std::string, std::string>> load_config(std::string stage) {
    std::vector<std::tuple<std::string, std::string, std::string>> config;

    bool found = false;

    if (std::ifstream file("/system/etc/ih8sn.conf"); file.good()) {
        std::string line;

        while (std::getline(file, line)) {            

            if (line[0] == '#') {
                continue;
            }
            
            line = rtrim(line);

            if (line.compare(stage) == 0) {
                found = true;
                continue;
            }
            
            if (found) {
                if (const auto separator1 = line.find(','); separator1 != std::string::npos) {
                    if (const auto separator2 = line.substr(separator1 + 1).find('='); separator2 != std::string::npos) {
                        config.push_back(std::make_tuple(line.substr(0, separator1), line.substr(separator1 + 1, separator2), line.substr(separator1 + separator2 + 2)));
                    }
                    else {
                        config.push_back(std::make_tuple(line.substr(0, separator1), line.substr(separator1 + 1), ""));
                    }
                }
                else {
                    found = false;
                }
            }
        }
    }

    return config;
}

static void handle_existing_prop(const char* name, const char* value, void* cookie) {

    std::vector<std::tuple<std::string, std::string, std::string>> *config = static_cast<std::vector<std::tuple<std::string, std::string, std::string>>*>(cookie);

    std::string newvalue = value;
    bool del = false;

    for(std::tuple<std::string, std::string, std::string> i : *config) {

        if (std::get<0>(i).compare("set") == 0) {
            if (std::regex_match(name, std::regex(std::get<1>(i)))) {
                newvalue = std::get<2>(i);
            }
        }
        else if (std::get<0>(i).compare("replace") == 0) {
            if (std::regex_search(newvalue, std::regex(std::get<1>(i)))) {
                newvalue = std::regex_replace(newvalue, std::regex(std::get<1>(i)), std::get<2>(i));
            }
        }
        else if (std::get<0>(i).compare("delete") == 0) {
            if (std::regex_match(name, std::regex(std::get<1>(i)))) {
                del = true;
            }
        }   
    }

    if (del)
    {       
        std::cout << "delete: " << name << "=" << value << "\n";
         __system_property_delete(name, false);
    }
    else if (newvalue.compare(value) != 0)
    {
        std::cout << "set: " << name << "=" << newvalue << "\n";
        property_override(name, newvalue);
    }
}

int main(int argc, char *argv[]) {
    if (__system_properties_init()) {
        return -1;
    }

    if (argc != 2) {
        return -1;
    }

    std::time_t result = std::time(nullptr);
    std::cout << "ih8sn stage=" << argv[1] << " " << std::asctime(std::localtime(&result));

    const auto config = load_config(argv[1]);

    std::cout << config.size() << " config entries\n";

    property_list(handle_existing_prop, (void *)&config);

    for(std::tuple<std::string, std::string, std::string> i : config) {

        if (std::get<0>(i).compare("add") == 0) {
            std::cout << "add: " << std::get<1>(i) << "=" << std::get<2>(i) + "\n";
            property_override(std::get<1>(i), std::get<2>(i), true);            
        }
    }

    return 0;
}