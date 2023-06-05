#pragma once

#include "../game_play/game_session.h"
#include <memory>

class Player {
public:
	using Id = util::Tagged<uint64_t, Player>;
    using Loots = std::vector<model::Loot>;
	Player() = delete;
    Player(std::shared_ptr<model::Dog>& dog, std::shared_ptr<model::GameSession>& session)
            : id_{next_id++}
            , dog_{dog}
            , session_{session} {};
    Player(Id id, std::shared_ptr<model::Dog> dog, std::shared_ptr<model::GameSession> session)
            : id_{id}
            , dog_{std::move(dog)}
            , session_{std::move(session)} {next_id = std::max(next_id, *id); next_id++; };
    const Id& GetId() const noexcept { return id_; }
    std::string GetName() const noexcept { return dog_->GetName(); }
    const std::shared_ptr<model::Dog>& GetDog() const noexcept { return dog_; }
    const std::shared_ptr<model::GameSession> GetSession() const noexcept { return session_; }
    const Loots& GetLoots() const noexcept { return loots_; }
    void AddLoot(model::Loot loot) noexcept;
    void GiveLoots();
    void AddScore(const size_t score) { score_ += score; }
    size_t GetScore() const noexcept { return score_; }
private:
	std::shared_ptr<model::Dog> dog_;
	std::shared_ptr<model::GameSession> session_;
    Loots loots_;
	Id id_;
    static uint64_t next_id;
    size_t score_ = 0;
};
