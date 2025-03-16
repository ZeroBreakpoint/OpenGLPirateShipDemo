#pragma once
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "imgui_glfw3.h"
#include "../dependencies/glfw/include/GLFW/glfw3.h"

class Camera {
public:
    Camera() : m_theta(-38), m_phi(-12), m_position(glm::vec3(-15, 8, 10)) {}

    // Returns the camera's view matrix
    glm::mat4 getViewMatrix() const {
        float thetaR = glm::radians(m_theta);
        float phiR = glm::radians(m_phi);
        glm::vec3 forward(cos(phiR) * cos(thetaR), sin(phiR), cos(phiR) * sin(thetaR));
        return glm::lookAt(m_position, m_position + forward, glm::vec3(0, 1, 0));
    }

    // Returns projection matrix based on screen size
    glm::mat4 getProjectionMatrix(float width, float height) {
        return glm::perspective(glm::pi<float>() * 0.25f, width / height, 0.1f, 1000.f);
    }

    void update(float deltaTime, GLFWwindow* window);
    glm::vec3 getPosition() const { return m_position; }
private:
    float m_theta; // Camera rotation (horizontal)
    float m_phi; // Camera tilt (vertical)
    glm::vec3 m_position; // Camera world position
};

