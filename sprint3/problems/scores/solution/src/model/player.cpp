#include "player.h"
uint64_t Player::next_id{0};

void Player::AddLoot(model::Loot& loot) noexcept {
    loots_.emplace_back(std::move(loot));
}

void Player::GiveLoots() {
    for ( const auto& loot : loots_)
        score += loot.GetValue();
    loots_.erase(loots_.begin(), loots_.end());
}
