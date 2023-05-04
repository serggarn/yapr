#include "game.h"
#include <cmath>

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
            sessions_.emplace_back(std::make_shared<GameSession>(session));
        } catch (...) {
            session_id_to_index_.erase(it);
            throw;
        }
    }

}

void Game::Tick(std::chrono::milliseconds delta) {
//    std::cout << "dog: " << dogs_.size() <<std::endl;
    double delta_real = delta.count() * ms_in_sec;
    for (const auto& gs : sessions_ ) {
        for (auto &dog: gs->GetDogs()) {
            auto pos = dog->GetPos();
            std::cout << "pos: " << pos.x << " : " << pos.y << std::endl;
            auto speed = dog->GetSpeed();
            Speed new_speed = speed;
            auto new_pos = Position{pos.x + (speed.vx * delta_real), pos.y + (speed.vy * delta_real)};
            std::cout << speed.vx * delta_real << " ; " << speed.vy * delta_real << std::endl;
            std::cout << "new_pos: " << new_pos.x << " : " << new_pos.y << std::endl;
            // check new pos
            auto roads = gs->GetMap()->GetRoadsByCoord(
                    Point{static_cast<int>(std::round(pos.x)), static_cast<int>(std::round(pos.y))});
            std::cout << "wee " << (roads.first == nullptr) << "; " << (roads.second == nullptr) << std::endl;
            if (roads.first == nullptr && roads.second == nullptr)
                continue;
            if (speed.vx != 0) {
                CoordReal minX, maxX;
                if (roads.second != nullptr) {
                    minX = std::min(roads.second->GetStart().x, roads.second->GetEnd().x) - Road::HALF_WIDTH;
                    maxX = std::max(roads.second->GetStart().x, roads.second->GetEnd().x) + Road::HALF_WIDTH;
                } else {
                    minX = roads.first->GetStart().x - Road::HALF_WIDTH;
                    maxX = roads.first->GetStart().x + Road::HALF_WIDTH;
                }
                if (new_pos.x <= minX || new_pos.x >= maxX)
                    new_speed.vx = 0;
                new_pos.x = new_pos.x < minX ? minX : new_pos.x;
                new_pos.x = new_pos.x > maxX ? maxX : new_pos.x;
            } else if (speed.vy != 0) {
                CoordReal minY, maxY;
                if (roads.first != nullptr) {
                    minY = std::min(roads.first->GetStart().y, roads.first->GetEnd().y) - Road::HALF_WIDTH;
                    maxY = std::max(roads.first->GetStart().y, roads.first->GetEnd().y) + Road::HALF_WIDTH;
                } else {
                    std::cout << 1 << std::endl;
                    minY = roads.second->GetStart().y - Road::HALF_WIDTH;
                    std::cout << 2 << std::endl;
                    maxY = roads.second->GetStart().y + Road::HALF_WIDTH;
                }
                if (new_pos.y <= minY || new_pos.y >= maxY)
                    new_speed.vy = 0;
                new_pos.y = new_pos.y < minY ? minY : new_pos.y;
                new_pos.y = new_pos.y > maxY ? maxY : new_pos.y;
            }
            std::cout << "new_pos res: " << new_pos.x << " : " << new_pos.y << std::endl;

            dog->SetPos(new_pos);
            if (speed != new_speed)
                dog->SetSpeed(new_speed);
        }
    }
}

} //namespace model
