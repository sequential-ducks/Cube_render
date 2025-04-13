#include "renderer.hpp"
#include <iostream> 

int main()
{
    // Declare unique pointers for the SFML window and OpenGL state
    std::unique_ptr<Window> window;
    std::unique_ptr<Renderer::GL_State> gl;
    try
    {
        // Create the SFML window, throws runtime error if fails
        window = std::make_unique<Window>();

        // Initialize the OpenGL state 
        gl = std::make_unique<Renderer::GL_State>();
    }
    catch(const std::runtime_error& except)
    {
        // Handle any errors during initialization and exit the program
        std::cerr << except.what();
        exit(EXIT_FAILURE);
    }

    // Main application loop
    while (window->isOpen())
    {
        while (const std::optional event = window->pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
	            window->close();
            }
            else if (const auto* key_pressed = event->getIf<sf::Event::KeyPressed>())
            {
                if (key_pressed->scancode == sf::Keyboard::Scancode::Escape)
                {
	                window->close();
                }
            }
        }
        // Clear the color and depth buffers for the next frame
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render the scene using the OpenGL state
        gl->draw(window);

        // Display the rendered frame (swap front and back buffers)
        window->display();
    }

    // ShaderProgram executed successfully
    return 0;
}