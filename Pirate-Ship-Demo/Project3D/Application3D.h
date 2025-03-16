#pragma once
#include <iostream>
#include "glad.h"
#include "Application.h"
#include <glm/mat4x4.hpp>
#include "Shader.h"
#include "Mesh.h"
#include "Camera.h"
#include "imgui_glfw3.h"

class Application3D : public aie::Application {
public:
    Application3D();
    virtual ~Application3D();
    virtual bool startup();
    virtual void shutdown();
    virtual void update(float deltaTime);
    virtual void draw();
 
protected:
        Camera m_camera; // Scene camera
        aie::ShaderProgram m_shader; // Basic shader program
        aie::ShaderProgram m_phongShader; // Phong shading program
        Mesh m_shipMesh;   // Mesh for the pirate ship
        glm::mat4 m_shipTransform; // Transform for ship positioning
        Mesh m_oceanMesh;  // Mesh for the ocean
        glm::mat4 m_oceanTransform; // Transform for ocean positioning

        struct Light {
            glm::vec3 direction;
            glm::vec3 colour;
        };

        glm::vec3 m_ambientLight; // Ambient lighting in scene
        Light m_light; // Primary light (sun)
        glm::vec3 m_fillLightDirection; // Secondary light (fill light)
        glm::vec3 m_fillLightColour; // Fill light colour
		glm::vec3 m_fillLightAmbient; // Fill light ambient

};
