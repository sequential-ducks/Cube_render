#include "renderer.hpp"


/**
 * @fn MessageCallback
 * @brief Callback function for handling OpenGL debug messages.
 *
 * This function is called whenever an OpenGL debug message is generated.
 *
 * @param source Specifies which part of the OpenGL API the message
 *               originated from.
 * @param type Specifies the type of message, such as error, performance
 *             warning, etc.
 * @param id Specifies the ID of the message.
 * @param severity Specifies the severity level of the message.
 * @param length Specifies the length of the message string.
 * @param message A pointer to the message string.
 * @param userParam A pointer to user-supplied data.
 */
void GLAPIENTRY MessageCallback
(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
    const GLchar* message, const void* userParam)
{
    // Mark unused params
    (void)source;
    (void)id;
    (void)userParam;
    (void)severity;
    (void)length;
    void(fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
        (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
        type, severity, message));
}

Renderer::Image::Image(const std::string& imagePath)
{
    // Load the image from the specified file path
    img_ = stbi_load(imagePath.c_str(), &imgWidth_, &imgHeight_,
        &imgNumberOfChannels_, 0);

    // Check if the image failed to load
    if (img_ == nullptr)
    {
        // Throw an exception with an error message if the image cannot be loaded
        throw std::domain_error("ERROR::CANNOT LOAD IMAGE " + imagePath);
    }
}

Renderer::Image::~Image()
{
    // Free the image memory
    stbi_image_free(img_);
    img_ = nullptr;
}

Renderer::Texture::Texture(const std::string& imagePath) : Image(imagePath)
{
    // Generate a texture ID and store it in texID_
    glGenTextures(1, &texID_);

    // Bind the texture object to the GL_TEXTURE_2D target
    glBindTexture(GL_TEXTURE_2D, texID_);

    // Set texture wrapping parameters for the S and T axes
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Set texture filtering parameters for minification and magnification
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Image is RGB (no alpha)
    if (imgNumberOfChannels_ == 3)
    {
        // Specify a 2D texture image using the loaded image data
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgWidth_, imgHeight_, 0,
            GL_RGB, GL_UNSIGNED_BYTE, img_);
    }
    else if (imgNumberOfChannels_ == 4)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgWidth_, imgHeight_, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, img_);
    }


    // Generate mipmaps for the texture
    glGenerateMipmap(GL_TEXTURE_2D);
}

unsigned int Renderer::Texture::getTexID() const
{
    return texID_;
}


Renderer::Shader::Shader(const std::string& sourcePath) : shaderID_{ 0 }
{
    // Declare a string to hold the shader source code
    std::string shaderCode;

    // Create an ifstream object to read the shader file
    std::ifstream shaderFile;

    // Configure the ifstream object to throw exceptions on failure
    shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        // Open the shader file at the specified path
        shaderFile.open(sourcePath);

        // Create a stringstream to read the file's contents
        std::stringstream shaderStream;

        // Read the entire file's buffer contents into the stringstream
        shaderStream << shaderFile.rdbuf();

        // Close the file after reading its contents
        shaderFile.close();

        // Convert the shader source code string to a C-style string
        shaderSource_ = shaderStream.str();

    }
    catch (const std::ifstream::failure& e)
    {
        throw std::domain_error(std::string("ERROR::CANNOT OPEN::") +
            sourcePath + " " + e.what());
    }
}

void Renderer::Shader::generateID(GLenum shaderType)
{
    shaderID_ = glCreateShader(shaderType);
}

void Renderer::Shader::checkShaderCompilation(const std::string& shaderType) const
{
    // Variable to store the compilation status
    int success;

    // Buffer to store the error log if compilation fails

    // Get the compilation status of the shader
    glGetShaderiv(shaderID_, GL_COMPILE_STATUS, &success);

    // If compilation failed, retrieve and throw the error log
    if (!success)
    {
	    char infoLog[512];
	    glGetShaderInfoLog(shaderID_, 512, NULL, infoLog);
        throw std::runtime_error(std::string("ERROR::SHADER::") + shaderType +
            std::string("::COMPILATION_FAILED\n ")
            + infoLog);
    }
}

// Compiles the shader source code into a shader object
void Renderer::Shader::compileShader() const
{
    // Convert the shader source code to a C-style string
    const GLchar* source = shaderSource_.c_str();

    // Set the source code for the shader object
    glShaderSource(shaderID_, 1, &source, NULL);

    // Compile the shader object
    glCompileShader(shaderID_);
}

// Constructor for VertexShader, initializes a vertex shader
Renderer::VertexShader::VertexShader() : Shader(Env::VERTEX_SHADER_PATH)
{
    // Generate a shader ID for a vertex shader
    generateID(GL_VERTEX_SHADER);

    // Compile the vertex shader
    compileShader();

    // Check for compilation errors
    checkShaderCompilation("VERTEX");
}

// Constructor for FragmentShader, initializes a fragment shader
Renderer::FragmentShader::FragmentShader() : Shader(Env::FRAG_SHADER_PATH)
{
    // Generate a shader ID for a fragment shader
    generateID(GL_FRAGMENT_SHADER);

    // Compile the fragment shader
    compileShader();

    // Check for compilation errors
    checkShaderCompilation("FRAGMENT");
}

// Constructor for ShaderProgram, links vertex and fragment shaders into a program
Renderer::ShaderProgram::ShaderProgram(const unsigned int vertexShaderID,
    const unsigned int fragShaderID)
{
    int success; // Variable to store the linking status

    // Create a shader program
    shaderProgram_ = glCreateProgram();

    // Attach the vertex shader to the program
    glAttachShader(shaderProgram_, vertexShaderID);

    // Attach the fragment shader to the program
    glAttachShader(shaderProgram_, fragShaderID);

    // Link the shaders into the program
    glLinkProgram(shaderProgram_);

    // Check for linking errors
    glGetProgramiv(shaderProgram_, GL_LINK_STATUS, &success);
    if (!success)
    {
	    char infoLog[512];
	    // Retrieve and throw the error log if linking fails
        glGetProgramInfoLog(shaderProgram_, 512, NULL, infoLog);
        throw std::runtime_error(std::string("ERROR::SHADER::PROGRAM::LINKING_FAILED\n")
            + infoLog);
    }

    // Delete the vertex shader as it is no longer needed
    glDeleteShader(vertexShaderID);

    // Delete the fragment shader as it is no longer needed
    glDeleteShader(fragShaderID);
}


Renderer::GL_State::GL_State() : shaderProgram_{ nullptr }, myBuffer_{ nullptr },
shelfTexture_{ nullptr }, duckyTexture_{ nullptr }, clock_()
{
    // Set the size of the initial OpenGL rendering context
    glViewport(0, 0, WindowAttributes::WINDOW_WIDTH,
        WindowAttributes::WINDOW_HEIGHT);

    // Enabled debug logging
    glEnable(GL_DEBUG_OUTPUT);
    // Determine callback function that is called whenever an error occurs.
    glDebugMessageCallback(MessageCallback, 0);
    glEnable(GL_DEPTH_TEST);


    // Set color buffer clear color 
    glClearColor
    (
        Renderer::GlConstants::CLEAR_COLOR_RED,
        Renderer::GlConstants::CLEAR_COLOR_GREEN,
        Renderer::GlConstants::CLEAR_COLOR_BLUE,
        Renderer::GlConstants::CLEAR_COLOR_OPACITY
    );
    glClear(GL_COLOR_BUFFER_BIT);

    // Create shader objects, if fail throws runtime error
    auto vertexShader = VertexShader();
    auto fragShader = FragmentShader();

    // Use the compiled shaders to get a linked shader program
    shaderProgram_ = std::make_unique<ShaderProgram>(vertexShader.getShaderID(),
        fragShader.getShaderID());

    // Move vertices data to the GPU buffer
    myBuffer_ = std::make_unique<BufferSetup>();

    // Load a texture
    shelfTexture_ = std::make_unique<Texture>(Env::SHELF_TEXTURE_PATH);
    duckyTexture_ = std::make_unique<Texture>(Env::DUCKY_TEXTURE_PATH);

}

void Renderer::GL_State::draw(const std::unique_ptr<Window>& window) const
{
    // Use the shader program for rendering
    glUseProgram(shaderProgram_->getProgramID());

    // Activate the default texture and bind the shelf texture
    glActiveTexture(Renderer::GlConstants::DEFAULT_TEXTURE);
    glBindTexture(GL_TEXTURE_2D, shelfTexture_->getTexID());

    // Activate the next texture unit and bind the ducky texture
    glActiveTexture(Renderer::GlConstants::DEFAULT_TEXTURE + 1);
    glBindTexture(GL_TEXTURE_2D, duckyTexture_->getTexID());

    // Set the uniform variables in the shader for the textures
    // "texture1" corresponds to the shelf texture bound to texture unit 0
    shaderProgram_->setUniform("texture1",
                               Renderer::GlConstants::DEFAULT_TEXTURE_UNIT);

    // "texture2" corresponds to the ducky texture bound to texture unit 1
    shaderProgram_->setUniform("texture2",
                               Renderer::GlConstants::DEFAULT_TEXTURE_UNIT + 1);

    glm::mat4 view = glm::mat4(1.0f);
    // The model matrix consists of translations, scaling and/or 
    // rotations we'd like to apply to transform all object's vertices to 
    // the global world space. 
    // This rotates a plane slightly along the negative x-axis.
    glm::mat4 model = glm::rotate(
        glm::mat4(1.0f),
        clock_.getElapsedTime().asSeconds() * glm::radians(50.0f),
        glm::vec3(0.5f, 1.0f, 0.0f));
    // Translate the scene forward (towards negative z) to give impression
    // of moving forward
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    // Use a perspective projection
    glm::mat4 projection = glm::perspective(
	    glm::radians(45.0f),
	    static_cast<float>(WindowAttributes::WINDOW_WIDTH) / static_cast<float>(WindowAttributes::WINDOW_HEIGHT),
	    0.1f,
	    1000.0f
    );
    // Set the vertices coordinate transformation matrices in our shader program.
    shaderProgram_->setUniform("model", model);
    shaderProgram_->setUniform("projection", projection);
    shaderProgram_->setUniform("view", view);

    // Bind the Vertex Array Object (VAO) that contains the vertex data
    glBindVertexArray(myBuffer_->getVAOId());
    glDrawArrays(GL_TRIANGLES, 0, 36);

}

Renderer::BufferSetup::BufferSetup()
{
    // Generate and bind the Vertex Array Object (VAO)
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    // Generate and bind the Vertex Buffer Object (VBO)
    glGenBuffers(1, &vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);

    // Upload vertex data to the GPU
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices_.size(),
        vertices_.data(), Renderer::GlConstants::DRAW_TYPE);

    // Check if there are indices to set up an Element Buffer Object (EBO)
    if (!indices_.empty())
    {
        // Generate and bind the Element Buffer Object (EBO)
        glGenBuffers(1, &ebo_);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);

        // Upload index data to the GPU
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
            indices_.size() * sizeof(unsigned int), indices_.data(),
            Renderer::GlConstants::DRAW_TYPE);
    }

    // Adjust the VAO to hold specific vertex attributes 
    enableVertexAttribute(VSLocation::POSITION);
    //enableVertexAttribute(Renderer::Vertex::Location::COLOR);
    enableVertexAttribute(VSLocation::TEXTURE);
}


void Renderer::BufferSetup::enableVertexAttribute(const VSLocation& type)
    const
{
    GLint attribute_location{ 0 };
    GLint attribute_size{ 0 };

    // Determine the attribute location and size based on the attribute type
    switch (type)
    {
    case VSLocation::POSITION:
        attribute_location = VerticeDataVector::POSITION_LOCATION;
        attribute_size = VerticeDataVector::POSITION_SIZE;
        break;

    case VSLocation::TEXTURE:
        attribute_location = VerticeDataVector::TEXTURE_LOCATION;
        attribute_size = VerticeDataVector::TEXTURE_SIZE;
        break;
    }

    // Define the vertex attribute pointer for the given type
    glVertexAttribPointer(static_cast<unsigned int>(type), attribute_size,
        GL_FLOAT, GL_FALSE,
        VerticeDataVector::STRIDE * sizeof(float),
        (void*)(attribute_location * sizeof(float)));

    // Enable the vertex attribute array for the given type
    glEnableVertexAttribArray(static_cast<unsigned int>(type));
}