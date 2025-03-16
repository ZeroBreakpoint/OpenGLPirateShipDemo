#include "Camera.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>


void Camera::update(float deltaTime, GLFWwindow* window) {
    float thetaR = glm::radians(m_theta);
    float phiR = glm::radians(m_phi);

    glm::vec3 forward(cos(phiR) * cos(thetaR), sin(phiR), cos(phiR) * sin(thetaR));
    glm::vec3 right(-sin(thetaR), 0, cos(thetaR));
    glm::vec3 up(0, 1, 0);

    if (glfwGetKey(window, GLFW_KEY_W))
        m_position += forward * deltaTime * 5.0f;
    if (glfwGetKey(window, GLFW_KEY_S))
        m_position -= forward * deltaTime * 5.0f;
    if (glfwGetKey(window, GLFW_KEY_A))
        m_position -= right * deltaTime * 5.0f;
    if (glfwGetKey(window, GLFW_KEY_D))
        m_position += right * deltaTime * 5.0f;
    if (glfwGetKey(window, GLFW_KEY_Z))
        m_position += up * deltaTime * 5.0f;
    if (glfwGetKey(window, GLFW_KEY_X))
        m_position -= up * deltaTime * 5.0f;

    // Mouse look (only when right-click is held)
    static double lastX = 0, lastY = 0;
    double x, y;
    glfwGetCursorPos(window, &x, &y);

    float turnSpeed = 0.1f;
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT)) {
        m_theta += turnSpeed * (float)(x - lastX);
        m_phi -= turnSpeed * (float)(y - lastY);
    }

    lastX = x;
    lastY = y;

    // Clamp camera tilt to prevent flipping
    m_phi = glm::clamp(m_phi, -70.0f, 70.0f);
}


