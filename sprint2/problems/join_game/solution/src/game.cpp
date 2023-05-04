#include "game.h"

using namespace std::literals;

namespace model {
void Game::AddMap(const Map& map)
{
    const size_t index = maps_.size();
    if (auto [it, inserted] = map_id_to_index_.emplace(map.GetId(), index); !inserted) {
        throw std::invalid_argument("Map with id "s + *map.GetId() + " already exists"s);
    } else {
        try {
            maps_.emplace_back(std::move(map));
        } catch (...) {
            map_id_to_index_.erase(it);
            throw;
        }
    }
}

void Game::AddSession(const GameSession& session)
{
    const size_t index = sessions_.size();
    if (auto [it, inserted] = session_id_to_index_.emplace(session.GetId(), index); !inserted) {
        throw std::invalid_argument("Session with id "s + *session.GetId() + " already exists"s);
    } else {
        try {
            sessions_.emplace_back(std::move(session));
        } catch (...) {
            session_id_to_index_.erase(it);
            throw;
        }
    }

}

} //namespace model
