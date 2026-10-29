#include <GL/glew.h> 
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "app.h"
#include "utils/math.h"


uint8_t* perlinPipeLine(float divisor, int octaves, float offset);

int main(int argc, char *argv[])
{
    App mApp = App();

    if (mApp.initProgram() != 0)
    {
        std::cout << "Could not init program." << std::endl;
        return 1;
    }
    else if (argc < 2)
    {
        std::cout << "Please provide a .obj file as an argument." << std::endl;
        return 1;
    }

    GLuint shaderProgram = mApp.makeShaderProgram();
    if (!shaderProgram)
    {
        return 1;
    }
    mApp.shaderProgram = shaderProgram;
    glUseProgram(shaderProgram);
    mApp.mesh.push_back(Mesh(argv[1], shaderProgram));

    // set up MVP matrices
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f)); 

    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

    glm::mat4 projection;
    projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

    // send once for fixed camera
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glEnable(GL_DEPTH_TEST);

    uint8_t* large = perlinPipeLine(350, 1, 0.0f);
    uint8_t* medium = perlinPipeLine(150, 3, 0.0f);
    uint8_t* small = perlinPipeLine(60, 3, 0.0f);

    // make the new texture
    uint8_t* combined = new uint8_t[800 * 600 * 4];
    for (int i = 0; i < 800 * 600; i++)
    {
        combined[i * 4 + 0] = large[i * 4];
        combined[i * 4 + 1] = medium[i * 4];
        combined[i * 4 + 2] = small[i * 4];
        combined[i * 4 + 3] = 255;
    }
    mApp.mesh[0].assignTex(combined, 800, 600);

    mApp.loop();

    glDeleteProgram(shaderProgram);
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

uint8_t* perlinPipeLine(float divisor, int octaves, float offset)
{
    // generate a 2D grid of perlin noise values
    uint8_t* pixels = new uint8_t[800 * 600 * 4];

    for (int x = 0; x < 800; x++)
    {
        for (int y = 0; y < 600; y++)
        {
            int index = (y * 800 + x) * 4;

            float val = 0;
            float freq = 1.0;
            float amp = 1.0;

            for (int i = 0; i < octaves; i++)
            {
                val += perlin(x * freq / divisor + offset, y * freq / divisor + offset) * amp;
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