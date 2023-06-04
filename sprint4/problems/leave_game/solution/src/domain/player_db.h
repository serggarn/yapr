#pragma once
#include <string>
#include <vector>
#include <chrono>
#include "../util/tagged_uuid.h"
#include <optional>
#include "../util/token.h"

namespace domain {

//namespace detail {
//struct PlayerTag {};
//}  // namespace detail

//using PlayerId = util::TaggedUUID<detail::TokenTag>;
using PlayerId = util::Tagged<std::string, detail::TokenTag>;

class Player {
public:
    Player(PlayerId id, /*std::string token,*/ std::string name, std::time_t join_time)
        : id_(std::move(id))
//        , token_(token)
        , name_(std::move(name))
        , join_time_(join_time) {
    }

    const PlayerId& GetId() const noexcept {
        return id_;
    }

    const std::string& GetName() const noexcept {
        return name_;
    }

    const std::time_t& GetJoinTime() const noexcept {
        return join_time_;
    }

    const std::time_t& GetStopTime() const noexcept {
        return stop_time_;
    }

//    const std::string& GetToken() const noexcept {
//        return token_;
//    }

    void SetStopTime(const time_t stop_time) {
        stop_time_ = stop_time;
    }

private:
    PlayerId id_;
    std::string name_;
    std::time_t join_time_;
    std::time_t stop_time_;
//    std::string token_;
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
    virtual void Update(const domain::PlayerId& id, const std::optional<long>& stop_time) = 0;
    virtual void Delete(const domain::PlayerId& id) = 0;

protected:
    ~PlayerRepository() = default;
};

}  // namespace domain
