#include "players.h"

namespace player {

std::pair<Player::Id, Token> Players::AddPlayer(std::shared_ptr<model::Dog> _dog,
                                                std::shared_ptr<model::GameSession> _session) {
    PlayerTokens pt;
    auto token = pt.generate_token();
    Player plyr{_dog, _session};
    players_.emplace(token, plyr);

// 	Token.
    return std::make_pair(plyr.GetId(), token);
}

void Players::AddPlayer(Token token,
                        std::shared_ptr<model::Dog> _dog,
                        std::shared_ptr<model::GameSession> _session) {
    Player player{_dog, _session};
    players_.emplace(token, player);
    players_.emplace(std::move(token), std::move(player));
}

std::shared_ptr<Player> Players::FindByToken(const Token &_token) const {
    if (players_.find(_token) == players_.end())
        return nullptr;
    return std::make_shared<Player>(players_.at(_token));
}
std::shared_ptr<Player> Players::FindByDogId(const int dog_id) const {
    auto player = std::find_if(players_.begin(), players_.end(), [&dog_id](const std::pair<Token, Player>& player){
        return *player.second.GetDog()->GetId() == dog_id;});
    return player == players_.end() ? nullptr : std::make_shared<Player>(player->second);
}
std::shared_ptr<Player> Players::GetPlayer(const Player::Id id) const noexcept {
    auto player = std::find_if(players_.begin(), players_.end(),
                               [&id](const std::pair<Token, Player>& pl)
                               { return pl.second.GetId() == id;});
    return player == players_.end() ? nullptr : std::make_shared<Player>(player->second);
}

void Players::DeletePlayer(const Token& token) {
    players_.erase(token);
}

}