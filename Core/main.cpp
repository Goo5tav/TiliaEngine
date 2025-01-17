#include "vendor/glad/KHR_Debug_openGL_3_3/include/glad/glad.h"
#include "vendor/glfw/include/GLFW/glfw3.h"
#undef APIENTRY

#include "vendor/glm/include/glm/glm.hpp"
#include "vendor/glm/include/glm/gtc/matrix_transform.hpp"
#include "vendor/glm/include/glm/gtc/type_ptr.hpp"
#include "vendor/glm/include/glm/gtx/string_cast.hpp"

#include "vendor/stb_image/include/stb_image/stb_image.h"
#include <iostream>
#include <memory>
#include <vector>
#include <map>

#include "Values/Directories.hpp"

#include TILIA_OPENGL_3_3_CONSTANTS_INCLUDE
#include TILIA_OPENGL_3_3_SHADER_INCLUDE
#include TILIA_OPENGL_3_3_SHADER_PART_INCLUDE
#include TILIA_OPENGL_3_3_TEXTURE_2D__INCLUDE
#include TILIA_OPENGL_3_3_CUBE_MAP_INCLUDE
#include TILIA_OPENGL_3_3_ERROR_HANDLING_INCLUDE
#include TILIA_LOGGING_INCLUDE
#include TILIA_OPENGL_3_3_BATCH_INCLUDE
#include TILIA_OPENGL_3_3_RENDERER_INCLUDE
#include TILIA_OPENGL_3_3_MESH_INCLUDE
#include TILIA_OPENGL_3_3_SHADER_DATA_INCLUDE
#include TILIA_TILIA_EXCEPTION_INCLUDE
#include TILIA_EXCEPTION_HANDLER_INCLUDE
#include TILIA_OPENGL_3_3_CONSTANTS_INCLUDE
#include TILIA_TEMP_CAMERA_INCLUDE
#include TILIA_TEMP_INPUT_INCLUDE
#include TILIA_TEMP_LIMIT_FPS_INCLUDE
#include TILIA_TEMP_STOPWATCH_INCLUDE
#include TILIA_OPENGL_3_3_UNIFORM_BUFFER_INCLUDE
#include TILIA_CONSTANTS_INCLUDE
#include TILIA_OPENGL_3_3_BUFFER_INCLUDE
#include TILIA_WINDOW_INCLUDE
#include TILIA_MONITOR_INCLUDE
#include TILIA_IMAGE_INCLUDE

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput();

void error_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{

    //std::cout << "OpenGL error"
    //    << "\nSource: " << source
    //    << "\nType: " << type
    //    << "\nId: " << id
    //    << "\nSeverity: " << severity
    //    << "\nMessage: " << message << '\n';

}

// settings
int SCR_WIDTH = 1440;
int SCR_HEIGHT = 810;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

using namespace tilia;
using namespace tilia::utils;
using namespace tilia::gfx;
using namespace tilia::log;

static Logger& logger{ Logger::Instance() };

static Exception_Handler& handler{ Exception_Handler::Instance() };

bool print_opengl_things{ false };

enums::Polymode polymode{ enums::Polymode::Fill };

bool pause{ true };

bool reload_shader{ false };

float light_z{ 0.0f };

float angle{};
float add_angle{ 60.0f };

glm::bvec3 axis{};

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

uint32_t passed_frames{};
uint32_t fps{};

Input_Manager input;

template<size_t size>
void create_square(Mesh<size>& mesh, glm::mat4 model, std::array<glm::vec2, 4> uv, uint32_t tex_slot);

template<size_t size>
void create_square(Mesh<size>& mesh, glm::mat4 model);

template<size_t size>
void create_circle(Mesh<size>& mesh, glm::mat4 model, size_t segment_count);

template<size_t size>
void create_cube(Mesh<size>& mesh, glm::mat4 model, bool complex = false);

template<size_t size>
void create_sphere(Mesh<size>& mesh, glm::mat4 model, uint32_t tex_index = 0);

void content_scale_callback(GLFWwindow* window, float x_scale, float y_scale)
{
    std::cout << window << " :  x_scale: " << x_scale << " :  y_scale: " << y_scale << '\n';
}

void test_function(bool value)
{

}

#if 0

int main()
{
    unsigned int shaderProgram;
    unsigned int VBO, VAO, EBO;

    try
    {

        std::cin >> print_opengl_things;

        logger.Add_Output(&std::cout);

        logger.Set_OpenGL_Filters({ "debug severity message" });

        if (print_opengl_things == true)
        {
            logger.Set_Output_Filters(&std::cout, { "debug severity message" });
        }

        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);



        // glfw window creation
        // --------------------
        GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
        if (window == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return -1;
        }
        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

        input.Init(window);

        glfwSwapInterval(0);

        //glad: load all OpenGL function pointers
        //---------------------------------------
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return -1;
        }

        const char* vertexShaderSource = "#version 330 core\n"
            "layout (location = 0) in vec3 aPos;\n"
            "void main()\n"
            "{\n"
            "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
            "}\0";
        const char* fragmentShaderSource = "#version 330 core\n"
            "out vec4 FragColor;\n"
            "void main()\n"
            "{\n"
            "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
            "}\n\0";

        // build and compile our shader program
        // ------------------------------------
        // vertex shader
        unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
        glCompileShader(vertexShader);
        // check for shader compile errors
        int success;
        char infoLog[512];
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        }
        // fragment shader
        unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
        glCompileShader(fragmentShader);
        // check for shader compile errors
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        }
        // link shaders
        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);
        // check for linking errors
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        // set up vertex data (and buffer(s)) and configure vertex attributes
        // ------------------------------------------------------------------
        float vertices[] = {
             0.5f,  0.5f, 0.0f,  // top right
             0.5f, -0.5f, 0.0f,  // bottom right
            -0.5f, -0.5f, 0.0f,  // bottom left
            -0.5f,  0.5f, 0.0f   // top left 
        };
        unsigned int indices[] = {  // note that we start from 0!
            0, 1, 3,  // first Triangle
            1, 2, 3   // second Triangle
        };
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
        //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
        // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
        glBindVertexArray(0);

        while (!glfwWindowShouldClose(window))
        {

            processInput();

            // render
            // ------
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            // draw our first triangle
            glUseProgram(shaderProgram);
            glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
            //glDrawArrays(GL_TRIANGLES, 0, 6);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            // glBindVertexArray(0); // no need to unbind it every time 

            glfwSwapBuffers(window);
            glfwPollEvents();

            handler.Update();

        }

    }
    catch (const utils::Tilia_Exception& e) {
        logger.Output(e);
    }
    catch (const std::exception& e) {
        logger.Output(e.what(), '\n');
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();

    while (true);

    return 0;

}

#endif

#if 0

#define CATCH_CONFIG_RUNNER
#include "vendor/Catch2/Catch2.hpp"

int main(int argc, char* argv[])
{
    
    int retval{};

    std::cin.get();

    try
    {

        // glfw: initialize and configure
            // ------------------------------
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
        
        // glfw window creation
        // --------------------
        GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
        if (window == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return -1;
        }
        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

        // tell GLFW to capture our mouse
        //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        input.Init(window);

        glfwSwapInterval(0);

        //glad: load all OpenGL function pointers
        //---------------------------------------
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return -1;
        }

        Catch::Session session{};
        retval = session.run(argc, argv);

        while (!glfwWindowShouldClose(window))
        {
            // input
            // -----
            processInput();



            // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
            // -------------------------------------------------------------------------------
            glfwSwapBuffers(window);
            glfwPollEvents();
        }

    }
    catch (const utils::Tilia_Exception& e) {
        std::cout << "\n<<<Tilia_Exception>>>\n";
        std::cout << e.what() << '\n';
    }
    catch (const std::exception& e) {
        std::cout << e.what() << '\n';
    }
    
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();

    std::cin.get();

    return retval;
}

TEST_CASE("Tilia_Exception", "[Tilia_Exception]") {
    tilia::utils::Tilia_Exception::Test();
}

TEST_CASE("Logger", "[Logger]") {
    tilia::log::Logger::Test();
}

TEST_CASE("Error_Handling", "[Error_Handling]") {
    tilia::utils::Error_Handling::Test();
}

TEST_CASE("Exception_Handler", "[Exception_Handler]") {
    tilia::utils::Exception_Handler::Test();
}

TEST_CASE("Monitor", "[Monitor]") {
    tilia::monitoring::Monitor::Test();
}

TEST_CASE("Image", "[Image]") {
    tilia::Image::Test();
}

#endif

#if 1

windowing::Window window{};

int main()
{

    try
    {

        std::cin >> print_opengl_things;

        logger.Add_Output(std::cout.rdbuf());

        logger.Set_OpenGL_Filters({ "debug severity message" });

        if (print_opengl_things == true)
        {
            logger.Add_Output_Filter(std::cout.rdbuf(), "debug severity message");
        }

        // glfw: initialize and configure
        // ------------------------------
        windowing::Window::Init();

        //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        //glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

        //glfwSetErrorCallback(Logger::GLFW_Error_Callback);

        //// glfw window creation
        //// --------------------
        //GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
        //if (window == NULL)
        //{
        //    std::cout << "Failed to create GLFW window" << std::endl;
        //    glfwTerminate();
        //    return -1;
        //}
        //glfwMakeContextCurrent(window);

        windowing::hints::context::Verion_Major(3);
        windowing::hints::context::Verion_Minor(3);
        windowing::hints::context::OpenGL_Profile(windowing::enums::context::OpenGL_Profile::Core);
        windowing::hints::context::OpenGL_Debug_Context(true);
        windowing::hints::Transparent_Framebuffer(true);

        window.Init(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", nullptr, nullptr);

        window.Make_Context_Current();

        //glad: load all OpenGL function pointers
        //---------------------------------------
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return -1;
        }

        window.Add_Callback(windowing::callbacks::Framebuffer_Size{ framebuffer_size_callback });

        window.Add_Callback(windowing::callbacks::Content_Scale{ content_scale_callback });

        //window.Set<enums::Window_Properties::Floating>({ true });

        window.Set<windowing::enums::Property::Swap_Interval>(0);

        monitoring::Monitor primary_monitor{ monitoring::Monitor::Get_Monitors()[0] };

        auto resolution{ primary_monitor.Get<monitoring::enums::Property::Resolution>() };
        auto refresh_rate{ primary_monitor.Get<monitoring::enums::Property::Refresh_Rate>() };

        std::cout << "primary monitor: " << resolution.first << " : " << resolution.second << " : " << refresh_rate << '\n';

        auto supported_refresh_rates{ primary_monitor.Get<monitoring::enums::Property::Supported_Refresh_Rates>(resolution) };
        const auto s_r_r_count{ supported_refresh_rates.size() };

        for (std::size_t i{ 0 }; i < s_r_r_count; ++i)
        {
            std::cout << "Supported refresh rate #" << i << " : " << supported_refresh_rates[i] << '\n';
        }
        
        //window.Set<windowing::enums::Property::Monitor>(primary_monitor);

        //std::cout << std::boolalpha << window.Get<windowing::enums::Property::Fullscreen>() << '\n';

        //window.Set<windowing::enums::Property::Monitor>(nullptr);

        //std::cout << std::boolalpha << window.Get<windowing::enums::Property::Fullscreen>() << '\n';

        //glfwSetWindowMonitor(window.Get<enums::Window_Properties::Underlying_Window>(), nullptr, 3840 >> 1, 2160 >> 1, 3840, 2160, GLFW_DONT_CARE);

        //window.Set(windowing::properties::Floating{ true });

        //window.Set<enums::Window_Properties::Should_Close>({ true });
        //window.Set<enums::Window_Properties::Should_Close>({ false });

        //window.Set(windowing::properties::Should_Close{ true });
        //window.Set(windowing::properties::Should_Close{ false });

        std::cout << window.Get<windowing::enums::Property::Underlying_Window>() << '\n';

        auto [content_scale_x, content_scale_y] { primary_monitor.Get<monitoring::enums::Property::Content_Scale>() };

        std::cout << content_scale_x << " : " << content_scale_y << '\n';

        //window.Set<enums::Window_Properties::Should_Close>(windowing::properties::Should_Close{ true });

        //window.Set(windowing::properties::Floating{ true });

        glClearColor(0.2f, 0.3f, 0.5f, 1.0f);
        //glClearColor(1.0f, 1.0f, 1.0f, 0.5f);

        //window.Set(window::properties::Should_Close{ true });

        //window.Set(window::properties::Opacity{ 0.0f });

        //while (window.Get(window::properties::Opacity{}) == 1.0f)
        //{

        //}

        //window.Set(window::properties::Size{ 5, 7 });

        //auto[ v1, v2, v3, v4 ] = window.Get(window::properties::Frame_Size{});

        //std::cout << v1 << " : " << v2 << " : " << v3 << " : " << v4 << '\n';

        //std::cout << x << " : " << y << " : " << z << " : " << w << '\n';

        //properties::Set_Window_Property<enums::Window_Properties::Aspect_Ratio, int> p{ 5 };

        //bool fwfw{};

        //window.Get_Property(properties::Get_Should_Close{ fwfw });

        //window.Set<enums::Window_Properties::Aspect_Ratio>(16, 9);

        //glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

        // tell GLFW to capture our mouse
        //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        //window.Set(windowing::properties::Swap_Interval{ 0 });

        //glfwSwapInterval(0);

        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

        glDebugMessageCallback(Logger::OpenGL_Error_Callback, &window);

        glfwSetErrorCallback(Logger::GLFW_Error_Callback);

        //int num{};
        //
        //glGetIntegerv(GL_MAX_DEBUG_LOGGED_MESSAGES, &num);
        //
        //std::cout << "Max debug logged messages " << num << '\n';
        //
        //std::int32_t extension_count{};
        //glGetIntegerv(GL_NUM_EXTENSIONS, &extension_count);

        //for (std::int32_t i{ 0 }; i < extension_count; ++i)
        //{
        //    const char* extension{ reinterpret_cast<const char*>(glGetStringi(GL_EXTENSIONS, i)) };
        //    std::cout << i << " : " << extension << '\n';
        //}

        input.Init(window.Get<windowing::enums::Property::Underlying_Window>());

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // configure global opengl state
        // -----------------------------
        glLineWidth(2);
        glPointSize(10);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_STENCIL_TEST);

        Renderer renderer{};

        camera.Reset();

        auto
        shader_v{ std::make_shared<Shader_Part>("res/shaders/geometry.vert", enums::Shader_Type::Vertex, true) }, 
        shader_f{ std::make_shared<Shader_Part>("res/shaders/geometry.frag", enums::Shader_Type::Fragment, true) },
        shader_g{ std::make_shared<Shader_Part>("res/shaders/geometry.geom", enums::Shader_Type::Geometry, true) };

        auto shader{ std::make_shared<Shader>() };

        shader->Init({ shader_v }, { shader_f }, { shader_g });

        int v{}, f{}, g{}, s{};

        glGetIntegerv(GL_MAX_VERTEX_UNIFORM_BLOCKS, &v);
        glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_BLOCKS, &f);
        glGetIntegerv(GL_MAX_GEOMETRY_UNIFORM_BLOCKS, &g);
        glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &s);

        std::cout << "V: " << v << " : F: " << f << " : G: " << g << " : S: " << s << '\n';

        glm::mat<4, 4, float, glm::qualifier::packed_lowp> mat{ 1.0f };

        std::cout << "Size: " << sizeof(mat) 
        << " : [0] offset: " << reinterpret_cast<std::uintptr_t>(&mat[0]) - reinterpret_cast<std::uintptr_t>(&mat) 
        << " : [1] offset: " << reinterpret_cast<std::uintptr_t>(&mat[1]) - reinterpret_cast<std::uintptr_t>(&mat)
        << " : [2] offset: " << reinterpret_cast<std::uintptr_t>(&mat[2]) - reinterpret_cast<std::uintptr_t>(&mat)
        << " : [3] offset: " << reinterpret_cast<std::uintptr_t>(&mat[3]) - reinterpret_cast<std::uintptr_t>(&mat) << '\n';

        Uniform_Buffer ub{};

        ub.Init({ { "projection", { enums::GLSL_Scalar_Type::Float, enums::GLSL_Container_Type::Matrix4 } }, { "view", { enums::GLSL_Scalar_Type::Float, enums::GLSL_Container_Type::Matrix4 } } }, true);

        //ub.debug_print();

        ub.Bind();

        //GL_CALL(glEnable(GL_FALSE));

        std::uint32_t uniform_buffer{ };
        GL_CALL(glGenBuffers(1, &uniform_buffer));

        GL_CALL(glBindBufferBase(GL_UNIFORM_BUFFER, 0, ub.Get_ID()));
        GL_CALL(glBindBufferBase(GL_UNIFORM_BUFFER, 0, uniform_buffer));

        ub.Set_Bind_Point(0);

        shader->Bind_Uniform_Block("Matrices", 0);

        Uniform_Buffer ub_2{ std::move(ub) };

        ub_2.Bind();

        auto mesh{ std::make_shared<Mesh<5>>() };

        mesh->Set_Shader()(shader);

        mesh->vertices.clear();
        mesh->indices.clear();
        
        mesh->vertices.push_back(Vertex<5>{ -0.5f,  0.5f, 1.0f, 0.0f, 0.0f });
        mesh->vertices.push_back(Vertex<5>{  0.5f,  0.5f, 0.0f, 1.0f, 0.0f });
        mesh->vertices.push_back(Vertex<5>{  0.5f, -0.5f, 0.0f, 0.0f, 1.0f });
        mesh->vertices.push_back(Vertex<5>{ -0.5f, -0.5f, 1.0f, 1.0f, 0.0f });

        mesh->indices.push_back(0);
        mesh->indices.push_back(1);
        mesh->indices.push_back(2);
        mesh->indices.push_back(3);

        Vertex_Info v_info{};

        v_info.sizes = { 2, 3 };
        v_info.strides = { 5 };
        v_info.offsets = { 0, 2 };

        mesh->Set_Vertex_Info()(v_info);

        renderer.Add_Mesh(mesh->Get_Mesh_Data());

        unsigned int framebuffer;
        glGenFramebuffers(1, &framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        // create a color attachment texture
        unsigned int textureColorbuffer;
        glGenTextures(1, &textureColorbuffer);
        glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
        //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH >> 2, SCR_HEIGHT >> 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
        
        // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
        unsigned int rbo;
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH >> 2, SCR_HEIGHT >> 2); // use a single renderbuffer object for both a depth AND stencil buffer.
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
        // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << '\n';
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        auto
            fbo_shader_v{ std::make_shared<Shader_Part>("res/shaders/texture.vert", enums::Shader_Type::Vertex, true) },
            fbo_shader_f{ std::make_shared<Shader_Part>("res/shaders/texture.frag", enums::Shader_Type::Fragment, true) };
        auto fbo_shader{ std::make_shared<Shader>() };

        fbo_shader->Init({ fbo_shader_v }, { fbo_shader_f }, { });

        float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
            // positions   // texCoords
            -1.0f,  1.0f,  0.0f, 1.0f,
            -1.0f, -1.0f,  0.0f, 0.0f,
             1.0f, -1.0f,  1.0f, 0.0f,

            -1.0f,  1.0f,  0.0f, 1.0f,
             1.0f, -1.0f,  1.0f, 0.0f,
             1.0f,  1.0f,  1.0f, 1.0f
        };
        
        // screen quad VAO
        unsigned int quadVAO, quadVBO;
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

        fbo_shader->Bind();

        fbo_shader->Uniform("texture_2d", { 0 });

        glEnable(GL_BLEND);

        Image image{ "res/textures/spaceInvader.png", enums::Image_Channels::Largest };

        window.Set<windowing::enums::Property::Icon>({ image });

        glBindTexture(GL_TEXTURE_2D, textureColorbuffer);

        Image image_0{ "res/textures/spaceInvader.png", enums::Image_Channels::Largest, enums::Image_Data_Type::Float, true };

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image_0.Width(), image_0.Height(), 0, GL_RGBA, GL_FLOAT, image_0.Get_Data());

        // render loop
        // -----------
        while (!window.Get<windowing::enums::Property::Should_Close>())
        {

            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
            glViewport(0, 0, SCR_WIDTH >> 2, SCR_HEIGHT >> 2);
            glEnable(GL_DEPTH_TEST);

            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

            processInput();

            if (reload_shader)
            {
                shader_v->Compile(true);
                shader_f->Compile(true);
                shader_g->Compile(true);
                fbo_shader_v->Compile(true);
                fbo_shader_f->Compile(true);
            }

            //if (static_cast<int>(glfwGetTime()) % 5 == 0)
                //window.Set(windowing::properties::Request_Attention{});

            mesh->Set_Polymode()(polymode);

            fps = static_cast<uint32_t>(static_cast<double>(passed_frames) / glfwGetTime());

            float currentFrame = static_cast<float>(glfwGetTime());
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            std::stringbuf title{};
            logger.Add_Output(&title, { "title" });
            logger.Add_Filter("title");

            //buffer << fps << " : " << deltaTime << " : " << add_angle << " : " << angle << " : " << axis.x << " , " << axis.y << " , " << axis.z;

            logger.Output(fps, " : ", deltaTime, " : ", add_angle, " : ", angle, " : ", axis.x, " , ", axis.y, " , ", axis.z, '\n');

            logger.Remove_Output(&title);
            logger.Remove_Filter("title");

            window.Set<windowing::enums::Property::Title>(title.str());
            
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

            //glfwSetWindowTitle(window.Get_Window(), title.str().c_str());
            //window.Set<enums::Window_Properties::Title>(title.str());

            //window.Set(windowing::properties::Should_Close{ true });

            // pass projection matrix to shader (note that in this case it could change every frame)
            glm::mat4 projection{ 1.0f };
            if (!window.Get<windowing::enums::Property::Iconify>())
            {
                projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.01f, 100.0f);
            }

            if (input.Get_Key_Pressed(KEY_RIGHT_SHIFT))
            {
                window.Set<windowing::enums::Property::Focus>(false);
            }

            ub_2.Uniform("projection", projection);
            //ub_2.Uniform("projection[0]", projection[0]);
            //ub_2.Uniform("projection[1]", projection[1]);
            //ub_2.Uniform("projection[2]", projection[2]);
            //ub_2.Uniform("projection[3]", projection[3]);
            // camera/view transformation
            glm::mat4 view{ camera.GetViewMatrix() };
            ub_2.Uniform("view", view);

            //ub_2.Map_Data();

            renderer.m_camera_pos = camera.Position;

            try
            {
                renderer.Render();
            }
            catch (const std::exception& e)
            {
                logger.Output(e.what(), '\n');
            }

            // now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
            //glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
            // clear all relevant buffers
            glClearColor(0.5f, 0.5f, 0.5f, 0.5f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            
            fbo_shader->Bind();
            glBindVertexArray(quadVAO);
            glBindTexture(GL_TEXTURE_2D, textureColorbuffer);	// use the color attachment texture as the texture of the quad plane
            glDrawArrays(GL_TRIANGLES, 0, 6);



            handler.Update();

            // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
            // -------------------------------------------------------------------------------
            window.Swap_Buffers();
            glfwPollEvents();
            input.Update();

            reload_shader = false;

            ++passed_frames;

        }

        window.Destroy();

        // optional: de-allocate all resources once they've outlived their purpose:
        // ------------------------------------------------------------------------

        // glfw: terminate, clearing all previously allocated GLFW resources.
        // ------------------------------------------------------------------
    }
    catch (const utils::Tilia_Exception& e) {
        logger.Output(e);
    }
    catch (const std::exception& e) {
        logger.Output(e.what(), '\n');
    }

    windowing::Window::Terminate();

    std::cin.get();

    return 0;
}

#endif

#if 0

int main()
{

    try
    {
        // glfw: initialize and configure
        // ------------------------------
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

        // glfw window creation
        // --------------------
        GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
        if (window == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return -1;
        }
        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

        // tell GLFW to capture our mouse
        //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        glfwSwapInterval(0);

        //glad: load all OpenGL function pointers
        //---------------------------------------
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return -1;
        }

        const GLubyte* extensions = glGetString(GL_EXTENSIONS);

        //std::cout << extensions << '\n';

        input.Init(window);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // configure global opengl state
        // -----------------------------
        glLineWidth(2);
        glPointSize(10);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_STENCIL_TEST);

        Renderer renderer{};

        camera.Reset();

        auto
            shader_v{ std::make_shared<Shader_Part>("res/shaders/compare.vert", enums::Shader_Type::Vertex, true) },
            shader_f{ std::make_shared<Shader_Part>("res/shaders/compare.frag", enums::Shader_Type::Fragment, true) };

        auto shader{ std::make_shared<Shader>() };

        shader->Init({ shader_v }, { shader_f }, {});

        auto mesh{ std::make_shared<Mesh<3>>() };

        mesh->Set_Shader()(shader);

        mesh->vertices.clear();
        mesh->indices.clear();

        mesh->vertices.push_back(Vertex<3>{ -1.0f, -1.0f, 0.0f });
        mesh->vertices.push_back(Vertex<3>{  1.0f, -1.0f, 0.0f });
        mesh->vertices.push_back(Vertex<3>{  1.0f, 1.0f, 0.0  });
        mesh->vertices.push_back(Vertex<3>{ -1.0f, 1.0f, 0.0f });

        mesh->indices.push_back(0);
        mesh->indices.push_back(1);
        mesh->indices.push_back(2);
        mesh->indices.push_back(2);
        mesh->indices.push_back(3);
        mesh->indices.push_back(0);

        mesh->Set_Polymode()(enums::Polymode::Fill);
        mesh->Set_Primitive()(enums::Primitive::Triangles);

        Vertex_Info v_info{};

        v_info.sizes = { 3 };
        v_info.strides = { 3 };
        v_info.offsets = { 0 };

        mesh->Set_Vertex_Info()(v_info);

        renderer.Add_Mesh(mesh->Get_Mesh_Data());

        // render loop
        // -----------
        while (!glfwWindowShouldClose(window))
        {

            fps = static_cast<uint32_t>(static_cast<double>(passed_frames) / glfwGetTime());

            float currentFrame = static_cast<float>(glfwGetTime());
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            std::cout << fps << '\n';

            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

            renderer.m_camera_pos = camera.Position;

            try
            {
                renderer.Render();
            }
            catch (const std::exception& e)
            {
                std::cout << e.what() << '\n';
            }

            // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
            // -------------------------------------------------------------------------------
            glfwSwapBuffers(window);
            glfwPollEvents();
            //input.Update();

            //reload_shader = false;

            ++passed_frames;

        }

        // optional: de-allocate all resources once they've outlived their purpose:
        // ------------------------------------------------------------------------

        // glfw: terminate, clearing all previously allocated GLFW resources.
        // ------------------------------------------------------------------
    }
    catch (const utils::Tilia_Exception & e) {
        std::cout << "\n<<<Tilia_Exception>>>\n";
        std::cout << e.what() << '\n';
    }
    catch (const std::exception & e) {
        std::cout << e.what() << '\n';
    }

    glfwTerminate();

    std::int32_t w{};
    std::cin >> w;

    return 0;
}

#endif

#if 0

int main()
{

    try
    {
        // glfw: initialize and configure
        // ------------------------------
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        // glfw window creation
        // --------------------
        GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
        if (window == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return -1;
        }
        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        
        // tell GLFW to capture our mouse
        //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        glfwSwapInterval(0);

        //glad: load all OpenGL function pointers
        //---------------------------------------
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return -1;
        }
        
        input.Init(window);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // configure global opengl state
        // -----------------------------
        glLineWidth(2);
        glPointSize(10);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_STENCIL_TEST);

        Renderer renderer{};

        camera.Reset();

        // auto light_shader{ std::make_shared<Shader<false>>(Shader<false>{ { "res/shaders/light_shader.vert" }, { "res/shaders/light_shader.frag" }, true }) };

        // auto cube_shader{ std::make_shared<Shader<false>>(Shader<false>{ { "res/shaders/cube_shader.vert" }, { "res/shaders/cube_shader.frag" }, true }) };

        auto 
        light_v_shader{ std::make_shared<Shader_Part>("res/shaders/light_shader.vert", enums::Shader_Type::Vertex, true) },
        light_f_shader{ std::make_shared<Shader_Part>("res/shaders/light_shader.frag", enums::Shader_Type::Fragment, true) };

        auto light_shader{ std::make_shared<Shader>() };

        light_shader->Init({ light_v_shader }, { light_f_shader }, {});

        auto 
        cube_v_shader{ std::make_shared<Shader_Part>("res/shaders/cube_shader.vert", enums::Shader_Type::Vertex, true) },
        cube_f_shader{ std::make_shared<Shader_Part>("res/shaders/cube_shader.frag", enums::Shader_Type::Fragment, true) };

        auto cube_shader{ std::make_shared<Shader>() };

        cube_shader->Init({ cube_v_shader }, { cube_f_shader }, {});

        Uniform_Buffer ub{};

        ub.Init({ { "projection", { enums::GLSL_Scalar_Type::Float, enums::GLSL_Container_Type::Matrix4 } }, { "view", { enums::GLSL_Scalar_Type::Float, enums::GLSL_Container_Type::Matrix4 } } }, true);

        ub.Set_Bind_Point(0);

        ub.Bind();

        light_shader->Bind_Uniform_Block("Matrices", 0);
        cube_shader->Bind_Uniform_Block("Matrices", 0);

        Cube_Map_Data def{};
        
        def.sides[0].file_path = "res/textures/container2.png";
        def.sides[1].file_path = "res/textures/container2.png";
        def.sides[2].file_path = "res/textures/container2.png";
        def.sides[3].file_path = "res/textures/container2.png";
        def.sides[4].file_path = "res/textures/container2.png";
        def.sides[5].file_path = "res/textures/container2.png";

        std::shared_ptr<Cube_Map> box_texture{ std::make_shared<Cube_Map>() };
        box_texture->Set_Cube_Map_Data(def, true);
        box_texture->Reload();
        
        // std::shared_ptr<Cube_Map> box_specular_texture{ std::make_shared<Cube_Map>() };
        // box_specular_texture->Set_Paths({
        //     "res/textures/container2_specular.png",
        //     "res/textures/container2_specular.png",
        //     "res/textures/container2_specular.png",
        //     "res/textures/container2_specular.png",
        //     "res/textures/container2_specular.png",
        //     "res/textures/container2_specular.png"
        // }, true);
        // box_specular_texture->Reload();
        
        //std::shared_ptr<Texture_2D_> tex_2d{ std::make_shared<Texture_2D_>() };
        //tex_2d->Set_Texture("res/teures/container2.png");

        constexpr size_t cube_count{ 10 };
        constexpr size_t light_count{ 1 };
        
        // positions all containers
        glm::vec3 cubePositions[] = {
            glm::vec3(0.0f,  0.0f,  0.0f),
            glm::vec3(2.0f,  5.0f, -15.0f),
            glm::vec3(-1.5f, -2.2f, -2.5f),
            glm::vec3(-3.8f, -2.0f, -12.3f),
            glm::vec3(2.4f, -0.4f, -3.5f),
            glm::vec3(-1.7f,  3.0f, -7.5f),
            glm::vec3(1.3f, -2.0f, -2.5f),
            glm::vec3(1.5f,  2.0f, -2.5f),
            glm::vec3(1.5f,  0.2f, -1.5f),
            glm::vec3(-1.3f,  1.0f, -1.5f)
        };
        // positions of the point lights
        glm::vec3 pointLightPositions[] = {
            glm::vec3(1.2f, 1.0f, 2.0f),
            glm::vec3(2.3f, -3.3f, -4.0f),
            glm::vec3(-4.0f,  2.0f, -12.0f),
            glm::vec3(0.0f,  0.0f, -3.0f)
        };

        std::vector<std::shared_ptr<Mesh<9>>> meshes{};
        
        glm::mat4 model{ 1.0f };

        for (size_t i = 0; i < cube_count; i++)
        {

            std::shared_ptr<Mesh<9>> new_mesh{ std::make_shared<Mesh<9>>() };

            new_mesh->Set_Shader()(light_shader);

            new_mesh->Add_Texture(box_texture);
            //new_mesh->Add_Texture(box_specular_texture);

            new_mesh->Set_Cull_Face()(enums::Face::Back);

            new_mesh->Set_Primitive()(enums::Primitive::Triangles);
            
            model = glm::translate(glm::mat4{ 1.0f }, cubePositions[i]);

            create_cube(*new_mesh, model, true);

            renderer.Add_Mesh(new_mesh->Get_Mesh_Data());

            meshes.push_back(new_mesh);

        }

        for (size_t i = 0; i < light_count; i++)
        {

            std::shared_ptr<Mesh<9>> new_mesh{ std::make_shared<Mesh<9>>() };

            new_mesh->Set_Shader()(cube_shader);

            new_mesh->Set_Cull_Face()(enums::Face::Back);

            new_mesh->Set_Primitive()(enums::Primitive::Triangles);

            model = glm::translate(glm::mat4{ 1.0f }, pointLightPositions[i]);
            model = glm::scale(model, { 0.2f, 0.2f, 0.2f });

            create_cube(*new_mesh, model);

            renderer.Add_Mesh(new_mesh->Get_Mesh_Data());

            meshes.push_back(new_mesh);

        }
    
        // render loop
        // -----------
        while (!glfwWindowShouldClose(window))
        {

            //S2D::utils::Sleep_Program(60);

            if (!pause)
                light_z += 1.0f * deltaTime;

            pointLightPositions[0].z = -sinf(static_cast<float>(light_z)) * 5.0f;
            
            for (size_t i = 0; i < cube_count + light_count; i++)
            {

                meshes[i]->Set_Polymode()(polymode);

                if (i < cube_count) {

                    model = glm::translate(glm::mat4{ 1.0f }, cubePositions[i]);

                    if (axis != glm::zero<glm::bvec3>())
                        model = glm::rotate(model, glm::radians(angle), static_cast<glm::vec3>(axis));

                    model = glm::scale(model, glm::vec3{ 2.0f, 1.0f, 1.0f });

                    create_cube(*meshes[i], model, true);

                }
                else if (i >= cube_count)
                {

                    model = glm::translate(glm::mat4{ 1.0f }, pointLightPositions[i - cube_count]);

                    model = glm::scale(model, glm::vec3{ 0.2f });

                    create_cube(*meshes[i], model);

                }

            }
            
            if (!pause)
                angle += add_angle * deltaTime;

            fps = static_cast<uint32_t>(static_cast<double>(passed_frames) / glfwGetTime());

            float currentFrame = static_cast<float>(glfwGetTime());
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            std::cout << fps << " : " << deltaTime << " : " << add_angle << " : "<< angle << " : " << axis.x << " , " << axis.y << " , " << axis.z << '\n';

            processInput();
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

            // pass projection matrix to shader (note that in this case it could change every frame)
            glm::mat4 projection{ glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.01f, 100.0f) };
            ub.Uniform("projection[0]", projection[0]);
            ub.Uniform("projection[1]", projection[1]);
            ub.Uniform("projection[2]", projection[2]);
            ub.Uniform("projection[3]", projection[3]);

            // camera/view transformation
            glm::mat4 view{ camera.GetViewMatrix() };
            ub.Uniform("view", view);

            // glBindBuffer(GL_UNIFORM_BUFFER, ube_matrices);
            // glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));

            // glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
            // glBindBuffer(GL_UNIFORM_BUFFER, 0);

            renderer.m_camera_pos = camera.Position;

            light_shader->Uniform("lightColor", { 1.0f, 1.0f, 1.0f });
            light_shader->Uniform("lightPos", pointLightPositions[0]);

            try
            {
                renderer.Render();
            }
            catch (const std::exception& e)
            {
                std::cout << e.what() << '\n';
            }

            // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
            // -------------------------------------------------------------------------------
            glfwSwapBuffers(window);
            glfwPollEvents();
            input.Update();

            ++passed_frames;

        }

        // optional: de-allocate all resources once they've outlived their purpose:
        // ------------------------------------------------------------------------

        // glfw: terminate, clearing all previously allocated GLFW resources.
        // ------------------------------------------------------------------
    }
    catch (const utils::Tilia_Exception& e) {
        std::cout << "\n<<<Tilia_Exception>>>\n";
        std::cout << e.what() << '\n';
    }
    catch (const std::exception& e) {
        std::cout << e.what() << '\n';
    }

    glfwTerminate();

    int w{};
    std::cin >> w;

    return 0;
}

#endif

#if 0

int main() {

    try
    {

        // glfw: initialize and configure
        // ------------------------------
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        // glfw window creation
        // --------------------
        GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
        if (window == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return -1;
        }
        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

        // tell GLFW to capture our mouse
        //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        glfwSwapInterval(0);

        //glad: load all OpenGL function pointers
        //---------------------------------------
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return -1;
        }

        // Shader<false> s{ {"res/shaders/light_shader.vert"}, {"res/shaders/light_shader.frag"}, true };

        // Shader<false> s_2{ {"res/shaders/light_shader.vert"}, {"res/shaders/light_shader.frag"}, true };

        // Shader_Part part{ "res/shaders/light_shader.vert", enums::Shader_Type::Vertex };

        // part.Init();
        // part.Source();
        // part.Compile();

        // std::cout << "ID: " << part.Get_ID() << '\n' << "Path: " << part.Get_Path() << '\n' << "Source: " << part.Get_Source() << '\n' << "Type: " << *part.Get_Type() << '\n';

        // part.Set_Path("res/shaders/light_shader.frag");
        // part.Set_Type(enums::Shader_Type::Fragment);

        // part.Source();
        // part.Compile();

        // std::cout << "ID: " << part.Get_ID() << '\n' << "Path: " << part.Get_Path() << '\n' << "Source: " << part.Get_Source() << '\n' << "Type: " << *part.Get_Type() << '\n';

        std::shared_ptr<Shader_Part> 
        vertex_part{ std::make_shared<Shader_Part>("res/shaders/light_shader.vert", enums::Shader_Type::Vertex) }, 
        fragment_part{ std::make_shared<Shader_Part>("res/shaders/light_shader.frag", enums::Shader_Type::Fragment) };

        vertex_part->Init();
        vertex_part->Source();
        vertex_part->Compile();

        fragment_part->Init();
        fragment_part->Source();
        fragment_part->Compile();

        std::cout << "Vertex\n";

        std::cout << "ID: " << vertex_part->Get_ID() << '\n' << "Path: " << vertex_part->Get_Path() << '\n' << "Source: " << vertex_part->Get_Source() << '\n' << "Type: " << *vertex_part->Get_Type() << '\n';

        std::cout << "\n\nFragment\n";

        std::cout << "ID: " << fragment_part->Get_ID() << '\n' << "Path: " << fragment_part->Get_Path() << '\n' << "Source: " << fragment_part->Get_Source() << '\n' << "Type: " << *fragment_part->Get_Type() << '\n';

        Shader shader{};

        shader.Init({vertex_part}, {fragment_part}, {});

        glm::mat4 projection{ glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.01f, 100.0f) };
        shader.Uniform("projection", projection);

        shader.Uniform("view", glm::mat4{1.0f});

        shader.Uniform("lightColor", { 1.0f, 1.0f, 1.0f });

        while (!glfwWindowShouldClose(window))
        {
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            
            glfwSwapBuffers(window);
            glfwPollEvents();
        }

    }
    catch (const utils::Tilia_Exception& e) {
        std::cout << "\n<<<Tilia_Exception>>>\n";
        std::cout << "Line: " << e.Get_Origin_Line() << '\n' <<
            "File: " << e.Get_Origin_File() << '\n';
        std::cout << e.what() << '\n';
    }
    catch (const std::exception& e) {
        std::cout << e.what() << '\n';
    }
    
    glfwTerminate();

    int w{};

    std::cin >> w;

}

#endif

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput()
{

    if (input.Get_Key_Pressed(KEY_ENTER))
    {
        if (polymode == enums::Polymode::Fill)
            polymode = enums::Polymode::Line;
        else if (polymode == enums::Polymode::Line)
            polymode = enums::Polymode::Fill;
    }

    if (input.Get_Key_Pressed(KEY_END))
    {
        if (print_opengl_things == true)
        {
            print_opengl_things = false;
            logger.Set_Output_Filters(std::cout.rdbuf(), { "default" });
        }
        else 
        {
            print_opengl_things = true;
            logger.Set_Output_Filters(std::cout.rdbuf(), { "debug severity message" });
        }
    }

    if (input.Get_Key_Pressed(KEY_BACKSPACE))
        pause = !pause;

    if (input.Get_Key_Pressed(KEY_HOME))
        reload_shader = true;

    if (input.Get_Key_Down(KEY_UP))
        add_angle += 10.0f * deltaTime;
    if (input.Get_Key_Down(KEY_DOWN))
        add_angle -= 10.0f * deltaTime;

    if (input.Get_Key_Pressed(KEY_1))
        axis.x = !axis.x;
    if (input.Get_Key_Pressed(KEY_2))
        axis.y = !axis.y;
    if (input.Get_Key_Pressed(KEY_3))
        axis.z = !axis.z;

    if (input.Get_Key_Down(KEY_W))
        camera.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
    if (input.Get_Key_Down(KEY_S))
        camera.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
    if (input.Get_Key_Down(KEY_A))
        camera.ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
    if (input.Get_Key_Down(KEY_D))
        camera.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);
    if (input.Get_Key_Down(KEY_SPACE))
        camera.ProcessKeyboard(Camera_Movement::UP, deltaTime);
    if (input.Get_Key_Down(KEY_LEFT_CONTROL))
        camera.ProcessKeyboard(Camera_Movement::DOWN, deltaTime);

    if (input.Get_Key_Pressed(KEY_TAB))
        camera.Reset(true);
    if (input.Get_Key_Pressed(KEY_0))
        camera.Position = { 0.0f, 0.0f, 0.0f };

    camera.ProcessMouseScroll(static_cast<float>(input.Get_Mouse_Scroll()));

    float xpos = static_cast<float>(input.Get_Mouse_Pos().x);
    float ypos = static_cast<float>(input.Get_Mouse_Pos().y);

    if (firstMouse && (input.Get_Mouse_Pos().x + input.Get_Mouse_Pos().y))
    {
        camera.Reset();
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    else if (firstMouse)
    {
        camera.Reset();
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;
    
    if (input.Get_Mouse_Button_Down(0))
        camera.ProcessMouseMovement(xoffset, yoffset);

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glfwMakeContextCurrent(window);
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;

}

template<size_t size>
void create_square(Mesh<size>& mesh, glm::mat4 model, std::array<glm::vec2, 4> uv, uint32_t tex_slot) {

    mesh.vertices.clear();
    mesh.indices.clear();

    glm::vec3 pos_1{ -0.5f, -0.5f, 0.0f };
    glm::vec3 pos_2{ 0.5f, -0.5f, 0.0f };
    glm::vec3 pos_3{ 0.5f,  0.5f, 0.0f };
    glm::vec3 pos_4{ -0.5f,  0.5f, 0.0f };

    pos_1 = glm::vec3{ model * glm::vec4{ pos_1, 1.0f } };
    pos_2 = glm::vec3{ model * glm::vec4{ pos_2, 1.0f } };
    pos_3 = glm::vec3{ model * glm::vec4{ pos_3, 1.0f } };
    pos_4 = glm::vec3{ model * glm::vec4{ pos_4, 1.0f } };

    mesh.vertices.push_back(Vertex<size>{ pos_1.x, pos_1.y, pos_1.z, uv[0].x, uv[0].y, static_cast<float>(tex_slot) });
    mesh.vertices.push_back(Vertex<size>{ pos_2.x, pos_2.y, pos_2.z, uv[1].x, uv[1].y, static_cast<float>(tex_slot) });
    mesh.vertices.push_back(Vertex<size>{ pos_3.x, pos_3.y, pos_3.z, uv[2].x, uv[2].y, static_cast<float>(tex_slot) });
    mesh.vertices.push_back(Vertex<size>{ pos_4.x, pos_4.y, pos_4.z, uv[3].x, uv[3].y, static_cast<float>(tex_slot) });

    mesh.indices.push_back(0);
    mesh.indices.push_back(1);
    mesh.indices.push_back(2);
    mesh.indices.push_back(2);
    mesh.indices.push_back(3);
    mesh.indices.push_back(0);

    Vertex_Info v_info{};

    v_info.sizes = { 3, 2, 1 };
    v_info.strides = { static_cast<int32_t>(size) };
    v_info.offsets = { 0, 3, 5 };

    mesh.Set_Vertex_Info(v_info);

}

template<size_t size>
void create_square(Mesh<size>& mesh, glm::mat4 model)
{

    mesh.vertices.clear();
    mesh.indices.clear();

    glm::vec3 pos_1{ -0.5f, -0.5f, 0.0f };
    glm::vec3 pos_2{ 0.5f, -0.5f, 0.0f };
    glm::vec3 pos_3{ 0.5f,  0.5f, 0.0f };
    glm::vec3 pos_4{ -0.5f,  0.5f, 0.0f };

    pos_1 = glm::vec3{ model * glm::vec4{ pos_1, 1.0f } };
    pos_2 = glm::vec3{ model * glm::vec4{ pos_2, 1.0f } };
    pos_3 = glm::vec3{ model * glm::vec4{ pos_3, 1.0f } };
    pos_4 = glm::vec3{ model * glm::vec4{ pos_4, 1.0f } };

    mesh.vertices.push_back(Vertex<size>{ pos_1.x, pos_1.y, pos_1.z, 0.0f, 0.0f });
    mesh.vertices.push_back(Vertex<size>{ pos_2.x, pos_2.y, pos_2.z, 1.0f, 0.0f });
    mesh.vertices.push_back(Vertex<size>{ pos_3.x, pos_3.y, pos_3.z, 1.0f, 1.0f });
    mesh.vertices.push_back(Vertex<size>{ pos_4.x, pos_4.y, pos_4.z, 0.0f, 1.0f });

    mesh.indices.push_back(0);
    mesh.indices.push_back(1);
    mesh.indices.push_back(2);
    mesh.indices.push_back(2);
    mesh.indices.push_back(3);
    mesh.indices.push_back(0);

    Vertex_Info v_info{};

    v_info.sizes = { 3, 2 };
    v_info.strides = { static_cast<int32_t>(size) };
    v_info.offsets = { 0, 3 };

    mesh.Set_Vertex_Info()(v_info);

}

template<size_t size>
void create_circle(Mesh<size>& mesh, glm::mat4 model, size_t segment_count) {

    mesh.vertices.clear();
    mesh.indices.clear();

    glm::vec3 mid_point{ glm::vec3{ model * glm::vec4{ 0.0f, 0.0f, 0.0f, 0.0f } } };

    mesh.vertices.push_back(Vertex<size>{ mid_point.x, mid_point.y, mid_point.z });

    for (float i = 360.0f; i >= 0.0f; i -= 360.0f / segment_count)
    {

        glm::vec2 vec{ model * glm::vec4{ sinf(glm::radians(i)), cosf(glm::radians(i)), 1.0f, 1.0f } };

        mesh.vertices.push_back(Vertex<size>{ vec.x, vec.y, 0.0f });

    }

    for (size_t i = 1; i <= segment_count; i++)
    {
        mesh.indices.push_back(0);
        mesh.indices.push_back(static_cast<uint32_t>(i));
        if (i != segment_count)
            mesh.indices.push_back(static_cast<uint32_t>(i + 1));
        else
            mesh.indices.push_back(1);
    }

    Vertex_Info v_info{};

    v_info.sizes = { 3 };
    v_info.strides = { static_cast<int32_t>(size) };
    v_info.offsets = { 0 };

    mesh.Set_Vertex_Info(v_info);

}

template<size_t size>
void create_cube(Mesh<size>& mesh, glm::mat4 model, bool complex)
{

    mesh.vertices.clear();
    mesh.indices.clear();
    
    if (complex)
    {
        std::vector<glm::vec3> positions{};
        std::vector<glm::vec3> normals{};
        std::vector<glm::vec3> tex_coords{};

        constexpr size_t vertex_count{ 24 };

        positions.push_back({ -0.5f, -0.5f,  0.5f });
        positions.push_back({ 0.5f, -0.5f,  0.5f });
        positions.push_back({ 0.5f,  0.5f,  0.5f });
        positions.push_back({ -0.5f,  0.5f,  0.5f });

        positions.push_back({ -0.5f, -0.5f, -0.5f });
        positions.push_back({ 0.5f, -0.5f, -0.5f });
        positions.push_back({ 0.5f,  0.5f, -0.5f });
        positions.push_back({ -0.5f,  0.5f, -0.5f });

        positions.push_back({ 0.5f, -0.5f,  0.5f });
        positions.push_back({ 0.5f, -0.5f,  -0.5f });
        positions.push_back({ 0.5f,  0.5f,  -0.5f });
        positions.push_back({ 0.5f,  0.5f,  0.5f });

        positions.push_back({ -0.5f, -0.5f, -0.5f });
        positions.push_back({ -0.5f, -0.5f, 0.5f });
        positions.push_back({ -0.5f,  0.5f, 0.5f });
        positions.push_back({ -0.5f,  0.5f, -0.5f });

        positions.push_back({ -0.5f, 0.5f,  0.5f });
        positions.push_back({ 0.5f, 0.5f,  0.5f });
        positions.push_back({ 0.5f,  0.5f,  -0.5f });
        positions.push_back({ -0.5f,  0.5f,  -0.5f });

        positions.push_back({ 0.5f, -0.5f, -0.5f });
        positions.push_back({ -0.5f, -0.5f, -0.5f });
        positions.push_back({ -0.5f,  -0.5f, 0.5f });
        positions.push_back({ 0.5f,  -0.5f, 0.5f });

        for (size_t i = 0; i < vertex_count; i++)
        {
            tex_coords.push_back(positions[i]);
            positions[i] = glm::vec3{ model * glm::vec4{ positions[i], 1.0f } };
        }

        normals.push_back({ 0.0f, 0.0f, 1.0f });
        normals.push_back({ 0.0f, 0.0f, 1.0f });
        normals.push_back({ 0.0f, 0.0f, 1.0f });
        normals.push_back({ 0.0f, 0.0f, 1.0f });

        normals.push_back({ 0.0f, 0.0f, -1.0f });
        normals.push_back({ 0.0f, 0.0f, -1.0f });
        normals.push_back({ 0.0f, 0.0f, -1.0f });
        normals.push_back({ 0.0f, 0.0f, -1.0f });

        normals.push_back({ 1.0f, 0.0f, 0.0f });
        normals.push_back({ 1.0f, 0.0f, 0.0f });
        normals.push_back({ 1.0f, 0.0f, 0.0f });
        normals.push_back({ 1.0f, 0.0f, 0.0f });

        normals.push_back({ -1.0f, 0.0f, 0.0f });
        normals.push_back({ -1.0f, 0.0f, 0.0f });
        normals.push_back({ -1.0f, 0.0f, 0.0f });
        normals.push_back({ -1.0f, 0.0f, 0.0f });

        normals.push_back({ 0.0f, 1.0f, 0.0f });
        normals.push_back({ 0.0f, 1.0f, 0.0f });
        normals.push_back({ 0.0f, 1.0f, 0.0f });
        normals.push_back({ 0.0f, 1.0f, 0.0f });

        normals.push_back({ 0.0f, -1.0f, 0.0f });
        normals.push_back({ 0.0f, -1.0f, 0.0f });
        normals.push_back({ 0.0f, -1.0f, 0.0f });
        normals.push_back({ 0.0f, -1.0f, 0.0f });

        for (size_t i = 0; i < vertex_count; i++)
        {
            normals[i] = { normals[i] * glm::mat3{glm::transpose(model)} };
        }

        for (size_t i = 0; i < vertex_count; i++)
        {
            mesh.vertices.push_back(Vertex<size>{
                positions[i].x, positions[i].y, positions[i].z,
                    normals[i].x, normals[i].y, normals[i].z,
                    tex_coords[i].x, tex_coords[i].y, tex_coords[i].z
            });
        }

        mesh.indices.push_back(0);
        mesh.indices.push_back(1);
        mesh.indices.push_back(2);
        mesh.indices.push_back(2);
        mesh.indices.push_back(3);
        mesh.indices.push_back(0);

        mesh.indices.push_back(5);
        mesh.indices.push_back(4);
        mesh.indices.push_back(7);
        mesh.indices.push_back(7);
        mesh.indices.push_back(6);
        mesh.indices.push_back(5);

        mesh.indices.push_back(8);
        mesh.indices.push_back(9);
        mesh.indices.push_back(10);
        mesh.indices.push_back(10);
        mesh.indices.push_back(11);
        mesh.indices.push_back(8);

        mesh.indices.push_back(12);
        mesh.indices.push_back(13);
        mesh.indices.push_back(14);
        mesh.indices.push_back(14);
        mesh.indices.push_back(15);
        mesh.indices.push_back(12);

        mesh.indices.push_back(16);
        mesh.indices.push_back(17);
        mesh.indices.push_back(18);
        mesh.indices.push_back(18);
        mesh.indices.push_back(19);
        mesh.indices.push_back(16);

        mesh.indices.push_back(22);
        mesh.indices.push_back(21);
        mesh.indices.push_back(20);
        mesh.indices.push_back(20);
        mesh.indices.push_back(23);
        mesh.indices.push_back(22);

        Vertex_Info v_info{};

        v_info.sizes = { 3, 3, 3 };
        v_info.strides = { static_cast<int32_t>(size) };
        v_info.offsets = { 0, 3, 6 };

        mesh.Set_Vertex_Info()(v_info);

    }
    else
    {
        
        std::vector<glm::vec3> positions{};

        constexpr size_t vertex_count{ 8 };

        positions.push_back({ -0.5f, -0.5f,  0.5f });
        positions.push_back({ 0.5f, -0.5f,  0.5f });
        positions.push_back({ 0.5f,  0.5f,  0.5f });
        positions.push_back({ -0.5f,  0.5f,  0.5f });

        positions.push_back({ -0.5f, -0.5f, -0.5f });
        positions.push_back({ 0.5f, -0.5f, -0.5f });
        positions.push_back({ 0.5f,  0.5f, -0.5f });
        positions.push_back({ -0.5f,  0.5f, -0.5f });

        for (size_t i = 0; i < vertex_count; i++)
        {
            positions[i] = glm::vec3{ model * glm::vec4{ positions[i], 1.0f } };
            mesh.vertices.push_back(Vertex<size>{ positions[i].x, positions[i].y, positions[i].z });
        }

        mesh.indices.push_back(0);
        mesh.indices.push_back(1);
        mesh.indices.push_back(2);
        mesh.indices.push_back(2);
        mesh.indices.push_back(3);
        mesh.indices.push_back(0);

        mesh.indices.push_back(5);
        mesh.indices.push_back(4);
        mesh.indices.push_back(7);
        mesh.indices.push_back(7);
        mesh.indices.push_back(6);
        mesh.indices.push_back(5);

        mesh.indices.push_back(1);
        mesh.indices.push_back(5);
        mesh.indices.push_back(6);
        mesh.indices.push_back(6);
        mesh.indices.push_back(2);
        mesh.indices.push_back(1);

        mesh.indices.push_back(4);
        mesh.indices.push_back(0);
        mesh.indices.push_back(3);
        mesh.indices.push_back(3);
        mesh.indices.push_back(7);
        mesh.indices.push_back(4);

        mesh.indices.push_back(3);
        mesh.indices.push_back(2);
        mesh.indices.push_back(6);
        mesh.indices.push_back(6);
        mesh.indices.push_back(7);
        mesh.indices.push_back(3);

        mesh.indices.push_back(5);
        mesh.indices.push_back(1);
        mesh.indices.push_back(0);
        mesh.indices.push_back(0);
        mesh.indices.push_back(4);
        mesh.indices.push_back(5);

        Vertex_Info v_info{};

        v_info.sizes = { 3 };
        v_info.strides = { static_cast<int32_t>(size) };
        v_info.offsets = { 0 };

        mesh.Set_Vertex_Info()(v_info);

    }

}

//template<size_t size>
//void create_cube(Mesh<size>& mesh, glm::mat4 model, uint32_t tex_slot) {
//
//    mesh.vertices.clear();
//    mesh.indices.clear();
//
//    glm::vec3 pos_1{ -0.5f, -0.5f,  0.5f };
//    glm::vec3 pos_2{  0.5f, -0.5f,  0.5f };
//    glm::vec3 pos_3{  0.5f,  0.5f,  0.5f };
//    glm::vec3 pos_4{ -0.5f,  0.5f,  0.5f };
//
//    glm::vec3 pos_5{ -0.5f, -0.5f, -0.5f };
//    glm::vec3 pos_6{  0.5f, -0.5f, -0.5f };
//    glm::vec3 pos_7{  0.5f,  0.5f, -0.5f };
//    glm::vec3 pos_8{ -0.5f,  0.5f, -0.5f };
//
//    pos_1 = glm::vec3{ model * glm::vec4{ pos_1, 1.0f } };
//    pos_2 = glm::vec3{ model * glm::vec4{ pos_2, 1.0f } };
//    pos_3 = glm::vec3{ model * glm::vec4{ pos_3, 1.0f } };
//    pos_4 = glm::vec3{ model * glm::vec4{ pos_4, 1.0f } };
//    pos_5 = glm::vec3{ model * glm::vec4{ pos_5, 1.0f } };
//    pos_6 = glm::vec3{ model * glm::vec4{ pos_6, 1.0f } };
//    pos_7 = glm::vec3{ model * glm::vec4{ pos_7, 1.0f } };
//    pos_8 = glm::vec3{ model * glm::vec4{ pos_8, 1.0f } };
//
//    mesh.vertices.push_back(Vertex<size>{ pos_1.x, pos_1.y, pos_1.z, -1.0f, -1.0f, 1.0f, static_cast<float>(tex_slot)});
//    mesh.vertices.push_back(Vertex<size>{ pos_2.x, pos_2.y, pos_2.z,  1.0f, -1.0f, 1.0f, static_cast<float>(tex_slot)});
//    mesh.vertices.push_back(Vertex<size>{ pos_3.x, pos_3.y, pos_3.z,  1.0f,  1.0f, 1.0f, static_cast<float>(tex_slot)});
//    mesh.vertices.push_back(Vertex<size>{ pos_4.x, pos_4.y, pos_4.z, -1.0f,  1.0f, 1.0f, static_cast<float>(tex_slot)});
//
//    mesh.vertices.push_back(Vertex<size>{ pos_5.x, pos_5.y, pos_5.z, -1.0f, -1.0f, -1.0f, static_cast<float>(tex_slot)});
//    mesh.vertices.push_back(Vertex<size>{ pos_6.x, pos_6.y, pos_6.z,  1.0f, -1.0f, -1.0f, static_cast<float>(tex_slot)});
//    mesh.vertices.push_back(Vertex<size>{ pos_7.x, pos_7.y, pos_7.z,  1.0f,  1.0f, -1.0f, static_cast<float>(tex_slot)});
//    mesh.vertices.push_back(Vertex<size>{ pos_8.x, pos_8.y, pos_8.z, -1.0f,  1.0f, -1.0f, static_cast<float>(tex_slot)});
//
//    mesh.indices.push_back(0);
//    mesh.indices.push_back(1);
//    mesh.indices.push_back(2);
//    mesh.indices.push_back(2);
//    mesh.indices.push_back(3);
//    mesh.indices.push_back(0);
//
//    mesh.indices.push_back(5);
//    mesh.indices.push_back(4);
//    mesh.indices.push_back(7);
//    mesh.indices.push_back(7);
//    mesh.indices.push_back(6);
//    mesh.indices.push_back(5);
//
//    mesh.indices.push_back(1);
//    mesh.indices.push_back(5);
//    mesh.indices.push_back(6);
//    mesh.indices.push_back(6);
//    mesh.indices.push_back(2);
//    mesh.indices.push_back(1);
//
//    mesh.indices.push_back(4);
//    mesh.indices.push_back(0);
//    mesh.indices.push_back(3);
//    mesh.indices.push_back(3);
//    mesh.indices.push_back(7);
//    mesh.indices.push_back(4);
//
//    mesh.indices.push_back(3);
//    mesh.indices.push_back(2);
//    mesh.indices.push_back(6);
//    mesh.indices.push_back(6);
//    mesh.indices.push_back(7);
//    mesh.indices.push_back(3);
//
//    mesh.indices.push_back(5);
//    mesh.indices.push_back(1);
//    mesh.indices.push_back(0);
//    mesh.indices.push_back(0);
//    mesh.indices.push_back(4);
//    mesh.indices.push_back(5);
//
//    Vertex_Info v_info{};
//
//    v_info.sizes = { 3, 3, 1 };
//    v_info.strides = { static_cast<int32_t>(size) };
//    v_info.offsets = { 0, 3, 6 };
//
//    mesh.Set_Var<Mesh_Var::Vertex_Info>()(v_info);
//
//}

template<size_t size>
void generate_icosadehdron(Mesh<size>& mesh) {

    float t = (1.0f + sqrt(5.0f)) * 0.5f;
    
    // add vertices
    //
    mesh.vertices.push_back(Vertex<size>{-1.0f,  t,     0.0f });
    mesh.vertices.push_back(Vertex<size>{ 1.0f,  t,     0.0f });
    mesh.vertices.push_back(Vertex<size>{-1.0f, -t,     0.0f });
    ////
    mesh.vertices.push_back(Vertex<size>{ 1.0f, -t,     0.0f });
    mesh.vertices.push_back(Vertex<size>{ 0.0f, -1.0f,  t    });
    mesh.vertices.push_back(Vertex<size>{ 0.0f,  1.0f,  t    });
    
    mesh.vertices.push_back(Vertex<size>{ 0.0f, -1.0f, -t    });
    mesh.vertices.push_back(Vertex<size>{ 0.0f,  1.0f, -t    });
    mesh.vertices.push_back(Vertex<size>{ t,     0.0f, -1.0f });
    //
    mesh.vertices.push_back(Vertex<size>{ t,     0.0f,  1.0f });
    mesh.vertices.push_back(Vertex<size>{-t,     0.0f, -1.0f });
    mesh.vertices.push_back(Vertex<size>{-t,     0.0f,  1.0f });
    
    mesh.indices.push_back(0);
    mesh.indices.push_back(11);
    mesh.indices.push_back(5);

    mesh.indices.push_back(0);
    mesh.indices.push_back(5);
    mesh.indices.push_back(1);

    mesh.indices.push_back(0);
    mesh.indices.push_back(1);
    mesh.indices.push_back(7);

    mesh.indices.push_back(0);
    mesh.indices.push_back(7);
    mesh.indices.push_back(10);

    mesh.indices.push_back(0);
    mesh.indices.push_back(10);
    mesh.indices.push_back(11);

    mesh.indices.push_back(1);
    mesh.indices.push_back(5);
    mesh.indices.push_back(9);

    mesh.indices.push_back(5);
    mesh.indices.push_back(11);
    mesh.indices.push_back(4);

    mesh.indices.push_back(11);
    mesh.indices.push_back(10);
    mesh.indices.push_back(2);

    mesh.indices.push_back(10);
    mesh.indices.push_back(7);
    mesh.indices.push_back(6);

    mesh.indices.push_back(7);
    mesh.indices.push_back(1);
    mesh.indices.push_back(8);

    mesh.indices.push_back(3);
    mesh.indices.push_back(9);
    mesh.indices.push_back(4);

    mesh.indices.push_back(3);
    mesh.indices.push_back(4);
    mesh.indices.push_back(2);

    mesh.indices.push_back(3);
    mesh.indices.push_back(2);
    mesh.indices.push_back(6);

    mesh.indices.push_back(3);
    mesh.indices.push_back(6);
    mesh.indices.push_back(8);

    mesh.indices.push_back(3);
    mesh.indices.push_back(8);
    mesh.indices.push_back(9);

    mesh.indices.push_back(4);
    mesh.indices.push_back(9);
    mesh.indices.push_back(5);

    mesh.indices.push_back(2);
    mesh.indices.push_back(4);
    mesh.indices.push_back(11);

    mesh.indices.push_back(6);
    mesh.indices.push_back(2);
    mesh.indices.push_back(10);

    mesh.indices.push_back(8);
    mesh.indices.push_back(6);
    mesh.indices.push_back(7);

    mesh.indices.push_back(9);
    mesh.indices.push_back(8);
    mesh.indices.push_back(1);

}

template<size_t size>
void create_sphere(Mesh<size>& mesh, glm::mat4 model, uint32_t tex_index)
{

    mesh.vertices.clear();
    mesh.indices.clear();

    generate_icosadehdron(mesh);

    size_t vertex_count{ mesh.vertices.size() };
    for (size_t i = 0; i < vertex_count; i++)
    {
        glm::vec4 pos{ mesh.vertices[i].vertices[0], mesh.vertices[i].vertices[1], mesh.vertices[i].vertices[2], 1.0f };
        pos = model * glm::normalize(pos);
        mesh.vertices[i].vertices[3] = mesh.vertices[i].vertices[0];
        mesh.vertices[i].vertices[4] = mesh.vertices[i].vertices[1];
        mesh.vertices[i].vertices[5] = mesh.vertices[i].vertices[2];
        mesh.vertices[i].vertices[0] = pos.x;
        mesh.vertices[i].vertices[1] = pos.y;
        mesh.vertices[i].vertices[2] = pos.z;
        mesh.vertices[i].vertices[6] = static_cast<float>(tex_index);
    }
    
    Vertex_Info v_info{};

    v_info.sizes = { 3, 3, 1 };
    v_info.strides = { static_cast<int32_t>(size) };
    v_info.offsets = { 0, 3, 6 };

    mesh.Set_Vertex_Info(v_info);

}
