#include "game_session.h"
#include <random>
#include <chrono>
#include "system/settings.h"

namespace model {
using namespace std::literals;
using namespace std::chrono_literals;

const Point GameSession::GetRandomPointFromRoads() const {
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

const Loot::Type GameSession::GetRandomLootType() const {
    std::random_device rd;   // non-deterministic generator
    std::mt19937 gen(rd());  // to seed mersenne twister.
    std::uniform_int_distribution<Loot::Type> type(0, map_->GetLootsDefinitions().size() - 1);
    return type(gen);
}

const Point GameSession::GetStartPointFromFirstRoad() const {
    auto fst_road = map_->GetRoads().front();
    return {fst_road->GetStart().y, fst_road->GetStart().y};
}

void GameSession::AddDog(const std::string& dog_name)
{
    auto sett = settings::Settings::GetInstance();
    std::cout << "randomiz? " << sett->IsRandomStart() << "; test_env" << sett->IsTestEnv() <<std::endl;
    auto point = sett->IsRandomStart() ? GetRandomPointFromRoads() : GetStartPointFromFirstRoad();
//    settings::randomize_start = true;
    std::cout << " point ok: " << point.x << "; " << point.y << std::endl;
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

void GameSession::AddLoot() {
    auto point = GetRandomPointFromRoads();
    auto position = Position{static_cast<CoordReal>(point.x),
                             static_cast<CoordReal>(point.y)};
    auto loot = Loot(GetRandomLootType(), position);
    map_->AddLoot(loot);
}

} // namespace model
