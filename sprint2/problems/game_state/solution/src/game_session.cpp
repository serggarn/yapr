#include "game_session.h"
#include <random>

namespace model {
using namespace std::literals;

const Point GameSession::GetRandomPointFromRoads() const {
    std::random_device rd;   // non-deterministic generator
    std::mt19937 gen(rd());  // to seed mersenne twister.
    std::uniform_int_distribution<model::Dimension> dist_road(0, map_->GetRoads().size() - 1);
    auto road = map_->GetRoads().at(dist_road(gen));
    std::uniform_int_distribution<model::Dimension> dist_x(road.GetStart().x, road.GetEnd().x);
    std::uniform_int_distribution<model::Dimension> dist_y(road.GetStart().y, road.GetEnd().y);
    return {dist_x(gen), dist_y(gen)};
}

void GameSession::AddDog(const std::string& dog_name)
{
    auto point = GetRandomPointFromRoads();
    model::Position crd{ static_cast<CoordReal>(point.x), static_cast<CoordReal>(point.y)};
    Dog dog(dog_name, crd);
    const size_t index = dogs_.size();
    if (auto [it, inserted] = dog_id_to_index_.emplace(dog.GetId(), index); !inserted) {
        throw std::invalid_argument("Session with id "s + *dog.GetId() + " already exists"s);
    } else {
        try {
            dogs_.emplace_back(std::move(dog));
        } catch (...) {
            dog_id_to_index_.erase(it);
            throw;
        }
    }

}

} // namespace model
