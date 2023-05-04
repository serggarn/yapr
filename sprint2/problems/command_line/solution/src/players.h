#pragma once

#include "player.h"
#include "token.h"
#include <map>

namespace player {
using TPlayers = std::map<Token, Player>;
class Players {
public:

    std::pair<Player::Id, Token> AddPlayer(std::shared_ptr<model::Dog> _dog, std::shared_ptr<model::GameSession> _session);

    std::shared_ptr<Player> FindByDogAndMapId(const int dog_id, const std::string &map_id);

    std::shared_ptr<Player> FindByToken(const Token &_token);

    const TPlayers &GetPlayers() { return players; }

private:
    TPlayers players;
};

}