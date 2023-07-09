#include "Visualization.h"

const float PI = 3.1415926f;
const float cos30 = std::cos(PI / 6.0f);
const float sin30 = std::sin(PI / 6.0f);

void Visualization::exec()
{
    _window = std::make_unique<sf::RenderWindow>(sf::VideoMode(800, 600), "Battlefield");
    resize(800, 600);

    initPools();
    initField();
}

void Visualization::beginDraw()
{
    _window->clear();
}

void Visualization::endDraw()
{
    _window->display();
}

void Visualization::drawField()
{
    _window->draw(*_fieldTemplate);
}

sf::RenderWindow* Visualization::window()
{
    return _window.get();
}

void Visualization::drawArcher(const Vector3& position, unsigned index)
{
    _archerTemplate->setFillColor(index == 1 ? TEAM1_COLOR : TEAM2_COLOR);
    _archerTemplate->setPosition(coordIsometry(position));
    _window->draw(*_archerTemplate);
}

void Visualization::drawArrow(const Vector3& position, const Vector3& direction)
{
    const auto forward = direction.normalize();
    const auto coord1 = coordIsometry(position - forward * 0.25f);
    const auto coord2 = coordIsometry(position + forward * 0.25f);

    const sf::Vertex line[] =
    {
        sf::Vertex(coord1, sf::Color(0, 50, 50)),
        sf::Vertex(coord2, sf::Color(0, 50, 50))
    };
    _window->draw(line, 2, sf::Lines);
}

void Visualization::resize(unsigned width, unsigned height)
{
    auto center = coordIsometry(Vector3(50.0f, 0.0f, 50.0f));
    sf::View view(sf::FloatRect(0.0f, 0.0f, 200.f, 200.f * height / width));
    view.setCenter(center);
    _window->setView(view);
}

void Visualization::initField()
{
    _fieldTemplate = std::make_shared<sf::ConvexShape>();
    _fieldTemplate->setPointCount(4);
    _fieldTemplate->setPoint(0, coordIsometry({ 0.0f, 0.0f, 0.0f }));
    _fieldTemplate->setPoint(1, coordIsometry({ 100.0f, 0.0f , 0.0f }));
    _fieldTemplate->setPoint(2, coordIsometry({ 100.0f, 0.0f, 100.0f }));
    _fieldTemplate->setPoint(3, coordIsometry({ 0.0f, 0.0f, 100.0f }));
    _fieldTemplate->setOutlineThickness(0.6f);
    _fieldTemplate->setOutlineColor(sf::Color(150, 150, 150));
}

void Visualization::initPools()
{
    _archerTemplate = std::make_shared<sf::RectangleShape>(sf::Vector2f(0.2f, 1.7f));
    _archerTemplate->setOrigin(sf::Vector2f(0.1f, 1.7f));
}

sf::Vector2f Visualization::coordIsometry(const Vector3& worldPoint) const
{
    return sf::Vector2f(cos30 * (worldPoint.z - worldPoint.x), sin30 * (worldPoint.x + worldPoint.z) - worldPoint.y);
}
