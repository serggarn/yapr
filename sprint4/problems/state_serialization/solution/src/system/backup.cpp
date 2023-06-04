//
// Created by serg on 29.05.23.
//

#include "backup.h"
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include "../model/model_serialization.h"
#include "../token.h"
#include <fstream>
#include <filesystem>
#include <memory>

using namespace serialization;

std::shared_ptr<Backup> Backup::backup_{};
std::shared_ptr<Backup> Backup::GetInstance() {
    if (backup_ != nullptr)
        return backup_;
    backup_ = std::shared_ptr<Backup>(new Backup());
    return backup_;
}

void Backup::Init(std::string path, std::chrono::milliseconds period) {
    path_ = std::move(path);
    save_period_ = period;
}


bool Backup::Save(const model::Game& game_, const player::Players& players_) {

    std::string tmp_file = path_ + ".tmp";
    std::ofstream ofs(tmp_file);
    {
        boost::archive::text_oarchive oa{ofs};
//        for ( const auto& map : game_.GetMaps() ) {
////            std::cout <<"03" <<std::endl;
//
//            // write count of loots on map
//            oa << map.GetLoots().size();
//            std::cout << "map: " << *map.GetId()<<std::endl;
//            auto loots = map.GetLoots();
//            std::cout << "count of loots in map: " << map.GetLoots().size() << "; " << loots.size() <<std::endl;
////            std::cout <<"02" <<std::endl;
//            for ( const auto& loot : map.GetLoots() ) {
//                oa << LootRepr(loot);
////                std::cout <<"01" <<std::endl;
//            }
//        }
//        std::cout <<"1" <<std::endl;
//        auto game_sessions = game_.GetSessions();
        // write count of players
        oa << players_.GetPlayers().size();
//        std::cout <<"2" <<std::endl;
        for ( const auto& player : players_.GetPlayers() ) {
            // write token
            oa << *player.first;
            auto dog = player.second.GetDog();
//            std::cout <<"3" <<std::endl;
            oa << DogRepr(*dog);
            // write map id
//            std::cout <<"4" <<std::endl;
            oa << *player.second.GetSession()->GetMap()->GetId();
            // write count of founds
//            std::cout <<"5" <<std::endl;
            oa << player.second.GetLoots().size();
            for ( const auto& loot : player.second.GetLoots() ) {
                oa << LootRepr(loot);
//                std::cout <<"6" <<std::endl;
            }
//            PlayerRepr player_rep(player.second);
//            oa << player_rep;
        }
        // write game sessions info
        for ( const auto& game_session : game_.GetSessions()) {
            const auto map = game_session->GetMap();
            // write mapId
            oa << *map->GetId();
            // write count of loots on game_session
            oa << map->GetLoots().size();
            std::cout << "map: " << *map->GetId() << std::endl;
            auto loots = map->GetLoots();
            std::cout << "count of loots in map: " << map->GetLoots().size() << "; " << loots.size() << std::endl;
//            std::cout <<"02" <<std::endl;
            for (const auto &loot: map->GetLoots()) {
                oa << LootRepr(loot);
//                std::cout <<"01" <<std::endl;
            }
        }
        ofs.close();
    }
    std::filesystem::rename(tmp_file, path_);
    return true;
}

bool Backup::Restore(model::Game& game_, player::Players& players_) {
    using gs = model::GameSession;
    std::ifstream ifs(path_);
    {
        boost::archive::text_iarchive ia{ifs};
//        for ( auto map : game_.GetMaps() ) {
//            // read count of loots on map
//            size_t count_of_loots;
//            ia >> count_of_loots;
//            std::cout << "count_of_loots: " << count_of_loots <<std::endl;
//            for ( int i = 0; i < count_of_loots; i++ ) {
//                LootRepr loot_repr;
//                ia >> loot_repr;
//                map.AddLoot(loot_repr.Restore());
//                std::cout << "count of loots in map: " << map.GetLoots().size() <<std::endl;
//            }
//        }
        // read count of players
        size_t count_of_players;
        ia >> count_of_players;
        for (; count_of_players > 0; count_of_players-- ) {
            std::string token_str;
            ia >> token_str;
            DogRepr dog_r;
            ia >> dog_r;
            // read map id
            std::string mapid;
            ia >> mapid;
            auto map = game_.FindMap(model::Map::Id{mapid.c_str()});
            std::shared_ptr<gs> game_session{nullptr};
            // если уже есть сессия, то найдём её
            if (game_.FindMap(model::Map::Id{mapid.c_str()}) != nullptr) {
                for (auto& session: game_.GetSessions()) {
                    if (*(session->GetMap()->GetId()) == mapid) {
                        game_session = session;
                    }
                }
            }
            if (game_session == nullptr) {
                gs gm_ses{gs::Id{mapid.c_str()}, map,
                          game_.GetLootGeneratorConfig()};
                game_.AddSession(gm_ses);
                game_session = game_.FindGameSession(gs::Id{mapid.c_str()});
            }
            auto dg = game_session->AddDog(dog_r.Restore());
            auto token = Token{token_str};
            players_.AddPlayer(token, dg, game_session);
            auto player = players_.FindByToken(token);
            size_t count_of_founds;
            // read count of founds and founds
            ia >> count_of_founds;
            for (; count_of_founds>0; count_of_founds--) {
                LootRepr loot_repr;
                ia >> loot_repr;
                player->AddLoot(loot_repr.Restore());
            }
//            PlayerRepr player_repr;
//            ia >> player_repr;
//            players_.AddPlayer(Token(token), player_repr.Restore());
        }
        // read game sessions info
        size_t count_of_game_sessions = game_.GetSessions().size();
        for ( ;count_of_game_sessions > 0; count_of_game_sessions--) {
            std::string mapId;
            // read mapId
            ia >> mapId;
            auto game_session = std::find_if(game_.GetSessions().begin(),
                                             game_.GetSessions().end(),
                                             [&mapId](const std::shared_ptr<model::GameSession>& gs) {
                return gs->GetMap()->GetId() == model::Map::Id{mapId};});
            size_t count_of_loots;
            // read count of loots on game_session
            ia >> count_of_loots;
//            auto loots = map->GetLoots();
//            std::cout << "count of loots in map: " << map->GetLoots().size() << "; " << loots.size() << std::endl;
            std::cout <<"count_of_loots " << count_of_loots <<std::endl;
            for (;count_of_loots > 0; count_of_loots--) {
                LootRepr loot_repr;
                ia >> loot_repr;
                (*game_session)->GetMap()->AddLoot(loot_repr.Restore());
//                std::cout <<"01" <<std::endl;
            }
        }

    }
    return true;
}

void Backup::TrySave(const std::chrono::milliseconds ms, const model::Game& game, const player::Players& players) {
    ms_ += ms;
    std::cout << "save? "<< ms_.count() <<std::endl;
    if (ms_ > save_period_) {
        std::cout << "save "<< save_period_.count() <<std::endl;
        ms_ = std::chrono::milliseconds {0};
//        Save(game, players);
        Save(game, players);
    }
}
