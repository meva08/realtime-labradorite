#include <GL/glew.h> 
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "app.h"
#include "utils/math.h"

uint8_t* perlinPipeLine();

int main(int arc, char *argv[])
{
    App mApp = App();

    if (mApp.initProgram() != 0)
    {
        std::cout << "Could not init program." << std::endl;
    }

    // temp quad data
    float vertices[] = {
        // positions          // colors           // texture coords
        0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
    };
    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    // tex coords
    float texCoords[] =
    {
        0.0f, 0.0f,  // lower-left corner  
        1.0f, 0.0f,  // lower-right corner
        0.5f, 1.0f   // top-center corner
    };

    // set up simple VAO
    glGenVertexArrays(1, &mApp.VAO);
    glBindVertexArray(mApp.VAO);

    GLuint VBO = 0;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GLuint EBO = 0;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // existing texture coords
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    GLuint shaderProgram = mApp.makeShaderProgram();
    if (!shaderProgram)
    {
        return 1;
    }
    glUseProgram(shaderProgram);

    uint8_t* pixels = perlinPipeLine();

    // set up texture
    unsigned int texture;
    glGenTextures(1, &texture);

    // bind and fill tex
    glBindTexture(GL_TEXTURE_2D, texture); 
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 800, 600, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glGenerateMipmap(GL_TEXTURE_2D);

    mApp.loop();

    glDeleteProgram(shaderProgram);
    glDeleteVertexArrays(1, &mApp.VAO);
    mApp.closeApp();
    return EXIT_SUCCESS;
}

// hashing function that takes in our int coords and produces a randomly angled vec2
vec2 randomGradient(int ix, int iy)
{
    const unsigned w = 8 * sizeof(unsigned);
    const unsigned s = w / 2; // rotation width
    unsigned a = ix, b = iy;
    a *= 3284157443;
    b ^= a << s | a >> w-s;
    b *= 1911520717;

    a^= b << s | b >> w-s;
    a *= 2048419325;
    float random = a * (3.14159265 / ~(~0u >> 1)); // in [0, 2*Pi]

    vec2 v;
    v.x = std::sin(random);
    v.y = std::cos(random);

    return v;
}

float interpolate(float a0, float a1, float w)
{
    return (a1 - a0) * (3.0 - w * 2.0) * w * w + a0;
}

float dotGridGradient(int ix, int iy, float x, float y)
{
    vec2 gradient = randomGradient(ix, iy);

    // compute the distance vector
    float dx = x - (float) ix;
    float dy = y - (float) iy;

    vec2 distance = {dx, dy};

    return gradient.Dot(distance);
}

float perlin(float x, float y)
{
    // get grid cell corner coords
    int x0 = (int)x;
    int y0 = (int)y;
    int x1 = x0 + 1;
    int y1 = y0 + 1;


    // get interpolation weights
    float sx = x - (float) x0;
    float sy = y - (float) y0;

    // interpolate top two corners
    float n0 = dotGridGradient(x0, y0, x, y);
    float n1 = dotGridGradient(x1, y0, x, y);
    float ix0 = interpolate(n0, n1, sx);

    // interpolate bottom two corners
    n0 = dotGridGradient(x0, y1, x, y);
    n1 = dotGridGradient(x1, y1, x, y);
    float ix1 = interpolate(n0, n1, sx);

    // interpolate between the two previously interpolated values
    float value = interpolate(ix0, ix1, sy);
    return value;
}

uint8_t* perlinPipeLine()
{
    // generate a 2D grid of perlin noise values
    uint8_t* pixels = new uint8_t[800 * 600 * 4];
    int gridSize = 400;
    auto* grid = new float[gridSize * gridSize];



    for (int x = 0; x < 800; x++)
    {
        for (int y = 0; y < 600; y++)
        {
            int index = (y * 800 + x) * 4;

            float val = 0;
            float freq = 1.0;
            float amp = 1.0;

            for (int i = 0; i < 12; i++)
            {
                val += perlin(x * freq / 256, y * freq / 256) * amp;
                freq *= 2.0;
                amp *= 0.5;
            }

            val *= 1.2;

            // clip 
            if (val > 1.0f)
            {
                val = 1.0f;
            }
            else if (val < -1.0f)
            {
                val = -1.0f;
            }

            // convert scale into 255 - 0
            int color = (int)(((val + 1.0f) * 0.5f) * 255);

            // set pixel color
            pixels[index] = color;
            pixels[index + 1] = color;
            pixels[index + 2] = color;
            pixels[index + 3] = 255;
        }
    }

    return pixels;
}