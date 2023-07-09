#include <Simulation.h>
#include <Visualization.h>
#include <SFML/Graphics.hpp>
#include <entt/entt.hpp>
#include <iostream>
#include <chrono>

int main()
{
    const size_t STEP_SIMULATION = 50; // 50ms
    const size_t NUMBER_ARCHERS = 20;

    Visualization visualization;
    Simulation simulation(&visualization, NUMBER_ARCHERS, STEP_SIMULATION);
    visualization.exec();
 
    const size_t NANO_PER_MSEC = 1000000;
    auto simulation_loop_time = std::chrono::high_resolution_clock::now();
    while (visualization.window()->isOpen())
    {
        sf::Event event;
        while (visualization.window()->pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                visualization.window()->close();
            if (event.type == sf::Event::Resized)
            {
                visualization.resize(event.size.width, event.size.height);
            }
        }

        if ((std::chrono::high_resolution_clock::now() - simulation_loop_time).count() / NANO_PER_MSEC > STEP_SIMULATION)
        {
            simulation_loop_time = std::chrono::high_resolution_clock::now();
            simulation.update();
        }
    }

    return 0;
}
