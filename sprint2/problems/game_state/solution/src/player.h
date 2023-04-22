#pragma once

// #include "dog.h"
#include "game_session.h"
#include <memory>

class Player {
public:
	using Id = util::Tagged<uint64_t, Player>;
	Player() = delete;
	Player(std::shared_ptr<model::Dog>& _dog, std::shared_ptr<model::GameSession>& _session)
        : id{next_id++}
        , dog{_dog}
        , session{_session} {};
    const Id& GetId() const noexcept { return id; }
    std::string GetName() const noexcept { return dog->GetName(); }
    const std::shared_ptr<model::Dog>& GetDog() const noexcept { return dog; }
private:
	std::shared_ptr<model::Dog> dog;
	std::shared_ptr<model::GameSession> session;
	Id id;
    static uint64_t next_id;
};
