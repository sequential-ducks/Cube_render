#include "renderer.hpp"
// For SFML windows
#include <optional>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <iostream> // for std::cerr 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


void math()
{
    glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);
    glm::mat4 translateM{1.0f};
    translateM = glm::translate(translateM, glm::vec3(vec.x, vec.y, vec.x));
    vec = translateM * vec;    
    std::cout << " vec x: " << vec.x << " vec y: " << vec.y << " vec z: " << vec.z << std::endl;
}


// Function to create and initialize an SFML window with OpenGL context
void CreateWindow(std::unique_ptr<sf::RenderWindow>& windowPtr)
{
    math();
    // Configure OpenGL context settings
    sf::ContextSettings settings;
    // Request a 24-bit depth buffer for 3D rendering
    settings.depthBits = 24; 
    // Request an 8-bit stencil buffer for advanced effects
    settings.stencilBits = 8; 
    // Enable 4x antialiasing for smoother edges
    settings.antiAliasingLevel = 4; 
    // Request OpenGL version 4.3
    settings.majorVersion = 4; 
    settings.minorVersion = 3;
    // Use the core profile for modern OpenGL
    settings.attributeFlags = sf::ContextSettings::Core; 

    // Create the SFML window with the specified settings
    windowPtr = std::make_unique<sf::RenderWindow>
    (
        sf::VideoMode({WindowAttributes::WINDOW_WIDTH, 
                      WindowAttributes::WINDOW_HEIGHT}), 
                      WindowAttributes::WINDOW_TITLE, sf::State::Windowed,
                      settings 
    );

    // Check if OpenGL functions are loaded and the window is successfully created
    if (!gladLoadGL() | !windowPtr.get()->isOpen()) 
    {
        // Throw an exception if initialization fails
        throw std::runtime_error("ERROR::Failed to initialize window context.");
    }

    // Activate the OpenGL context for the window
    windowPtr.get()->setActive(true);

    // Enable vertical synchronization to limit the frame rate to the monitor's 
    // refresh rate (only works on fullscreen windows)
    //windowPtr.get()->setVerticalSyncEnabled(true);
}

int main()
{
    // Declare unique pointers for the SFML window and OpenGL state
    std::unique_ptr<sf::RenderWindow> window;
    std::unique_ptr<Renderer::GL_State> gl;

    try
    {
        // Create the SFML window with OpenGL context
        CreateWindow(window);

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
    while (window.get()->isOpen())
    {
        while (const std::optional event = window.get()->pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.get()->close();
            }
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
            {
                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
                {
                    window.get()->close();
                }
            }
        }
        // Clear the color and depth buffers for the next frame
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render the scene using the OpenGL state
        gl.get()->Draw();

        // Display the rendered frame (swap front and back buffers)
        window.get()->display();
    }

    // Program executed successfully
    return 0;
}