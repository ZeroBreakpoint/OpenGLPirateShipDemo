#include "Mesh.h"
#include "Shader.h"
#include <assimp/scene.h>
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <vector>
#include <cassert>

Mesh::Mesh()
    : Ka(0.1f), Kd(1.0f), Ks(1.0f), specularPower(32.0f) {
}

Mesh::~Mesh() {

    // Cleanup all submeshes
    for (auto& sub : m_subMeshes) {
        if (sub.vao) glDeleteVertexArrays(1, &sub.vao);
        if (sub.vbo) glDeleteBuffers(1, &sub.vbo);
        if (sub.ibo) glDeleteBuffers(1, &sub.ibo);
    }
}

bool Mesh::initialiseFromFile(const char* filename) {
    // Load model using Assimp
    const aiScene* scene = aiImportFile(filename,
        aiProcess_Triangulate |
        aiProcess_GenNormals |
        aiProcess_FlipUVs);

    // Check if the file loaded correctly
    if (!scene || !scene->HasMeshes()) {
        printf("Error: Failed to load model file %s\n", filename);
        return false;
    }

    // Clear out any existing submeshes
    m_subMeshes.clear();

    // For each aiMesh in the scene, create a SubMesh
    for (unsigned int meshIndex = 0; meshIndex < scene->mNumMeshes; meshIndex++) {
        aiMesh* mesh = scene->mMeshes[meshIndex];
        std::vector<Vertex>    vertices;
        std::vector<unsigned int> indices;

        vertices.reserve(mesh->mNumVertices);
        for (unsigned int v = 0; v < mesh->mNumVertices; v++) {
            Vertex vertex{};
            vertex.position = glm::vec4(
                mesh->mVertices[v].x,
                mesh->mVertices[v].y,
                mesh->mVertices[v].z,
                1.0f
            );

            if (mesh->HasNormals()) {
                vertex.normal = glm::vec4(
                    mesh->mNormals[v].x,
                    mesh->mNormals[v].y,
                    mesh->mNormals[v].z,
                    0.0f
                );
            }
            else {
                vertex.normal = glm::vec4(0, 1, 0, 0);
            }

            // Texture coords
            if (mesh->HasTextureCoords(0)) {
                vertex.texCoord = glm::vec2(
                    mesh->mTextureCoords[0][v].x,
                    1.0f - mesh->mTextureCoords[0][v].y
                );
            }
            else {
                vertex.texCoord = glm::vec2(0, 0);
            }
            vertices.push_back(vertex);
        }

        indices.reserve(static_cast<std::vector<unsigned int, std::allocator<unsigned int>>::size_type>(mesh->mNumFaces) * 3);
        for (unsigned int f = 0; f < mesh->mNumFaces; f++) {
            const aiFace& face = mesh->mFaces[f];
            // Ensure it's a triangle
            if (face.mNumIndices == 3) {
                indices.push_back(face.mIndices[0]);
                indices.push_back(face.mIndices[1]);
                indices.push_back(face.mIndices[2]);
            }
        }

        // Setup OpenGL buffers
        SubMesh subMesh;
        glGenVertexArrays(1, &subMesh.vao);
        glBindVertexArray(subMesh.vao);

        // Vertex buffer
        glGenBuffers(1, &subMesh.vbo);
        glBindBuffer(GL_ARRAY_BUFFER, subMesh.vbo);
        glBufferData(GL_ARRAY_BUFFER,
            vertices.size() * sizeof(Vertex),
            vertices.data(),
            GL_STATIC_DRAW);

   
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(Vertex), (void*)16);

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)32);

        glGenBuffers(1, &subMesh.ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, subMesh.ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
            indices.size() * sizeof(unsigned int),
            indices.data(),
            GL_STATIC_DRAW);

        subMesh.indexCount = (unsigned int)indices.size();

        // Grab the material name from the mesh’s material index
        if (scene->mMaterials && mesh->mMaterialIndex < scene->mNumMaterials) {
            aiMaterial* aiMat = scene->mMaterials[mesh->mMaterialIndex];
            aiString aiMatName;
            // e.g. "mat_0-texture014.jpg"
            if (AI_SUCCESS == aiMat->Get(AI_MATKEY_NAME, aiMatName)) {
                subMesh.materialName = aiMatName.C_Str();
            }
            else {
                subMesh.materialName = "default-grey.jpg";
            }
        }
        else {
            subMesh.materialName = "default-grey.jpg";
        }

        // Unbind
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        // Store this submesh
        m_subMeshes.push_back(subMesh);
    }

    // Done with Assimp data
    aiReleaseImport(scene);

    return true;
}

void Mesh::loadMaterial(const char* fileName) {
    std::fstream file(fileName, std::ios::in);
    if (!file) {
        std::cerr << "Failed to open material file: " << fileName << std::endl;
        return;
    }

    std::string directory(fileName);
    size_t index = directory.find_last_of("/\\");
    directory = (index != std::string::npos) ? directory.substr(0, index + 1) : "";

    std::string line;
    std::string header;
    char buffer[256];

    while (!file.eof()) {
        file.getline(buffer, 256);
        line = buffer;
        std::stringstream ss(line, std::stringstream::in | std::stringstream::out);

        if (line.find("Ka") == 0) ss >> header >> Ka.x >> Ka.y >> Ka.z;
        else if (line.find("Kd") == 0) ss >> header >> Kd.x >> Kd.y >> Kd.z;
        else if (line.find("Ks") == 0) ss >> header >> Ks.x >> Ks.y >> Ks.z;
        else if (line.find("Ns") == 0) ss >> header >> specularPower;
        else if (line.find("map_Kd") == 0) {
            std::string mapFileName;
            ss >> header >> mapFileName;

            std::string texturePath = directory + mapFileName;
            textures[mapFileName] = aie::Texture();

            if (!textures[mapFileName].load(texturePath.c_str())) {
                std::cerr << "Failed to load texture: " << texturePath << std::endl;
            }
        }
    }
}

void Mesh::draw(aie::ShaderProgram* shader) {
    // For each submesh, apply its material & draw
    for (auto& sub : m_subMeshes) {
        applyMaterial(shader, sub.materialName);

        // Bind the VAO
        glBindVertexArray(sub.vao);
        glDrawElements(GL_TRIANGLES, sub.indexCount, GL_UNSIGNED_INT, 0);
    }
    // unbind
    glBindVertexArray(0);
}

void Mesh::applyMaterial(aie::ShaderProgram* shader, const std::string& textureName) const {
    // Set material properties in the shader
    shader->bindUniform("Ka", Ka);
    shader->bindUniform("Kd", Kd);
    shader->bindUniform("Ks", Ks);
    shader->bindUniform("specularPower", specularPower);

    std::string correctedTextureName = textureName; // Store the texture name for potential correction

    // If the texture name starts with "mat_", remove the "mat_#" prefix to extract the actual texture filename
    if (correctedTextureName.rfind("mat_", 0) == 0) {
        size_t dashPos = correctedTextureName.find('-'); // Locate the first dash after "mat_#"
        if (dashPos != std::string::npos) {
            // Remove the "mat_#" prefix.
            correctedTextureName = correctedTextureName.substr(dashPos + 1); // Keep only the texture name after the dash
        }
    }
    // If the texture name starts with "mtl_", it's likely an ocean material
    else if (correctedTextureName.rfind("mtl_", 0) == 0) {
        // Mapthe material to its known texture.
        if (correctedTextureName == "mtl_001")
            correctedTextureName = "./textures/txt_001_diff.png";
    }

    // Attempt to find the corrected texture name in the texture map
    auto it = textures.find(correctedTextureName);
    if (it != textures.end()) {
        glActiveTexture(GL_TEXTURE0); // Activate texture unit 0
        it->second.bind(0); // Bind the found texture to unit 0
        shader->bindUniform("diffuseTex", 0); // Send texture slot to the shader
    }
    else {
        // If the texture is not found, output a warning and use a default texture
        std::cerr << "Warning: Texture not found for material: "
            << textureName << ". Using default-grey.jpg" << std::endl;

        // Try to find a fallback "default-grey.jpg" texture in the map
        auto defaultTex = textures.find("default-grey.jpg");
        if (defaultTex != textures.end()) {
            glActiveTexture(GL_TEXTURE0);
            defaultTex->second.bind(0);
            shader->bindUniform("diffuseTex", 0);
        }
    }
}
