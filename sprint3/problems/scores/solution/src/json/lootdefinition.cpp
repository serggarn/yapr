//
// Created by serg on 20.05.23.
//

#include "lootdefinition.h"
#include "json_tags.h"

LootDefinition::Value LootDefinition::GetValue() const noexcept{
    return defenition_.contains(json_tags::value)
        ? defenition_.at(json_tags::value).as_int64()
        : 0;
}