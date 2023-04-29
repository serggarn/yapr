#include "players.h"

namespace player {

std::pair<Player::Id, Token> Players::AddPlayer(std::shared_ptr<model::Dog> _dog, std::shared_ptr<model::GameSession> _session) {
    PlayerTokens pt;
    auto token = pt.generate_token();
    Player plyr{_dog, _session};
    players.emplace(token, plyr);

// 	Token.
    return std::make_pair(plyr.GetId(), token);
}

std::shared_ptr<Player> Players::FindByToken(const Token &_token) {
    if (players.find(_token) == players.end())
        return nullptr;
    return std::make_shared<Player>(players.at(_token));
}
}