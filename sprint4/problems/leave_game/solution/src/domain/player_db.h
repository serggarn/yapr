#pragma once
#include <string>
#include <vector>
#include <chrono>
#include "../util/tagged_uuid.h"

namespace domain {

namespace detail {
struct PlayerTag {};
}  // namespace detail

using PlayerId = util::TaggedUUID<detail::PlayerTag>;

class Player {
public:
    Player(PlayerId id, std::time_t join_time)
        : id_(std::move(id))
        , join_time_(join_time) {
    }

    const PlayerId& GetId() const noexcept {
        return id_;
    }

    const std::time_t& GetJoinTime() const noexcept {
        return join_time_;
    }

    const std::time_t& GetStopTime() const noexcept {
        return stop_time_;
    }

    void SetStopTime(const time_t stop_time) {
        stop_time_ = stop_time;
    }

private:
    PlayerId id_;
    std::time_t join_time_;
    std::time_t stop_time_;
};

class PlayerRepository {
public:
    using authors_info = std::vector<std::pair<std::string, std::string>>;
    virtual void Save(const Player& player) = 0;
//    virtual authors_info Get() = 0;

protected:
    ~PlayerRepository() = default;
};

}  // namespace domain
