#include "game_session.h"
#include <random>
#include <chrono>

namespace model {
using namespace std::literals;
using namespace std::chrono_literals;
static constexpr double ms_in_sec = 0.001;

const Point GameSession::GetRandomPointFromRoads() const {
    auto fst_road = map_->GetRoads().front();
    return {fst_road->GetStart().y, fst_road->GetStart().y};
    std::random_device rd;   // non-deterministic generator
    std::mt19937 gen(rd());  // to seed mersenne twister.
    std::uniform_int_distribution<model::Dimension> dist_road(0, map_->GetRoads().size() - 1);
    auto road = map_->GetRoads().at(dist_road(gen));
    auto minX = std::min(road->GetStart().x, road->GetEnd().x);
    auto maxX = std::min(road->GetStart().x, road->GetEnd().x);
    auto minY = std::min(road->GetStart().y, road->GetEnd().y);
    auto maxY = std::min(road->GetStart().y, road->GetEnd().y);
    std::uniform_int_distribution<model::Dimension> dist_x(minX, maxX);
    std::uniform_int_distribution<model::Dimension> dist_y(minY, maxY);
    return {dist_x(gen), dist_y(gen)};
}

void GameSession::AddDog(const std::string& dog_name)
{
    auto point = GetRandomPointFromRoads();
    std::cout << " point ok" << std::endl;
    model::Position crd{ static_cast<CoordReal>(point.x), static_cast<CoordReal>(point.y)};
    std::cout << " crd ok" << std::endl;
    auto dog = std::make_shared<Dog>(dog_name, crd);
    std::cout << " dog ok" << std::endl;
    const size_t index = dogs_.size();
    std::cout << "dogs_.size(): " <<dogs_.size() <<std::endl;
    std::cout << " index ok" << std::endl;
    if (auto [it, inserted] = dog_id_to_index_.emplace(dog->GetId(), index); !inserted) {
        std::cout << "dog_id_to_index_ not inserted" << std::endl;
        throw std::invalid_argument("Session with id "s + *dog->GetId() + " already exists"s);
    } else {
        try {
            dogs_.emplace_back(std::move(dog));
            std::cout<< "emplace_back ok " << dogs_.size() << std::endl;

        } catch (...) {
            std::cout << "catch" << std::endl;

            dog_id_to_index_.erase(it);
            throw;
        }
    }

}

void GameSession::Tick(int delta) {
    std::cout << "dog: " << dogs_.size() <<std::endl;
    double delt_r = delta * ms_in_sec;

    for (auto& dog :dogs_) {
        auto pos = dog->GetPos();
        std::cout << "pos: " << pos.x << " : " << pos.y <<std::endl;
        auto speed = dog->GetSpeed();
        Speed new_speed = speed;
        auto new_pos = Position {pos.x + (speed.vx * delt_r ), pos.y + (speed.vy * delt_r)};
        std::cout << speed.vx * delt_r << " ; " << speed.vy * delt_r << std::endl;
        std::cout << "new_pos: " << new_pos.x << " : " << new_pos.y <<std::endl;
        // check new pos
        auto roads = map_->GetRoadsByCoord(Point{static_cast<int>(pos.x),static_cast<int>(pos.y)});

        if ( roads.first == nullptr && roads.second == nullptr )
            continue;
        if (speed.vx != 0 ) {
            CoordReal minX, maxX;
            if (roads.second != nullptr) {
                minX = std::min(roads.second->GetStart().x, roads.second->GetEnd().x) - Road::HALF_WIDTH;
                maxX = std::max(roads.second->GetStart().x, roads.second->GetEnd().x) + Road::HALF_WIDTH;
            }
            else {
                minX = roads.second->GetStart().x - Road::HALF_WIDTH;
                maxX = roads.second->GetStart().x + Road::HALF_WIDTH;
            }
            if ( new_pos.x <= minX || new_pos.x >= maxX )
                new_speed.vx = 0;
            new_pos.x = new_pos.x < minX ? minX : new_pos.x;
            new_pos.x = new_pos.x > maxX ? maxX : new_pos.x;
        }
        else if ( speed.vy != 0 ) {
            CoordReal minY, maxY;
            if (roads.first != nullptr) {
                minY = std::min(roads.first->GetStart().y, roads.first->GetEnd().y) - Road::HALF_WIDTH;
                maxY = std::max(roads.first->GetStart().y, roads.first->GetEnd().y) + Road::HALF_WIDTH;
            }
            else {
                minY = roads.first->GetStart().y - Road::HALF_WIDTH;
                maxY = roads.first->GetStart().y + Road::HALF_WIDTH;
            }
            if ( new_pos.y <= minY || new_pos.y >= maxY )
                new_speed.vy = 0;
            new_pos.y = new_pos.y < minY ? minY : new_pos.y;
            new_pos.y = new_pos.y > maxY ? maxY : new_pos.y;
        }
        std::cout << "new_pos res: " << new_pos.x << " : " << new_pos.y <<std::endl;

        dog->SetPos(new_pos);
        if (speed != new_speed)
            dog->SetSpeed(new_speed);
    }
}
} // namespace model
