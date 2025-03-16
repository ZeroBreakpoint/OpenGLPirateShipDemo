#pragma once
#include <iostream>
#include <glm/glm.hpp>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include "Texture.h"
#include "Shader.h"

// Forward declaration of ShaderProgram
namespace aie { class ShaderProgram; }

// Represents a single 3D model with multiple submeshes
class Mesh {
public:

    // Structure to hold data for each submesh
    struct SubMesh {
        unsigned int vao = 0;
        unsigned int vbo = 0;
        unsigned int ibo = 0;
        unsigned int indexCount = 0;
        std::string  materialName;  // Material file name
    };

    // Vertex structure for 3D models
    struct Vertex {
        glm::vec4 position;
        glm::vec4 normal;
        glm::vec2 texCoord;
    };

    Mesh(); // Constructor
	virtual ~Mesh(); // Destructor

    // Loads a mesh from a file (supports multiple submeshes)
    bool initialiseFromFile(const char* filename);

    // Loads a material file (.mtl) and its associated textures
    void loadMaterial(const char* fileName);

    // Draws the mesh with the given shader
    void draw(aie::ShaderProgram* shader);

    // Applies a named material from internal texture storage
    void applyMaterial(aie::ShaderProgram* shader, const std::string& textureName) const;


protected:
    // Stores all submeshes of the model
    std::vector<SubMesh> m_subMeshes;

    // Material properties (Phong lighting)
    glm::vec3 Ka; // Ambient reflectance
    glm::vec3 Kd; // Diffuse reflectance
    glm::vec3 Ks; // Specular reflectance
    float specularPower; // Shininess factor

    // Texture storage
    std::map<std::string, aie::Texture> textures; 

};
