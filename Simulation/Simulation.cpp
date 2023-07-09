#include "Simulation.h"
#include "Visualization.h"
#include "Components.h"

#include <thread>
#include <iostream>

Simulation::Simulation(Visualization* visualization, unsigned numberArcherTeam, unsigned fixedDeltaTime) : _visualization(visualization),
_remainingArchers(numberArcherTeam), _fixedDeltaTime(fixedDeltaTime / 1000.0f)
{
    auto spawnPoint1 = _registry.create();
    _registry.emplace<Spawn>(spawnPoint1, Vector3(10.0f, 0.0f, 10.0f));
    _registry.emplace<TeamAffiliation>(spawnPoint1, 1u);
    auto spawnPoint2 = _registry.create();
    _registry.emplace<Spawn>(spawnPoint2, Vector3(90.0f, 0.0f, 90.0f));
    _registry.emplace<TeamAffiliation>(spawnPoint2, 2u);
}

void Simulation::update()
{
    // Маркером spawn_mark, делаем появление лучников через шаг Update, каждые 100мс
    // (упрощение)
    static bool spawn_mark = false;
    spawn_mark = !spawn_mark;

    // Создание лучников
    if (spawn_mark && _remainingArchers > 0)
    {
        createArcher(1);
        createArcher(2);
        _remainingArchers--;
    }

    // Прогон "систем"
    targetSettings();
    actionEnemyTarget();
    movingHeavyProjectiles();
    collisionProjectiles();
    collisionBetweenCollider();
    toDraw();
}


void Simulation::createArcher(unsigned team)
{
    Vector3 pos;
    for (auto&& [entity, anchor, teamAffiliation] : _registry.view<Spawn, TeamAffiliation>().each()) {
        if (teamAffiliation.index == team)
        {
            pos = anchor.pos;
            break;
        }
    }

    auto archer = _registry.create();
    _registry.emplace<Position>(archer, pos);
    _registry.emplace<TeamAffiliation>(archer, team);
    _registry.emplace<Сooldown>(archer, 1.0f, 0.0f);
    _registry.emplace<СolliderCylinder>(archer, Vector3(0.0f, 0.85f, 0.0f), 0.25f, 1.7f);
    _registry.emplace<Target>(archer);
    _registry.emplace<Health>(archer, 100.0f);
}

void Simulation::attackArcher(const entt::entity& entity)
{
    //5. поведение стрелы
    //  a.стрела летит по законам гравитации
    //  b.скорость стрелы 40 метров в секунду(упрощение - не меняется)

    auto& cooldown = _registry.get<Сooldown>(entity);
    cooldown.timeLeft -= _fixedDeltaTime;
    if (cooldown.timeLeft < 0.0f)
    {
        cooldown.timeLeft = cooldown.timeСooldown;
        auto& targetEntity = _registry.get<Target>(entity);
        if (!targetEntity.target.has_value())
            return;
        auto& position = _registry.get<Position>(entity).pos;
        auto& positionTarget = _registry.get<Position>(targetEntity.target.value()).pos;
        auto direction = (positionTarget - position).normalize();
        // Подстраиваем вычисления чтобы стрела преодолевала в горизонтальной плоскости 40м/с
        float distance = std::sqrt(targetEntity.distance2);
        float timeFly = distance / SPEED_ARROW;
        float speedHorizontal = SPEED_ARROW;
        float speedVertical = GRAVITY / 2.0f * timeFly;

        Vector3 speed = { direction.x * speedHorizontal, speedVertical,  direction.z * speedHorizontal };

        auto arrow = _registry.create();
        _registry.emplace<Position>(arrow, position + Vector3(0.0f, 0.85f, 0.0f));
        _registry.emplace<Speed>(arrow, speed);
        _registry.emplace<Damage>(arrow, 20.01f);
    }
}

void Simulation::moveArcher(const entt::entity& entity)
{
    auto& targetEntity = _registry.get<Target>(entity);
    if (!targetEntity.target.has_value())
        return;
    auto& position = _registry.get<Position>(entity).pos;
    auto& positionTarget = _registry.get<Position>(targetEntity.target.value()).pos;
    position += (positionTarget - position).normalize() * 0.1f;
}

void Simulation::targetSettings()
{
    std::vector<std::pair<float, entt::entity>> closestTargets;
    closestTargets.reserve(20);
    auto group = _registry.group<Position, TeamAffiliation, Target>();
    for (auto&& [entity, positionEntity, teamEntity, targetEntity] : group.each()) {
        closestTargets.clear();
        for (auto&& [entity2, positionEntity2, teamEntity2, targetEntity2] : group.each()) {
            if (teamEntity.index != teamEntity2.index)
            {
                closestTargets.push_back({ (positionEntity2.pos - positionEntity.pos).length2(), entity2 });
            }
        }

        std::sort(closestTargets.begin(), closestTargets.end(), [](const auto& v1, const auto& v2)
            { return v1.first < v2.first; });
        if (closestTargets.empty())
            continue;

        targetEntity.distance2 = closestTargets.begin()->first;
        targetEntity.target = closestTargets.begin()->second;
    }
}

void Simulation::actionEnemyTarget()
{
    float distance2 = DISTANCE_ATTACK * DISTANCE_ATTACK;
    for (auto&& [entity, targetEntity] : _registry.view<Target>().each()) {

        if (!targetEntity.target.has_value())
            continue;

        if (!_registry.valid(targetEntity.target.value()))
        {
            targetEntity.target = {};
            continue;
        }

        if (targetEntity.distance2 < distance2)
        {
            attackArcher(entity);
        }
        else
        {
            moveArcher(entity);
        }
    }
}

void Simulation::movingHeavyProjectiles()
{
    // Движение снарядов под гравитацией
    for (auto&& [entity, positionEntity, speedEntity] : _registry.view<Position, Speed>().each()) {
        speedEntity.speed += Vector3(0.0f, -GRAVITY * _fixedDeltaTime, 0.0f);
        positionEntity.pos += speedEntity.speed * _fixedDeltaTime;

        if (positionEntity.pos.y < 0.0f && speedEntity.speed.y < 0.0f)
        {
            _registry.destroy(entity);
            continue;
        }
    }
}

void Simulation::collisionProjectiles()
{
    for (auto&& [entityProjectiles, positionProjectiles, speedProjectiles, damageProjectiles] : _registry.view<Position, Speed, Damage>().each()) {
        for (auto&& [entity, positionEntity, colliderEntity, healthEntity] : _registry.view<Position, СolliderCylinder, Health>().each()) {

            float distance = (Vector3(positionProjectiles.pos.x, 0.0f, positionProjectiles.pos.z) -
                Vector3(positionEntity.pos.x, 0.0f, positionEntity.pos.z)).length2();

            // Если точка снаряда входит в цилиндр
            if (positionProjectiles.pos.y > (positionEntity.pos.y + colliderEntity.origin.y - colliderEntity.height / 2) &&
                positionProjectiles.pos.y < (positionEntity.pos.y + colliderEntity.origin.y + colliderEntity.height / 2) && distance < 0.25f)
            {
                _registry.get<Health>(entity).health -= damageProjectiles.damage;
                _registry.destroy(entityProjectiles);
                if (_registry.get<Health>(entity).health < 0.0f)
                {
                    _registry.destroy(entity);
                }
                break;
            }
        }
    }
}

void Simulation::collisionBetweenCollider()
{
    for (auto&& [entity1, positionEntity1, colliderEntity1] : _registry.view<Position, СolliderCylinder>().each()) {
        for (auto&& [entity2, positionEntity2, colliderEntity2] : _registry.view<Position, СolliderCylinder>().each()) {
            if (entity1 == entity2)
                continue;
            float distance = (positionEntity2.pos - positionEntity1.pos).length2();
            if (distance < 0.25f)
            {
                if (distance < std::numeric_limits<float>().epsilon())
                {
                    Vector3 move = Vector3(static_cast<float>(rand() % 100 - 50), 0.0f, static_cast<float>(rand() % 100 - 50)).normalize();
                    positionEntity1.pos += move * _fixedDeltaTime;
                    positionEntity2.pos -= move * _fixedDeltaTime;
                    continue;
                }

                auto normalDirection = (positionEntity2.pos - positionEntity1.pos).normalize();
                positionEntity1.pos -= normalDirection * _fixedDeltaTime;
                positionEntity2.pos += normalDirection * _fixedDeltaTime;
            }
        }
    }
}

void Simulation::toDraw()
{
    _visualization->beginDraw();
    _visualization->drawField();
    // Лучники
    for (auto&& [entity, positionEntity, teamEntity] : _registry.view<Position, TeamAffiliation>().each()) {
        _visualization->drawArcher(positionEntity.pos, teamEntity.index);
    }

    // Стрелы
    for (auto&& [entity, positionEntity, speedEntity] : _registry.view<Position, Speed>().each()) {
        _visualization->drawArrow(positionEntity.pos, speedEntity.speed);
    }

    _visualization->endDraw();
}
