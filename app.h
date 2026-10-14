#pragma once
#include <SDL3/SDL.h>
#include <string>

class App
{
    private:
        const int SCREEN_WIDTH = 800;
        const int SCREEN_HEIGHT = 600;
        SDL_Window *window;
        SDL_GLContext ctx;
        GLuint shaderProgram;

    public:
        App();
        bool running; // tells if prog is running
        GLuint VAO;

        int initProgram();
        void closeApp();
        GLuint makeShaderProgram();
        void loop();
        void Input();
        void Update();
        void Render();
        std::string readFile(const std::string& path);
        GLuint compileShader(GLenum type, const std::string& src);

};