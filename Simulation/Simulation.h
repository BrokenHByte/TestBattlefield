#pragma once
#include <entt/entt.hpp>
#include <mutex>
#include <optional>
#include "Geometry.h"

class Visualization;

class Simulation
{
public:
    Simulation(Visualization* visualization, unsigned numberArcherTeam, unsigned fixedDeltaTime);
    void update();
private:
    void targetSettings(); // "Система" целеполагания
    void actionEnemyTarget(); // "Система" действия сущностей с целью
    void movingHeavyProjectiles(); // "Система" движения снарядов с гравитацией
    void collisionProjectiles(); // "Система" столкновение снарядов 
    void collisionBetweenCollider(); // "Система" отталкивания коллайдеров
    void toDraw(); // "Система" выдачи на рендер

    void createArcher(unsigned team);
    void attackArcher(const entt::entity &entity);
    void moveArcher(const entt::entity& entity);

    unsigned _remainingArchers; // оставшийся спаун для каждой группы лучников
    float _fixedDeltaTime; // фиксированный шаг симуляции (ms)
    const float DISTANCE_ATTACK = 40.0f;
    const float SPEED_ARROW = 40.0f;
    const float GRAVITY = 9.8f;
    entt::registry _registry;
    Visualization* _visualization;
};

