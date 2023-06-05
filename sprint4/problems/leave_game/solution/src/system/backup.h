//
// Created by serg on 29.05.23.
//
#pragma once
#include <iostream>
#include "../game_play/game.h"

class Backup {
protected:
    Backup() {};
    static std::shared_ptr<Backup> backup_;
public:
    Backup(Backup &) = delete;

    Backup &operator=(const Backup &) = delete;

    static std::shared_ptr<Backup> GetInstance();
    void Init(std::string path, std::chrono::milliseconds period);

public:
    Backup(std::string path, std::chrono::milliseconds period)
    : path_(std::move(path))
    , save_period_(period) {}
    bool Save(const model::Game& game, const player::Players& players);
    void TrySave(const std::chrono::milliseconds ms, const model::Game& game, const player::Players& players);
    bool Restore(model::Game& game, player::Players& players);
private:
    std::string path_;
    std::chrono::milliseconds ms_ {0};
    std::chrono::milliseconds save_period_;
};
