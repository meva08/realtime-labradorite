#include <GL/glew.h>
#include "app.h"
#include <iostream>
#include <SDL3/SDL.h>
#include <string>
#include <fstream>
#include <sstream>

    App::App()
    {
        running = false; // tells if prog is running
        window = nullptr;
        ctx = nullptr;
        shaderProgram = 0;
        VAO = 0;
    }
    void App::loop()
    {
        // main loop
        while (running)
        {
            Input();
            Update();
            Render();
        }
    }

    int App::initProgram()
    {
        SDL_Init(SDL_INIT_VIDEO);

        // request the OpenGL 4.3 context
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);


        window = SDL_CreateWindow("Hello SDL World", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
        if (NULL == window)
        {
            std::cout << "Could not create window: " << SDL_GetError( ) << std::endl;
            return 1;
        }

        ctx = SDL_GL_CreateContext(window);
        if (!ctx)
        {
            std::cout << "Context failed: " << SDL_GetError() << std::endl;
            return 1;
        }

        // Initialize GLEW
        glewExperimental = GL_TRUE;
        GLenum err = glewInit();
        if (err != GLEW_OK) {
            std::cout << "glewInit failed: " << glewGetErrorString(err) << std::endl;
            return 1;
        }
        glGetError();
        SDL_GL_SetSwapInterval(1);

        std::cout << "GL version: " << glGetString(GL_VERSION) << std::endl;
        std::cout << "Renderer:   " << glGetString(GL_RENDERER) << std::endl;

        running = true;
        return 0;
    }

    void App::closeApp()
    {
        SDL_GL_DestroyContext(ctx);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    void App::Input()
    {
        SDL_Event windowEvent;
        if (SDL_PollEvent(&windowEvent))
        {
            if (SDL_EVENT_QUIT == windowEvent.type)
            {
                running = false;
            }

            if (windowEvent.type == SDL_EVENT_KEY_DOWN && windowEvent.key.key == SDLK_ESCAPE)
            {
                running = false;
            }
        }
    }
    void App::Update()
    {
    }

    void App::Render()
    {
        int w, h;
        SDL_GetWindowSizeInPixels(window, &w, &h);
        glViewport(0, 0, w, h);


        glClearColor(0.1f, 0.2f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        SDL_GL_SwapWindow(window);
    }

    std::string App::readFile(const std::string& path)
    {
        std::ifstream f(path, std::ios::binary);
        if (!f)
        {
            std::cout << "Could not open shader file: " << path << std::endl;
            return "";
        }
        std::stringstream ss;
        ss << f.rdbuf();
        return ss.str();
    }

    GLuint App::compileShader(GLenum type, const std::string& src)
    {
        GLuint s = glCreateShader(type);
        const char* ptr = src.c_str();
        glShaderSource(s, 1, &ptr, nullptr);
        glCompileShader(s);

        GLint ok = 0;
        glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
        if (!ok)
        {
            char log[2048];
            glGetShaderInfoLog(s, sizeof(log), nullptr, log);
            std::cout << "Shader compile error:\n" << log << std::endl;
            return 0;
        }
        return s;
    }


    GLuint App::makeShaderProgram()
    {
        std::string vertSrc = readFile("./shaders/raytracer.vert");
        std::string fragSrc = readFile("./shaders/raytracer.frag");

        if (vertSrc.empty() || fragSrc.empty()) return 0;

        GLuint vert = compileShader(GL_VERTEX_SHADER, vertSrc);
        GLuint frag = compileShader(GL_FRAGMENT_SHADER, fragSrc);
        if (!vert || !frag) return 0;

        GLuint prog = glCreateProgram();
        glAttachShader(prog, vert);
        glAttachShader(prog, frag);
        glLinkProgram(prog);

        GLint ok = 0;
        glGetProgramiv(prog, GL_LINK_STATUS, &ok);
        if (!ok)
        {
            char log[2048];
            glGetProgramInfoLog(prog, sizeof(log), nullptr, log);
            std::cout << "Program link error:\n" << log << std::endl;
            return 0;
        }

        glDeleteShader(vert);
        glDeleteShader(frag);
        return prog;
    }