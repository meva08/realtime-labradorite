#include "mesh.h"

Mesh::Mesh(std::string filename, GLuint shaderProgram)
{
    // set shader program
    this->shaderProgram = shaderProgram;
    this->readOBJ(filename);
}


void Mesh::readOBJ(std::string filename)
{
    std::ifstream file(filename);

    std::vector<float> uvs;
    std::string line;

    while(std::getline(file, line))
    {
        char trash;

        if (line.starts_with("v "))
        {
            vertex curr_v;
            std::sscanf(line.c_str(), "v %f %f %f %f %f %f", &curr_v.x, &curr_v.y, &curr_v.z, &curr_v.r, &curr_v.g, &curr_v.b);
            vertices.push_back(curr_v);
        }
        else if (line.starts_with("vt "))
        {
            float u, v;
            std::sscanf(line.c_str(), "vt %f %f", &u, &v);
            uvs.push_back(u);
            uvs.push_back(v);
        }
        else if (line.starts_with("f "))
        {
            int v1, v2, v3;
            if (std::sscanf(line.c_str(), "f %i/%*i/%*i %i/%*i/%*i %i/%*i/%*i", &v1, &v2, &v3) == 3)
            {
                faces.push_back(v1 - 1);
                faces.push_back(v2 - 1);
                faces.push_back(v3 - 1);
            }
        }
    }

    // attach uvs to vertices
    for (size_t i = 0; i < vertices.size() && i * 2 + 1 < uvs.size(); i++)
    {
        vertices[i].u = uvs[i * 2];
        vertices[i].v = uvs[i * 2 + 1];
    }

    // set up vao, vbo and ebo
    glGenVertexArrays(1, &this->VAO);
    glBindVertexArray(this->VAO);

    GLuint VBO = 0;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    GLuint EBO = 0;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * faces.size(), faces.data(), GL_STATIC_DRAW);

    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // existing texture coords
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    printf("read in mesh. vertices: %i faces: %i\n", vertices.size(), faces.size());
}

int Mesh::numVerts()
{
    return this->vertices.size();
}

int Mesh::numFaces()
{
    return this->faces.size() / 3;
}

void Mesh::render(GLuint shaderProgram)
{
    glUseProgram(shaderProgram);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, faces.size(), GL_UNSIGNED_INT, 0);
}

void Mesh::assignTex(uint8_t* textureData, int width, int height)
{
    // set up texture
    glGenTextures(1, &texture);

    // bind and fill tex
    glBindTexture(GL_TEXTURE_2D, texture); 
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);
    glGenerateMipmap(GL_TEXTURE_2D);
}