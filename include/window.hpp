#pragma once
#include <glad/glad.h> // For loading OpenGL function pointers.
// For SFML windows
#include <optional>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>


namespace WindowAttributes
{
    constexpr GLsizei WINDOW_WIDTH{ 1133 };
    constexpr GLsizei WINDOW_HEIGHT{ 755 };
    constexpr const char* WINDOW_TITLE{ "Textured rectangle" };
    // Configure OpenGL context settings
    inline sf::ContextSettings getSettings() 
    {
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
        return settings;
    }
};

class Window : public sf::RenderWindow
{
public:
    /**
     * @brief Constructs a Window object and initializes the rendering context.
     *
     * This constructor creates a window using the specified attributes from 
     * the WindowAttributes namespace, including width, height, title, and 
     * OpenGL settings. It also ensures that the OpenGL functions are loaded 
     * and the window is successfully created.
     *
     * @throws std::runtime_error If the OpenGL context fails to initialize 
     *         or the window cannot be opened.
     */
    Window();
    virtual ~Window() = default;

    // Getter for the clock_ member
    const sf::Clock* getClock() const { return clock_.get(); }

private:
    std::unique_ptr<sf::Clock> clock_;
};