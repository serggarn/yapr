#include "loot_generator.h"

#include <algorithm>
#include <cmath>
//#include <iostream>

namespace loot_gen {

unsigned LootGenerator::Generate(TimeInterval time_delta, unsigned loot_count,
                                 unsigned looter_count) {
//    std::cout << "time_without_loot " <<time_without_loot_.count()<<std::endl;
    time_without_loot_ += time_delta;
//    std::cout << "time_without_loot " <<time_without_loot_.count()<<std::endl;
    const unsigned loot_shortage = loot_count > looter_count ? 0u : looter_count - loot_count;
    const double ratio = std::chrono::duration<double>{time_without_loot_} / base_interval_;
    const double probability
        = std::clamp((1.0 - std::pow(1.0 - probability_, ratio)) * random_generator_(), 0.0, 1.0);
    const unsigned generated_loot = static_cast<unsigned>(std::round(loot_shortage * probability));
//    std::cout << "generated_loot " <<generated_loot<<std::endl;

    if (generated_loot > 0) {
        time_without_loot_ = {};
    }
    return generated_loot;
}

} // namespace loot_gen
