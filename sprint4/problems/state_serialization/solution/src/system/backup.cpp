//
// Created by serg on 29.05.23.
//

#include "backup.h"
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include "../model/model_serialization.h"
#include "../token.h"
#include <fstream>

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

    std::ofstream ofs(path_);
    {
        boost::archive::text_oarchive oa{ofs};
        for ( const auto& map : game_.GetMaps() ) {
//            std::cout <<"03" <<std::endl;

            // write count of loots on map
            oa << map.GetLoots().size();
//            std::cout <<"02" <<std::endl;
            for ( const auto& loot : map.GetLoots() ) {
                oa << LootRepr(loot);
//                std::cout <<"01" <<std::endl;
            }
        }
//        std::cout <<"1" <<std::endl;
        auto game_sessions = game_.GetSessions();
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
    }
    return true;
}

bool Backup::Restore(model::Game& game_, player::Players& players_) {
    using gs = model::GameSession;
    std::ifstream ifs(path_);
    {
        boost::archive::text_iarchive ia{ifs};
        for ( auto map : game_.GetMaps() ) {
            // read count of loots on map
            size_t count_of_loots;
            ia >> count_of_loots;
            for ( int i = 0; i < count_of_loots; i++ ) {
                LootRepr loot_repr;
                ia >> loot_repr;
                map.AddLoot(loot_repr.Restore());
            }
        }
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
    }
    return true;
}

void Backup::TrySave(const std::chrono::milliseconds ms, const model::Game& game, const player::Players& players) {
    ms_ += ms;
    if (ms_ > save_period_) {
//        std::cout << "save " <<std::endl;
        ms_ = std::chrono::milliseconds {0};
        Save(game, players);
    }
}
