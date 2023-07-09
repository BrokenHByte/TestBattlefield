
// Точка спауна
struct Spawn
{
    Vector3 pos;
};

struct Position
{
    Vector3 pos;
};

struct Target
{
    std::optional<entt::entity> target{};
    float distance2; // квадрат расстояния до цели
};

struct Damage
{
    float damage;
};

struct Speed
{
    Vector3 speed;
};

// Откат атаки
struct Сooldown {
    float timeСooldown;
    float timeLeft;
};

struct СolliderCylinder {
    СolliderCylinder(const Vector3 &origin, float radius, float height) : origin(origin), radius(radius), height(height) {}
    Vector3 origin;
    float radius;
    float height;
};

struct Health {
    float health;
};

struct TeamAffiliation
{
    // Принадлежность к команде. 0 - нейтрал
    unsigned index;
};