//
// Created by serg on 03.05.23.
//

#ifndef GAME_SERVER_JSON_NAMES_H
#define GAME_SERVER_JSON_NAMES_H

#include <iostream>

namespace json_tags {
    static const std::string id = "id";
    static const std::string name = "name";
    static const std::string roads = "roads";
    static const std::string maps = "maps";
    static const std::string buildings = "buildings";
    static const std::string offices = "offices";
    static const std::string x0 = "x0";
    static const std::string y0 = "y0";
    static const std::string y1 = "y1";
    static const std::string x1 = "x1";
    static const std::string x = "x";
    static const std::string y = "y";
    static const std::string w = "w";
    static const std::string h = "h";
    static const std::string offsetX = "offsetX";
    static const std::string offsetY = "offsetY";
    static const std::string code = "code";
    static const std::string message = "message";
    static const std::string pos = "pos";
    static const std::string speed = "speed";
    static const std::string dir = "dir";
    static const std::string players = "players";
    static const std::string timeDelta = "timeDelta";
    static const std::string move = "move";
    static const std::string defaultDogSpeed = "defaultDogSpeed";
    static const std::string dogSpeed = "dogSpeed";
    static const std::string authToken = "authToken";
    static const std::string playerId = "playerId";
    static const std::string userName = "userName";
    static const std::string mapId = "mapId";
//    static const std::string  = "";
//    static const std::string  = "";
} // namespace json_tags
#endif //GAME_SERVER_JSON_NAMES_H
