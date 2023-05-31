#pragma once
#include <random>
#include "tagged.h"
#include <sstream>
#include <iomanip>

namespace detail {
struct TokenTag {
};
	
}  // namespace detail

using Token = util::Tagged<std::string, detail::TokenTag>;
static const size_t token_length = 32;

class PlayerTokens {
public:
    Token generate_token() {
		auto g1 = generator1_();
		auto g2 = generator2_();
		std::stringstream ss;
		ss << std::hex << std::setfill('0') << std::setw(token_length / 2) << g1;
		ss << std::hex << std::setfill('0') << std::setw(token_length / 2) << g2;
//        std::cout << "token: " << ss.str() << std::endl;
		return Token{ss.str()};
	}
private:
    std::random_device random_device_;
    std::mt19937_64 generator1_{[this] {
        std::uniform_int_distribution<std::mt19937_64::result_type> dist;
        return dist(random_device_);
    }()};
    std::mt19937_64 generator2_{[this] {
        std::uniform_int_distribution<std::mt19937_64::result_type> dist;
        return dist(random_device_);
    }()};
    // Чтобы сгенерировать токен, получите из generator1_ и generator2_
    // два 64-разрядных числа и, переведя их в hex-строки, склейте в одну.
    // Вы можете поэкспериментировать с алгоритмом генерирования токенов,
    // чтобы сделать их подбор ещё более затруднительным
}; 
