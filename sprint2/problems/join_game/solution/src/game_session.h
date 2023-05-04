#pragma once
#include "model.h"
#include "dog.h"
#include <vector>
#include <memory>

// using namespace model;
namespace model {

class GameSession {
public:
// 	using Id = util::Tagged<model::Map, GameSession>;
	using Id = util::Tagged<std::string, GameSession>;
	using Dogs = std::vector<player::Dog>;
// 	GameSession(Id id, std::string map) 
	
	GameSession(Id id, std::shared_ptr<model::Map> map) 
		: id_ {std::move(id)}
		, map_{std::move(map)} {
		}
	void AddDog(const player::Dog& dog);
	
    const Id& GetId() const noexcept {
        return id_;
    }
	const Dogs& GetDogs() const noexcept {
        return dogs_;
    }
    const std::shared_ptr<Map>& GetMap() const noexcept {
        return map_;
    }
    
    std::shared_ptr<player::Dog> FindDog(const player::Dog::Id& id) const noexcept {
        if (auto it = dog_id_to_index_.find(id); it != dog_id_to_index_.end()) {
            return std::make_shared<player::Dog>(dogs_.at(it->second));
        }
        return nullptr;
    }


private:
	using DogIdHasher = util::TaggedHasher<player::Dog::Id>;
    using DogIdToIndex = std::unordered_map<player::Dog::Id, size_t, DogIdHasher>;

	Id id_;
	DogIdToIndex dog_id_to_index_;
	Dogs dogs_;
	std::shared_ptr<model::Map> map_;
// 	std::string map_;
};
} // namespace model
