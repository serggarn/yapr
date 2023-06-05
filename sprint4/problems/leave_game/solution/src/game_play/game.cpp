#include "game.h"
#include <cmath>
#include "../collision/collision_detector.h"
#include "../system/settings.h"

using namespace std::literals;

namespace model {

double Game::default_dog_speed =  1.0;
size_t Game::default_bag_capacity =  3;
const double Game::loot_width = 0.0;
const double Game::player_width = 0.6;
const double Game::office_width = 0.5;
const double Game::default_dog_retirement_time = 60.0;

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

void Game::Tick(const std::chrono::milliseconds delta, player::Players& players, postgres::Database& db) {
    game_time_ += delta;
    std::cout << "tick" <<std::endl;
    double delta_real = delta.count() * ms_in_sec;
    using namespace collision_detector;
    auto gatherers = std::vector<Gatherer>();
    for (auto& gs : sessions_ ) {
        auto map = gs->GetMap();
        for (const auto &dog: gs->GetDogs()) {
            auto dog_in_players = players.FindByDogId(*dog->GetId())->GetDog();
            auto pos = dog_in_players->GetPos();
            auto speed = dog_in_players->GetSpeed();
            Vec2D new_speed = speed;
            std::cout << "delta " << delta_real << "; " << speed.x<< "; " << speed.y<<"; " << (speed.x * delta_real) << "; " << (speed.y * delta_real)  <<std::endl;
            auto new_pos = Point2D{pos.x + (speed.x * delta_real), pos.y + (speed.y * delta_real)};
            std::cout << "new pos calc " << new_pos.x << "; " << new_pos.y <<std::endl;
            // check new pos
            auto roads = map->GetRoadsByCoord(
                    Point{static_cast<int>(std::round(pos.x)), static_cast<int>(std::round(pos.y))});

            if (roads.first == nullptr && roads.second == nullptr)
                continue;
            if (speed.x != 0) {
                CoordReal minX, maxX;
                if (roads.second != nullptr) {
                    minX = std::min(roads.second->GetStart().x, roads.second->GetEnd().x) - Road::HALF_WIDTH;
                    maxX = std::max(roads.second->GetStart().x, roads.second->GetEnd().x) + Road::HALF_WIDTH;
                } else {
                    minX = roads.first->GetStart().x - Road::HALF_WIDTH;
                    maxX = roads.first->GetStart().x + Road::HALF_WIDTH;
                }
                if (new_pos.x <= minX || new_pos.x >= maxX)
                    new_speed.x = 0;
                new_pos.x = new_pos.x < minX ? minX : new_pos.x;
                new_pos.x = new_pos.x > maxX ? maxX : new_pos.x;
            } else if (speed.y != 0) {
                CoordReal minY, maxY;
                if (roads.first != nullptr) {
                    minY = std::min(roads.first->GetStart().y, roads.first->GetEnd().y) - Road::HALF_WIDTH;
                    maxY = std::max(roads.first->GetStart().y, roads.first->GetEnd().y) + Road::HALF_WIDTH;
                } else {
                    minY = roads.second->GetStart().y - Road::HALF_WIDTH;
                    maxY = roads.second->GetStart().y + Road::HALF_WIDTH;
                }
                if (new_pos.y <= minY || new_pos.y >= maxY)
                    new_speed.y = 0;
                new_pos.y = new_pos.y < minY ? minY : new_pos.y;
                new_pos.y = new_pos.y > maxY ? maxY : new_pos.y;
            }

            auto start_pos = geom::Point2D{pos.x, pos.y};
            auto end_pos = geom::Point2D{new_pos.x, new_pos.y};
            gatherers.emplace_back(Gatherer{start_pos, end_pos, player_width});
            std::cout << "new pos " << new_pos.x << "; " << new_pos.y <<std::endl;
            dog_in_players->SetPos(new_pos);
            if (speed != new_speed) {
                dog_in_players->SetSpeed(new_speed);
            }
        }

        auto items = std::vector<Item>();
        // add loot to items
        auto loots = map->GetLoots();
        for (const auto& loot : loots ) {
            auto loot_pos = loot.GetPosition();
            auto pos = geom::Point2D(loot_pos.x, loot_pos.y);
            items.emplace_back(Item{pos, loot_width});
        }
        // add offices to items
        auto offices = map->GetOffices();
        for (const auto& office : offices) {
            auto office_pos = office.GetPosition();
            auto pos = geom::Point2D(office_pos.x, office_pos.y);
            items.emplace_back(Item{pos, office_width});
        }
        // collect gatherer events
        ItemsGatherInterface itemGathererIface{items, gatherers};
        auto events = FindGatherEvents(itemGathererIface);

        for (auto const& event : events) {
            auto player = players.GetPlayer(Player::Id{event.gatherer_id});
            if ( player == nullptr )
                continue;
            auto loot = map->GetLoot(event.item_id);
            if ( event.item_id >= loots.size() )
                player->GiveLoots();
            else if ( event.item_id < loots.size() && player->GetLoots().size() < map->GetBagCapacity() ) {
                player->AddLoot(*loot);
                map->RemoveLoot(Loot::Id(event.gatherer_id));
            }
            //            auto dog = gs->FindDog(model::Dog::Id{event.gatherer_id})
        }

        // generateloot
        auto loot_generator = gs->GetLootGenerator();
//        std::cout << "Sizes: " << delta.count() << "; " << map->GetLoots().size() << "; " << gs->GetDogs().size() <<std::endl;
        auto count = loot_generator->Generate(delta, map->GetLoots().size(), gs->GetDogs().size());
//        std::cout << count << "; " << loot_generator->GetTime() <<std::endl;
        for (; count > 0; count--) {
            gs->AddLoot();
        }
    }

    // check retired dogs
    auto sett = settings::Settings::GetInstance();
    auto curr_time = game_time_ - retired_time_;
    auto transaction = db.CreateTransaction();
    auto dogs_to_retire = db.GetPlayers(transaction).Get(curr_time.count());
    std::cout << "count of retired dogs: " << dogs_to_retire.size() <<std::endl;
    for ( const auto& dog_bd : dogs_to_retire ) {
//        std::cout << "1" <<std::endl;
        std::cout << "2" <<std::endl;
        db.GetPlayers(transaction).Delete(domain::PlayerId{dog_bd.token});
        std::shared_ptr<Dog> dog;
        std::cout << "3 " << dog_bd.token << std::endl;
        auto player = players.FindByToken(Token(dog_bd.token));
        if (player == nullptr)
            continue;
        std::cout << "4 " << (player == nullptr) << std::endl;
        auto dog_id = player->GetDog()->GetId();
        std::cout << "5" <<std::endl;
        auto score = player->GetScore();
        std::cout << "6" <<std::endl;
        players.DeletePlayer(Token(dog_bd.token));
        std::cout << "7" <<std::endl;
        double diff = (game_time_.count() - dog_bd.playTime) / 1000;
        std::cout << "times: " << game_time_.count()<< "; " << dog_bd.playTime << "; " << diff <<std::endl;

        db.GetRecords(transaction).Save({domain::RecordId::New(), dog_bd.name, score, diff});


        for (const auto& gs : sessions_) {
            auto dog_on_map = gs->FindDog(dog_id);
            if ( dog_on_map != nullptr ) {

            }
        }

    }
    transaction.commit();
//        std::cout << "finc Tick end" << std::endl;
}

} //namespace model
