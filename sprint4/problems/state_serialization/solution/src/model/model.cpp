#include "model.h"

#include <stdexcept>
#include <iostream>

namespace model {
using namespace std::literals;

uint64_t Loot::next_id{0};

void Map::AddOffice(const Office& office) {
    if (warehouse_id_to_index_.contains(office.GetId())) {
        throw std::invalid_argument("Duplicate warehouse");
    }

    const size_t index = offices_.size();
    Office& o = offices_.emplace_back(std::move(office));
    try {
        warehouse_id_to_index_.emplace(o.GetId(), index);
    } catch (...) {
        // Удаляем офис из вектора, если не удалось вставить в unordered_map
        offices_.pop_back();
        throw;
    }
}

void Map::AddRoad(const Road& road) {
    roads_.emplace_back(std::make_shared<Road>(std::move(road)));
    auto rd = roads_.back();
    if ( rd->IsHorizontal() ) {
        if ( ! hor_roads_.contains(rd->GetStart().y) )
            hor_roads_.emplace(rd->GetStart().y, MapRoad1D{});
        auto hr = hor_roads_.find(rd->GetStart().y);
        auto res = hr->second.emplace(std::min(rd->GetStart().x, rd->GetEnd().x), rd);
    }
    else {
        if ( ! vert_roads_.contains(rd->GetStart().x) )
            vert_roads_.emplace(rd->GetStart().x, MapRoad1D{});
        auto hr = vert_roads_.find(rd->GetStart().x);
        hr->second.emplace(std::min(rd->GetStart().y, rd->GetEnd().y), rd);
    }
//    if ( rd->IsHorizontal())
//        std::cout << "AddRoad after edding: " << hor_roads_.at(rd->GetStart().y).size() <<std::endl;
//    else
//        std::cout << "AddRoad after edding: " << vert_roads_.at(rd->GetStart().x).size() <<std::endl;

//    PrintVHRoads();
}

/**
 * @brief GetRoadsByCoord - возвращает дороги (1 или 2), на которых находится пёс
 * @param pos
 * @return <вертиальная дорога, горизонтальная дорога> могут быть nullptr
 */
const std::pair <std::shared_ptr<Road>, std::shared_ptr<Road>> Map::GetRoadsByCoord(const Point& pos) const {
    auto res = std::make_pair<std::shared_ptr<Road>, std::shared_ptr<Road>> (nullptr, nullptr);
//    std::cout << "GetRoadsByCoord: " << pos.x <<" : " << pos.y <<std::endl;
//    std::cout << "Sizes: " << vert_roads_.size() <<" : " << hor_roads_.size() <<std::endl;

    if ( vert_roads_.contains(pos.x)) {
        auto rds = vert_roads_.at(pos.x);
//        std::cout << "vert contains " << rds.size() <<std::endl;
        for (const auto& rd : rds ) {
            auto x_left = rd.first;
            auto x_right = std::max(rd.second->GetStart().y, rd.second->GetEnd().y);
            if (pos.x < x_left)
                break;
            if ( rd.second->CheckPoint(pos))
                res.first = rd.second;
        }
    }
    if ( hor_roads_.contains(pos.y)) {
        auto rds = hor_roads_.at(pos.y);
//        std::cout << "hor contains " << rds.size() << std::endl;
        for (const auto& rd : rds ) {
            auto y_up = rd.first;
//            std::cout << "y_up: " << y_up << "; pos.y: " <<pos.y <<std::endl;
            auto y_down = std::max(rd.second->GetStart().x, rd.second->GetEnd().x);
            if (pos.y < y_up)
                break;
            if ( rd.second->CheckPoint(pos))
                res.second = rd.second;
        }
    }

    return res;
}

bool Road::CheckPoint(const Point& point) const noexcept {
//    std::cout << "CheckPoint: " <<std::min(start_.x, end_.x)  << "<=" << point.x << " && " << point.x << "<=" << std::max(start_.x, end_.x)
//              << " && " << std::min(start_.y, end_.y) << "<=" << point.y << " && " << point.y << "<=" << std::max(start_.y, end_.y) <<std::endl;
    return std::min(start_.x, end_.x) <= point.x && point.x <= std::max(start_.x, end_.x)
           && std::min(start_.y, end_.y) <= point.y && point.y <= std::max(start_.y, end_.y);
}

bool Road::CheckPoint(const Point2D& point) const noexcept {
//    std::cout << "CheckPoint: " <<std::min(start_.x, end_.x)  << "<=" << point.x << " && " << point.x << "<=" << std::max(start_.x, end_.x)
//              << " && " << std::min(start_.y, end_.y) << "<=" << point.y << " && " << point.y << "<=" << std::max(start_.y, end_.y) <<std::endl;
    if ( IsHorizontal() ) {
        return std::min(start_.x, end_.x) <= point.x && point.x <= std::max(start_.x, end_.x)
               && std::min(start_.y, end_.y) - HALF_WIDTH <= point.y && point.y <= std::max(start_.y, end_.y) + HALF_WIDTH;
    }
    else {
        return std::min(start_.x, end_.x) - HALF_WIDTH <= point.x && point.x <= std::max(start_.x, end_.x) + HALF_WIDTH
               && std::min(start_.y, end_.y) <= point.y && point.y <= std::max(start_.y, end_.y);
    }
}

void Map::PrintVHRoads() const {
    std::cout << "MAP: " << name_ << std::endl
            <<"vert_roads: " << vert_roads_.size() <<std::endl;
    for (const auto& rds : vert_roads_) {
        std::cout << rds.second.size() <<std::endl;
        for (const auto &rd: rds.second)
            std::cout << rds.first << "; " << rd.first << "; " << std::endl;
    }
    std::cout << "hor_roads: " << hor_roads_.size() <<std::endl;
    for (const auto& rds : vert_roads_) {
        std::cout << rds.second.size() <<std::endl;
        for (const auto &rd: rds.second)
            std::cout << rd.first << "; " << rds.first << "; " << std::endl;
    }
}

void Map::AddLootDefinition(const LootDefinition &definition) {
    loots_definitions_.emplace_back(std::move(definition));
}

void Map::AddLoot(const model::Loot &loot) {
    auto res = loots_.emplace_back(std::move(loot));
}

void Map::RemoveLoot(const Loot::Id id) {
    loots_.erase(std::find_if(loots_.begin(), loots_.end(),
                              [&id](const Loot& lt) { return lt.GetId() == id; }));
}

std::shared_ptr<Loot> Map::GetLoot(const size_t index) const noexcept {
    return loots_.size() > index ? std::make_shared<Loot>(loots_.at(index)) : nullptr;
}

}  // namespace model
