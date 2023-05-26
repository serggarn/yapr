//
// Created by serg on 20.05.23.
//

#ifndef GAME_SERVER_LOOTDEFINITION_H
#define GAME_SERVER_LOOTDEFINITION_H
#include <boost/json.hpp>

namespace json = boost::json;
class LootDefinition {
public:
    LootDefinition(const json::object& defenition) : defenition_(std::move(defenition)) {};
    const json::object& GetDefiinition() const noexcept {
        return defenition_;
    } 
private:
    json::object defenition_;
};


#endif //GAME_SERVER_LOOTDEFINITION_H
