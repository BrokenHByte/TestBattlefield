#include <memory>
#include <vector>

#include "Geometry.h"
#include <SFML/Graphics.hpp>
#include <unordered_map>

class Visualization
{
public:
    void exec();
    void beginDraw();
    void endDraw();
    void drawField();
    void drawArcher(const Vector3 &position, unsigned index);
    void drawArrow(const Vector3& position, const Vector3& direction);

    sf::RenderWindow* window();
    void resize(unsigned width, unsigned height);
private:
    void initField();
    void initPools();
    sf::Vector2f coordIsometry(const Vector3& worldPoint) const;
    std::unique_ptr<sf::RenderWindow> _window;
    std::shared_ptr<sf::RectangleShape> _archerTemplate;
    std::shared_ptr<sf::ConvexShape> _fieldTemplate;

    const size_t SIZE_ARCHER_POOL = 100;
    const sf::Color TEAM1_COLOR = sf::Color(255, 0, 0);
    const sf::Color TEAM2_COLOR = sf::Color(0, 0, 255);
};