#include "players.h"

//Players::cur_id = 0;

std::pair<Player::Id, Token> Players::AddPlayer(std::shared_ptr<player::Dog> _dog, std::shared_ptr<model::GameSession> _session ) {
    PlayerTokens pt;
    auto token = pt.generate_token();
    Player plyr {_dog, _session};
    players.emplace(plyr.GetId(), token);

// 	Token.
	return std::make_pair(plyr.GetId(), token);
}
