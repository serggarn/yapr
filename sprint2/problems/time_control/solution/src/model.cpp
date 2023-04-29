#include "model.h"

#include <stdexcept>

namespace model {
using namespace std::literals;

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
    roads_.emplace_back(std::make_shared<Road>(road));
    auto rd = roads_.back();
    if ( rd->IsHorizontal() ) {
        if ( ! hor_roads_.contains(rd->GetStart().y) )
            hor_roads_.emplace(rd->GetStart().y, MapRoad1D{});
        auto hr = hor_roads_.at(rd->GetStart().y);
        hr.emplace(std::min(rd->GetStart().x, rd->GetEnd().x), rd);
    }
    else {
        if ( ! vert_roads_.contains(rd->GetStart().x) )
            vert_roads_.emplace(rd->GetStart().x, MapRoad1D{});
        auto hr = vert_roads_.at(rd->GetStart().x);
        hr.emplace(std::min(rd->GetStart().y, rd->GetEnd().y), rd);
    }
}

/**
 * @brief GetRoadsByCoord - возвращает дороги (1 или 2), на которых находится пёс
 * @param pos
 * @return <вертиальная дорога, горизонтальная дорога> могут быть nullptr
 */
const std::pair <std::shared_ptr<Road>, std::shared_ptr<Road>> Map::GetRoadsByCoord(const Point& pos) const {
    auto res = std::make_pair<std::shared_ptr<Road>, std::shared_ptr<Road>> (nullptr, nullptr);
    if ( vert_roads_.contains(pos.x)) {
        auto rds = vert_roads_.at(pos.x);
        for (const auto& rd : rds ) {
            auto x_left = rd.first;
            auto x_right = std::max(rd.second->GetStart().y, rd.second->GetEnd().y);
            if (pos.x < x_left)
                break;
            if ( pos.x >= x_left && pos.x <= x_right)
                res.first = rd.second;
        }
    }
    if ( hor_roads_.contains(pos.y)) {
        auto rds = hor_roads_.at(pos.y);
        for (const auto& rd : rds ) {
            auto y_up = rd.first;
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
    return std::min(start_.x, end_.x) <= point.x && point.x <= std::max(start_.x, end_.x)
            && std::min(start_.y, end_.y) <= point.y && point.y <= std::max(start_.y, end_.y);
}

}  // namespace model
