#pragma once

#include "player.h"
#include "token.h"
#include <map>


class Players {
private:
	std::map<Player::Id, Token> players;
public:
	std::pair<Player::Id, Token> AddPlayer(std::shared_ptr<player::Dog> _dog, std::shared_ptr<model::GameSession> _session );
	std::shared_ptr<Player> FindByDogAndMapId(const int dog_id, const std::string& map_id);
	std::shared_ptr<Player> FindByToken(Token _token);
};
