#include "Core/Engine.hpp"
#include <iostream>

int main()
{
    Engine* engine = nullptr;

    try
    {
        engine = new Engine();
        engine->loop();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        delete engine;
        return EXIT_FAILURE;
    }
    delete engine;
    return EXIT_SUCCESS;
}