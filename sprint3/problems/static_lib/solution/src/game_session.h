#pragma once
#include "model/model.h"
#include "model/dog.h"
#include <vector>
#include <memory>
#include <chrono>
#include "model/loot_generator.h"

// using namespace model;
namespace model {

    struct LootGeneratorConfig {
        double period;
        double probability;
    };

    class GameSession {
public:
// 	using Id = util::Tagged<model::Map, GameSession>;
	using Id = util::Tagged<std::string, GameSession>;
	using Dogs = std::vector<std::shared_ptr<model::Dog>>;
    using LG = loot_gen::LootGenerator;
//    using ms =
// 	GameSession(Id id, std::string map) 
	
	GameSession(Id id, std::shared_ptr<model::Map> map,
                model::LootGeneratorConfig config)
		: id_ {std::move(id)}
		, map_{std::move(map)} {
             loot_generator  = std::make_shared<LG>(
                     LG::TimeInterval(static_cast<unsigned>(config.period * 1000)),
                     config.probability);
		}
	void AddDog(const std::string& dog_name);
	
    const Id& GetId() const noexcept {
        return id_;
    }
	const Dogs& GetDogs() const noexcept {
        return dogs_;
    }
    const std::shared_ptr<Map>& GetMap() const noexcept {
        return map_;
    }
    
    std::shared_ptr<model::Dog> FindDog(const model::Dog::Id& id) const noexcept {
        if (auto it = dog_id_to_index_.find(id); it != dog_id_to_index_.end()) {
            return dogs_.at(it->second);
        }
        return nullptr;
    }

    const std::shared_ptr<loot_gen::LootGenerator>& GetLootGenerator() const noexcept{
        return loot_generator;
    }

    void AddLoot();

private:
	using DogIdHasher = util::TaggedHasher<model::Dog::Id>;
    using DogIdToIndex = std::unordered_map<model::Dog::Id, size_t, DogIdHasher>;

	Id id_;
	DogIdToIndex dog_id_to_index_;
	Dogs dogs_;
	std::shared_ptr<model::Map> map_;
    std::shared_ptr<loot_gen::LootGenerator> loot_generator;
    const Point GetRandomPointFromRoads() const;
    const Loot::Type GetRandomLootType() const;
    const Point GetStartPointFromFirstRoad() const;

};
} // namespace model
