#pragma once
#include <string>
#include <vector>
#include <chrono>
#include "../util/tagged_uuid.h"
#include <optional>
#include "../util/token.h"

namespace domain {

using PlayerId = util::Tagged<std::string, detail::TokenTag>;

class Player {
public:
    Player(PlayerId id, /*std::string token,*/ std::string name, std::chrono::milliseconds join_time, std::chrono::milliseconds stop_time)
        : id_(std::move(id))
        , name_(std::move(name))
        , join_time_(join_time)
        , stop_time_(stop_time) {
    }

    const PlayerId& GetId() const noexcept {
        return id_;
    }

    const std::string& GetName() const noexcept {
        return name_;
    }

    const std::chrono::milliseconds& GetJoinTime() const noexcept {
        return join_time_;
    }

    const std::chrono::milliseconds& GetStopTime() const noexcept {
        return stop_time_;
    }


    void SetStopTime(const std::chrono::milliseconds stop_time) {
        stop_time_ = stop_time;
    }

private:
    PlayerId id_;
    std::string name_;
    std::chrono::milliseconds join_time_;
    std::chrono::milliseconds stop_time_;
};

struct player_info {
    std::string token;
    std::string name;
    double playTime;
};

using players_info = std::vector<player_info>;

class PlayerRepository {
public:

    virtual void Save(const Player& player) = 0;
    virtual players_info Get() = 0;
    virtual players_info Get(const int stop_time) = 0;
    virtual void Update(const domain::PlayerId& id, const std::optional<std::chrono::milliseconds>& stop_time) = 0;
    virtual void Delete(const domain::PlayerId& id) = 0;

protected:
    ~PlayerRepository() = default;
};

}  // namespace domain
