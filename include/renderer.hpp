/**
 * @file renderer.hpp
 * @brief This header file defines the Renderer namespace and its associated 
 *        classes, constants, and utilities for OpenGL rendering.
 * 
 * The Renderer namespace provides a comprehensive set of tools for managing 
 * OpenGL rendering, including vertex attributes, textures, shaders, and 
 * buffer configurations. It also includes utility classes for handling 
 * images, textures, and OpenGL state management.
 * 
 * Key Components:
 * - Vertex: Contains definitions and utilities for handling vertex attributes 
 *           and their layout in vertex buffers.
 * - GlConstants: Provides constant values used for configuring rendering states.
 * - Image: Represents an image loaded from a file.
 * - Texture: Manages textures loaded from image files and provides functionality 
 *            for texture management.
 * - Shader: Represents GLSL shaders and provides functionality for shader 
 *           compilation and management.
 * - ShaderProgram: Manages shader programs by linking vertex and fragment shaders.
 * - BufferSetup: Handles OpenGL buffer setup and configuration, including VAOs, 
 *                VBOs, and EBOs.
 * - GL_State: Manages the OpenGL rendering context and sets up necessary 
 *             resources for rendering.
 * 
 * This file is part of a rendering system designed for use with SFML and OpenGL.
 * It provides a modular and extensible framework for creating and managing 
 * rendering pipelines.
 * 
 * @note This file assumes the presence of an OpenGL context and requires 
 *       appropriate OpenGL headers to be included.
 * 
 * @author [Anna Sivula]
 * @date [spring 2025]
 */

#pragma once
#include <window.hpp>  // For window attribute constants.
#include <vector>      // For using std::vector to store vertex and index data.
#include <stdexcept>   // For throwing exceptions like runtime_error.
#include <fstream>     // For reading shader source files.
#include <sstream>     // For constructing strings from file streams.
#include <memory>      // For using smart pointers like std::unique_ptr.
#include <string>      // For handling std::string operations.
#include <stb_image.h> // For loading image files into memory for textures.
#include <array>       // For using std::array for fixed-size arrays.
#include <glm/glm.hpp> // OpenGL Mathematics library
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



namespace Env
{
    constexpr const char* VERTEX_SHADER_PATH = "../../../shaders/shader.vs";
    constexpr const char* FRAG_SHADER_PATH = "../../../shaders/shader.fs";
    constexpr const char* SHELF_TEXTURE_PATH = "../../../resources/sky.jpg";
    constexpr const char* DUCKY_TEXTURE_PATH = "../../../resources/rubber-ducky.png";
};

/**
 * @namespace VerticeDataVector
 * @brief This namespace provides constants and enumerations to define the
 * layout of vertex attributes such as position, color, and texture
 * coordinates.
 **/
namespace VerticeDataVector
{
    //The total number of components in a single vertex.
    constexpr GLuint STRIDE = 5;
    // The location for vertex position in the vertices data.
    constexpr GLuint POSITION_LOCATION = 0;
    // The location for vertex texture in the vertices data. 
    constexpr GLuint TEXTURE_LOCATION = 3;
    // The number of values in the position attribute.
    constexpr GLuint POSITION_SIZE = 3;
    // The number of values in the texture coordinate attribute.
    constexpr GLuint TEXTURE_SIZE = 2;
};


/**
 * @enum VSLocation
 * @brief Enum class for location attributes in vertex shader
 */
enum class VSLocation : std::uint8_t
{
    POSITION = 0, ///< Vertex positions
    TEXTURE,  ///< Texture coordinates  
};


/**
 * @namespace Renderer
 * @brief Provides a collection of classes, constants, and utilities for 
 *        rendering using OpenGL.
 */
namespace Renderer 
{
  /**
     * @namespace GlConstants
     * @brief Contains OpenGL constants used for rendering configurations.
     */
    namespace GlConstants
    {
        // Specifies the default drawing mode (e.g., GL_TRIANGLES).
        constexpr GLenum DRAW_MODE = GL_TRIANGLES;
        // Specifies the default texture unit (e.g., GL_TEXTURE0).
        constexpr GLenum DEFAULT_TEXTURE = GL_TEXTURE0;
        // Specifies the default texture unit index.
        constexpr GLint DEFAULT_TEXTURE_UNIT = 0;
        // Specifies the default buffer draw type (e.g., GL_STATIC_DRAW).
        constexpr GLenum DRAW_TYPE = GL_STATIC_DRAW;
        // Specifies the default number of indices for rendering.
        constexpr GLuint INDICES_COUNT = 6;
        // Specifies the data type of indices (e.g., GL_UNSIGNED_INT).
        constexpr GLenum INDICE_TYPE = GL_UNSIGNED_INT;
        // Specifies the red component of the default clear color.
        constexpr GLfloat CLEAR_COLOR_RED = 0.3f;
        // Specifies the green component of the default clear color.
        constexpr GLfloat CLEAR_COLOR_GREEN = 0.3f;
        // Specifies the blue component of the default clear color.
        constexpr GLfloat CLEAR_COLOR_BLUE = 0.3f;
        // Specifies the opacity of the default clear color.
        constexpr GLfloat CLEAR_COLOR_OPACITY = 0.5f;

    }; 

    /**
     * @class Image
     * @brief Represents an image loaded from a file.
     *
     * This class provides functionality to read an image from a specified file 
     * path and store its data along with metadata such as width, height, 
     * and number of channels.
     * */

    class Image 
    {
    public:
        /**
         * @param imagePath The file path to the image to be loaded.
         * @throws std::domain_error If the image cannot be read from the path.
         */
        explicit Image(const std::string& imagePath);
        virtual ~Image();

        // Delete copy constructor and copy assignment operator
        Image(const Image&) = delete;
        Image& operator=(const Image&) = delete;

    protected:

        /*** @brief The width of the image in pixels.*/
        int imgWidth_;
        /*** @brief The height of the image in pixels.*/
        int imgHeight_;
        /*** @brief The number of color channels in the image (e.g., 3 for RGB).*/
        int imgNumberOfChannels_;
        /*** @brief A pointer to the raw image data. */
        unsigned char* img_;
    };


    /**
     * @class Texture
     * @brief Represents a texture loaded from an image file and provides
     * functionality for texture management.
     * 
     * The Texture class is derived from the Image class and is used to handle 
     * textures. It encapsulates the loading of an image file and the creation 
     * of a 2D texture object.
     */
    class Texture final : public Image
    {
    public:
        /**
         * @brief Constructs a Texture object and initializes an OpenGL texture.
         * 
         * This constructor takes the path to an image file, loads the image data,
         * and creates an OpenGL texture. It sets texture parameters for wrapping
         * and filtering, uploads the texture data to the GPU, and generates mipmaps.
         * 
         * @param imagePath The file path to the image to be loaded as a texture.
         * 
         * @note OpenGL operations performed:
         * @note - Generates a texture ID using glGenTextures.
         * @note - Binds the texture to the GL_TEXTURE_2D target.
         * @note - Sets texture wrapping parameters (GL_REPEAT for both S and 
         *                                           T axes).
         * @note - Sets texture filtering parameters (GL_LINEAR_MIPMAP_LINEAR 
         * for minification, GL_LINEAR for magnification).
         * @note - Uploads the image data to the GPU using glTexImage2D.
         * @note - Generates mipmaps for the texture using glGenerateMipmap.
         */
        explicit Texture(const std::string& imagePath);
        virtual ~Texture() override = default;

        // Delete copy constructor and copy assignment operator
        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        /**
         * @brief Gets the unique identifier of the loaded texture.
         * @return The OpenGL texture ID.
         */
        unsigned int getTexID() const;

    private:
        /** @brief Unique identifier of the loaded texture. */
        // ReSharper disable once CppInconsistentNaming
        unsigned int texID_;
    };
    /**
     * @class Shader
     * @brief A class representing a GLSL shader.
     * This class provides functionality to manage and compile GLSL shaders.
     * It holds the shader source code and the compiled shader ID.
     */
    class Shader
    {
    public:
        /**
         * @brief Initializes source data from the provided source code.
         * @param sourcePath The location of the GLSL source code for the shader.
         * @return void This function does not return a value.
         * @throw domain_error When file at given source cannot be opened
        */
        Shader(const std::string& sourcePath);

        /**
         * @brief Default virtual destructor.
         * @return void This function does not return a value.
         */
        virtual ~Shader() = default;

        // Delete copy constructor and copy assignment operator.
        Shader(const Shader&) = delete;  
        Shader& operator=(const Shader&) = delete;

        /**
         * @brief Getter for the shader ID.
         * @return The shader ID.
         */
        unsigned int getShaderID() const 
        { 
            return shaderID_; 
        }
    protected:
        /**
         * @brief Generates a shader ID for the specified shader type.
         * This function creates a new shader object and assigns it an ID based 
         * on the provided shader type. The shader type must be one of the valid 
         * OpenGL shader types, such as GL_VERTEX_SHADER or GL_FRAGMENT_SHADER.
         * @param shaderType The type of shader to generate. 
         * Must be a valid GLenum value representing a shader type.
         * @return void This function does not return a value.
         */
        void generateID(GLenum shaderType);

        /**
         * @brief Checks if any errors happened and if a shader was compiled.
         * @throws std::logic_error if shader compilation failed.
         * @return void This function does not return a value.
        */
        void checkShaderCompilation(const std::string &shaderType) const;

        /**
         * @fn void Shader::compileShader();
         * @brief Compiles the shader from the provided source code that also
         * determines the type of the shader.
         * @note Requires that shaderID_ is set properly to a nonzero value.
         * @return void This function does not return a value.
         */
        void compileShader() const;

        /**
         * @var shaderSource_
         * @brief Contains the GLSL source code for the shader.
         */
        std::string shaderSource_;

        /** 
         * @var shaderID_
         * @brief An unsigned integer representing a compiled shader from a 
         * derived class object.
        */
        unsigned int shaderID_;

    };

    /**
     * @class VertexShader
     * @brief A class representing a vertex shader.
     * 
     * This class is derived from the Shader class and is used to handle vertex
     * shaders. It provides functionality to construct a vertex shader
     * with given source code and to generate an ID for the shader.
    **/
    class VertexShader final : public Shader
    {
    public:
        /**
         * @brief Constructs a VertexShader object with the given source code.
         * @return void This function does not return a value.
         */
        VertexShader();

        /**
         * @fn VertexShader::~VertexShader()
         * @brief Default destructor for the VertexShader class.
         * @return void This function does not return a value.
        */
        ~VertexShader() override = default;

        // Delete copy constructor and copy assignment operator
        VertexShader(const VertexShader&) = delete;  
        VertexShader& operator=(const VertexShader&) = delete;  
    };

    class FragmentShader final : public Shader  
    {  
    public:  
       /**  
        * @brief Constructs a FragmentShader object with the given source code.  
        * @return void This function does not return a value.  
        */  
       FragmentShader();  

       /**  
        * @fn FragmentShader::~FragmentShader()  
        * @brief Default destructor for the FragmentShader class.  
        * @return void This function does not return a value.  
        */  
       ~FragmentShader() override = default;  

       // Delete copy constructor and copy assignment operator  
       FragmentShader(const FragmentShader&) = delete;  
       FragmentShader& operator=(const FragmentShader&) = delete;  
    };

    class ShaderProgram
    {
    public:
        /**
         * @brief Constructs a shader program by linking a vertex shader and a
         *        fragment shader.
         * 
         * This constructor creates an OpenGL shader program by attaching
         * the provided vertex and fragment shaders, linking them, and checking 
         * for any linking errors. If the linking fails, an exception is thrown 
         * with the error log. After successful linking, the shaders are deleted 
         * as they are no longer needed.
         * 
         * @param vertexShaderID The ID of the compiled vertex shader.
         * @param fragShaderID The ID of the compiled fragment shader.
         * 
         * @throws std::runtime_error If the shader program linking fails.
         */
        ShaderProgram(const unsigned int vertexShaderID, 
                const unsigned int fragShaderID);
        ~ShaderProgram() = default;

        /**
         * @fn unsigned int ShaderProgram::getProgramID() const
         * @brief Getter for the shader program ID.
         * @return The shader program ID.
         */
        unsigned int getProgramID() const 
        { 
            return shaderProgram_; 
        }

        /**
         * @brief Sets a uniform variable in the shader program.
         * 
         * This function locates the uniform variable in the shader program by 
         * its name and assigns it the specified value. The value type is 
         * determined by the template parameter, allowing flexibility for
         * different uniform types (int, float, bool).
         * 
         * @tparam T The type of the uniform variable (int, float, bool, 
         *           4x4 matrix).
         * @param name The name of the uniform variable in the shader program.
         * @param value The value to set for the uniform variable.
         * 
         * @note The shader program must be bound before calling this function.
         * @note If the uniform variable name does not exist or is not used in
         *       the shader, glGetUniformLocation may return -1, and the
         *       operation will have no effect.
         */
        template <typename T>
        void setUniform(const std::string &name, T value) const
        {
            GLint location = glGetUniformLocation(shaderProgram_, name.c_str());
            if constexpr (std::is_same_v<T, bool>)
            {
                glUniform1i(location, static_cast<int>(value));
            }
            else if constexpr (std::is_same_v<T, int>)
            {
                glUniform1i(location, value);
            }
            else if constexpr (std::is_same_v<T, float>)
            {
                glUniform1f(location, value);
            }else if constexpr (std::is_same_v<T, glm::mat4>)
            {
                glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
            }
            else
            {
                throw std::invalid_argument("Unsupported uniform type");
            }
        }
        
    private:
        unsigned int shaderProgram_;
    };

    /**
     * @class BufferSetup
     * @brief Manages OpenGL buffer setup and configuration.
     * 
     * The BufferSetup class is responsible for creating and managing OpenGL 
     * buffers, including Vertex Array Objects (VAOs), Vertex Buffer Objects 
     * (VBOs), and Element Buffer Objects (EBOs). It provides functionality to
     * initialize these buffers with vertex and index data, 
     * configure vertex attributes, and manage multiple VBOs.
     * */
    class BufferSetup
    {
    public:
    
        /**
         * @fn BufferSetup::BufferSetup()
         * @brief Constructs a BufferSetup object and initializes OpenGL buffers.
         * 
         * This constructor generates and binds a Vertex Array Object (VAO), 
         * a Vertex Buffer Object (VBO), and an Element Buffer Object (EBO) if
         * indices member is defined with values. 
         * It then uploads the provided vertex and index data to the 
         * GPU and configures the vertex attributes.
         * */
        BufferSetup();

        /**
         * @brief Default destructor for the BufferSetup class.
         * 
         * This destructor is defined as the default, meaning it will automatically
         * clean up any resources managed by the BufferSetup class when an
         * instance of the class is destroyed.
         */
        ~BufferSetup() = default;
    
        // Delete copy constructor and copy assignment operator.
        BufferSetup& operator=(const BufferSetup&) = delete;

        /**
         * @brief Enables a vertex attribute for rendering by configuring its 
         *        pointer and enabling it.
         * 
         * This function sets up a vertex attribute pointer and enables 
         * the corresponding vertex attribute array based on the provided 
         * attribute type. It determines the attribute's location and size using 
         * predefined constants and configures the OpenGL state accordingly.
         * 
         * @param type The type of the vertex attribute to enable. This is an 
         *             enum value of type AttributeLoc, 
         *             which can represent POSITION, COLOR, or TEXTURE.
         * 
         * @note The function performs the following steps:
         * @note Determines the attribute location and size based on the 
         *       provided type.
         * @note Configures the vertex attribute pointer using 
         *       `glVertexAttribPointer`.
         * @note Enables the vertex attribute array using 
         *       `glEnableVertexAttribArray`.
         * 
         * @note The function assumes that the attribute type corresponds to a 
         *       valid predefined location and size in the VertexAttributes 
         *       namespace.
         * 
         * @warning Ensure that the OpenGL context is properly initialized and 
         *          that the vertex buffer is bound before calling this function.
         */
        void enableVertexAttribute(const VSLocation& type)
        const;

        /**
         * @brief Getter for VAO
         * 
         * This function returns the unique ID of the Vertex Array Object (VAO).
         * 
         * @return GLuint The unique ID of the VAO.
         */
        GLuint getVAOId() const 
        { 
            return vao_; 
        }
    
        /**
         * @brief Getter for EBO
         * 
         * This function returns the unique ID of the Element Buffer Object (EBO).
         * 
         * @return GLuint The unique ID of the EBO.
         */
        GLuint getEBOId() const
        { 
            return ebo_; 
        }
    
    private:
        /**
         * @brief ID created for Vertex Array Object. 
         **/
        GLuint vao_;
        /**
         * @brief ID created for Element Buffer Object. 
         **/
        GLuint ebo_;

        GLuint vbo_;

        /**
         * @brief A vector containing vertex data for a textured rectangle.
         * 
         * Each vertex is represented by 8 float values:
         * - 3 floats for position (x, y, z)
         * - 2 floats for texture coordinates (u, v)
         */
        std::vector<float> vertices_ =
        {
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
            0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
            0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
           -0.5f,  0.5f, -0.5f, 0.0f, 1.0f,
           -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
       
           -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,
            0.5f, -0.5f,  0.5f, 1.0f, 0.0f,
            0.5f,  0.5f,  0.5f, 1.0f, 1.0f,
            0.5f,  0.5f,  0.5f, 1.0f, 1.0f,
           -0.5f,  0.5f,  0.5f, 0.0f, 1.0f,
           -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,
       
           -0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
           -0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
           -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
           -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
           -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,
           -0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
       
            0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
            0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
            0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
            0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
            0.5f, -0.5f,  0.5f, 0.0f, 0.0f,
            0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
       
           -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
            0.5f, -0.5f,  0.5f, 1.0f, 0.0f,
            0.5f, -0.5f,  0.5f, 1.0f, 0.0f,
           -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,
           -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
       
           -0.5f,  0.5f, -0.5f, 0.0f, 1.0f,
            0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
            0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
            0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
           -0.5f,  0.5f,  0.5f, 0.0f, 0.0f,
           -0.5f,  0.5f, -0.5f, 0.0f, 1.0f
        };
                    //0, 1, 3, // First triangle: top right, bottom right, bottom left
            //1, 2, 3 // Second triangle: bottom right, bottom left, top left
            //0, 3, 5, // Third triangle: top right, top left, top middle
            //1, 2, 5  // Fourth triangle: bottom right, bottom left, bottom middle
        /**
         * @brief A vector containing index data for drawing triangles.
         * 
         * Each group of three indices represents a triangle, specifying the 
         * vertices in counter-clockwise order. These indices map to the 
         * `vertices_` vector to define the geometry of the object.
         */
        std::vector<unsigned int> indices_ ={};

    };

    class GL_State final
    {
    public:
        /**
         * @brief Constructor for the GL_State class, responsible for 
         *        initializing the OpenGL rendering context and setting up 
         *        necessary resources.
         *
         * @note This constructor performs the following tasks:
         * @note Sets the size of the OpenGL viewport to match the window 
         *       dimensions.
         * @note Enables OpenGL debug output and sets a callback for error 
         *       logging.
         * @note Specifies the clear color for the color buffer.
         * @note Clears the color buffer to apply the specified clear color.
         * @note Creates and compiles vertex and fragment shaders, and links 
         *       them into a shader program.
         * @note Allocates and uploads vertex data to a GPU buffer.
         * @note Loads textures from file paths specified in the environment 
         *       variables.
         *
         * @throws std::runtime_error If shader creation or linking fails.
         */
        GL_State();
        virtual ~GL_State() = default;
        /**
         * @brief Renders the scene by drawing the configured buffers and 
         *        textures.
         * 
         * This method binds the necessary OpenGL resources, such as shaders, 
         * textures, and buffers, and issues draw calls to render the scene. 
         * It assumes that the OpenGL context is properly initialized and active.
         * @param window The window context where to draw. 
         */
        void draw(const std::unique_ptr<Window>& window) const;
        
        // Delete copy constructor and copy assignment operator
        GL_State(const GL_State&) = delete;  
        GL_State& operator=(const GL_State&) = delete;  
    
    private:
        std::unique_ptr<ShaderProgram> shaderProgram_;
        std::unique_ptr<BufferSetup> myBuffer_;
        std::unique_ptr<Texture> shelfTexture_;
        std::unique_ptr<Texture> duckyTexture_;
        sf::Clock clock_;
        
    };

}