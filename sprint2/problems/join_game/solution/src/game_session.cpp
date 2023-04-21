#include "game_session.h"

namespace model {
using namespace std::literals;

void GameSession::AddDog(const player::Dog& dog)
{
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
