#include "window.hpp"

Window::Window() : sf::RenderWindow(sf::VideoMode
    ({WindowAttributes::WINDOW_WIDTH, WindowAttributes::WINDOW_HEIGHT}), 
    WindowAttributes::WINDOW_TITLE, sf::State::Windowed, 
    WindowAttributes::getSettings())
{
     // Check if OpenGL functions are loaded and the window is successfully created
     if (!gladLoadGL() || !isOpen()) 
     {
         // Throw an exception if initialization fails
         throw std::runtime_error("ERROR::Failed to initialize window context.");
     }

     clock_ = std::make_unique<sf::Clock>();
}