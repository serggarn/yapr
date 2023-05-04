#pragma once
#include <iostream>
#include "tagged.h"

namespace player {
	
class Dog {
public:
	using Id = util::Tagged<std::string, Dog>;
	Dog(Id _id, std::string _name) noexcept 
		: id(std::move(_id))
		, name (std::move(_name)) {};

    const Id& GetId() const noexcept {
        return id;
    }
    const std::string& GetName() const noexcept {
        return name;
    }
private:
	Id id;
	std::string name;
};

} // namespace player
