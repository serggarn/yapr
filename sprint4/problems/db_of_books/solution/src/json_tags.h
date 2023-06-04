//
// Created by serg on 31.05.23.
//

#ifndef POSTGRES_JSON_TAGS_H
#define POSTGRES_JSON_TAGS_H
#include <iostream>
#include <map>

namespace tags {
    static const std::string id = "id";
    static const std::string action = "action";
    static const std::string payload = "payload";
    static const std::string title = "title";
    static const std::string author = "author";
    static const std::string year = "year";
    static const std::string ISBN = "ISBN";
    static const std::string result = "result";
//static const std::string  = "";

    static const std::map<bool, std::string> result_map{
            {true,  "true"},
            {false, "false"}
    };
}
#endif //POSTGRES_JSON_TAGS_H
