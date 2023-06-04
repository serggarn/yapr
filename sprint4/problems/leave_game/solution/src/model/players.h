#pragma once

#include "player.h"
#include "../util/token.h"
#include <map>

namespace player {
class Players {
public:
    using TPlayers = std::map<Token, Player>;

    std::pair<Player::Id, Token> AddPlayer(std::shared_ptr<model::Dog> _dog,
                                           std::shared_ptr<model::GameSession> _session);
    void AddPlayer(Token token,
                   std::shared_ptr<model::Dog> _dog,
                   std::shared_ptr<model::GameSession> _session);

    std::shared_ptr<Player> FindByDogAndMapId(const int dog_id, const std::string &map_id);

    std::shared_ptr<Player> FindByToken(const Token &_token) const ;

    [[ nodiscard ]] const TPlayers &GetPlayers() const noexcept { return players_; }

    std::shared_ptr<Player> GetPlayer(const Player::Id id) const noexcept;

    void DeletePlayer(const Token& token);

private:
    TPlayers players_;
};

}