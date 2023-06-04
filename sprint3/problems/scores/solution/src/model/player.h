#pragma once

// #include "dog_.h"
#include "../game_play/game_session.h"
#include <memory>

class Player {
public:
	using Id = util::Tagged<uint64_t, Player>;
    using Loots = std::vector<model::Loot>;
	Player() = delete;
	Player(std::shared_ptr<model::Dog>& dog, std::shared_ptr<model::GameSession>& session)
        : id{next_id++}
        , dog_{dog}
        , session_{session} {};
    const Id& GetId() const noexcept { return id; }
    std::string GetName() const noexcept { return dog_->GetName(); }
    const std::shared_ptr<model::Dog>& GetDog() const noexcept { return dog_; }
    const std::shared_ptr<model::GameSession> GetSession() const noexcept { return session_; }
    const Loots& GetLoots() const noexcept { return loots_; }
    void AddLoot(model::Loot& loot) noexcept;
    void GiveLoots();
    size_t GetScore() const noexcept { return score; }
private:
	std::shared_ptr<model::Dog> dog_;
	std::shared_ptr<model::GameSession> session_;
    Loots loots_;
	Id id;
    static uint64_t next_id;
    size_t score = 0;
};
