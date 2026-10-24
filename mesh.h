#pragma once

#include <SDL3/SDL.h>
#include <GL/glew.h> 
#include <string>
#include <vector>
#include <fstream>

struct vertex
{ 
    float x, y, z, r = 1, g = 1, b = 1, u = 0, v = 0;
};

class Mesh
{
    public:
        GLuint shaderProgram;
        std::vector<vertex> vertices;
        std::vector<int> faces;
        GLuint VAO;
        GLuint texture = 0;


        Mesh(std::string filename, GLuint shaderProgram);
        void readOBJ(std::string filename);
        int numVerts();
        int numFaces();
        void render(GLuint shaderProgram);
        void assignTex(uint8_t* textureData, int width, int height);

};