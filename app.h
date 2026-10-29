#pragma once
#include <SDL3/SDL.h>
#include <string>
#include <vector>
#include "mesh.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class App
{
    private:
        const int SCREEN_WIDTH = 800;
        const int SCREEN_HEIGHT = 600;
        SDL_Window *window;
        SDL_GLContext ctx;

    public:
        App();
        bool running; // tells if prog is running
        std::vector<Mesh> mesh;
        GLuint shaderProgram;

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