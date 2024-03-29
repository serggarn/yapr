#pragma once 
// #include "model.h"
#include "game_session.h"
#include "model/loot_generator.h"

namespace model {
    static constexpr double ms_in_sec = 0.001;

	class Game {

public:
    using Maps = std::vector<Map>;
	using Sessions = std::vector<std::shared_ptr<GameSession>>;
	

    void AddMap(const Map& map);

    const Maps& GetMaps() const noexcept {
        return maps_;
    }
    const Sessions& GetSessions() const noexcept {
            return sessions_;
    }

    const std::shared_ptr<Map> FindMap(const Map::Id& id) const noexcept {
//        std::cout <<"FindMap" <<std::endl;
        if (auto it = map_id_to_index_.find(id); it != map_id_to_index_.end()) {
            return std::make_shared<Map>(maps_.at(it->second));
        }
        return nullptr;
    }
    
    void AddSession(const GameSession& session);

	const std::shared_ptr<GameSession> FindGameSession(const GameSession::Id& id) const noexcept {
        if (auto it = session_id_to_index_.find(id); it != session_id_to_index_.end()) {
            return sessions_.at(it->second);
        }
        return nullptr;
    }

    void Tick(std::chrono::milliseconds delta);

    inline void AddLootGeneratorConfig(const LootGeneratorConfig& config) {
        loot_generator_config_ = std::move(config);
    }

    const LootGeneratorConfig& GetLootGeneratorConfig() const noexcept {
        return loot_generator_config_;
    }

private:
    using MapIdHasher = util::TaggedHasher<Map::Id>;
    using MapIdToIndex = std::unordered_map<Map::Id, size_t, MapIdHasher>;
    using SessionIdHasher = util::TaggedHasher<GameSession::Id>;
    using SessionIdToIndex = std::unordered_map<GameSession::Id, size_t, SessionIdHasher>;

    Maps maps_;
    MapIdToIndex map_id_to_index_;
	Sessions sessions_;
	SessionIdToIndex session_id_to_index_;
    LootGeneratorConfig loot_generator_config_ {};

};
} // namespace model
