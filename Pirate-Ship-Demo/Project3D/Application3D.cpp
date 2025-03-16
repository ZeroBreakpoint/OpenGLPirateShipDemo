#include "Application3D.h"
#include "Gizmos.h"
#include "Input.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "../dependencies/glfw/include/GLFW/glfw3.h"



using glm::vec3;
using glm::vec4;
using glm::mat4;
using aie::Gizmos;

Application3D::Application3D()
    : m_shipTransform(1.0f),
    m_oceanTransform(1.0f),
    m_light{ glm::vec3(0.0f, 0.0f, 0.0f) },
    m_ambientLight(0.25f, 0.25f, 0.25f),
    m_fillLightDirection(glm::vec3(1.0f, 2.0f, -2.0f)),
    m_fillLightColour(glm::vec3(2.0f, 2.0f, 2.0f)),
    m_fillLightAmbient(glm::vec3(0.5f, 0.5f, 0.5f))
{
}


Application3D::~Application3D() {

}

bool Application3D::startup() {
    if (!glfwInit()) {
        printf("GLFW initialization failed!\n");
        return false;
    }

    if (!gladLoadGL()) {
        printf("Failed to initialize OpenGL loader!\n");
        return false;
    }

    glfwSwapInterval(1);
    setBackgroundColour(0.25f, 0.25f, 0.25f);

    // Initialise rendering tools
    Gizmos::create(10000, 10000, 0, 0);
    aie::ImGui_Init(glfwGetCurrentContext(), true);


    // Load and compile shaders
    m_phongShader.loadShader(aie::eShaderStage::VERTEX, "../bin/Shaders/phong.vert");
    m_phongShader.loadShader(aie::eShaderStage::FRAGMENT, "../bin/Shaders/phong.frag");
    m_phongShader.link();


	// Load the ocean 3D model and material
    m_oceanMesh.initialiseFromFile("../bin/ocean/Ocean.obj");
    m_oceanMesh.loadMaterial("../bin/ocean/Ocean.obj.sxfil.mtl");
    m_oceanTransform = glm::mat4(1.0f);
    m_oceanTransform = glm::translate(m_oceanTransform, glm::vec3(0.0f, -0.5f, 0.0f));
    m_oceanTransform = glm::scale(m_oceanTransform, glm::vec3(20.0f, 15.0f, 20.0f));



    // Load the ship 3D model and material
    m_shipMesh.initialiseFromFile("../bin/pirate_ship/pirate_ship.obj");
    m_shipMesh.loadMaterial("../bin/pirate_ship/pirate_ship.mtl");
    m_shipTransform = glm::mat4(1.0f);
    m_shipTransform = glm::scale(glm::mat4(1.0f), glm::vec3(5.0f));
    m_shipTransform = glm::translate(m_shipTransform, glm::vec3(0.0f, 0.75f, 0.0f));

    // Set up light properties
    m_light.colour = glm::vec3(5.0f, 5.0f, 5.0f);
    m_ambientLight = glm::vec3(0.5f, 0.5f, 0.5f);
    m_light.direction = glm::vec3(2, 0, 2);

    return true;
}



void Application3D::shutdown() {
    aie::ImGui_Shutdown();  // Shutdown ImGui
    aie::Gizmos::destroy(); // Cleanup Gizmos
}

void Application3D::update(float deltaTime) {
    glfwPollEvents();

    aie::ImGui_NewFrame();  // Start new ImGui frame

    ImGuiIO& io = ImGui::GetIO();
    
    m_camera.update(deltaTime, glfwGetCurrentContext());

    // Quit application if Escape key is pressed
    if (aie::Input::getInstance()->isKeyDown(aie::INPUT_KEY_ESCAPE))
        quit();

    // Set ImGui window flags to make it movable, resizable and allow input (not functional due to a legacy version of Imgui provided in bootstrap)
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize;

    ImGui::SetNextWindowSize(ImVec2(430, 165));
    ImGui::Begin("Light Details", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::DragFloat3("Sunlight Direction", &m_light.direction[0], 0.1f, -1.0f, 1.0f);
    ImGui::DragFloat3("Sunlight Colour", &m_light.colour[0], 0.1f, 0.0f, 2.0f);
    ImGui::DragFloat3("Sunlight Ambient", &m_ambientLight[0], 0.1f, 0.0f, 2.0f);

    ImGui::DragFloat3("Fill Light Direction", &m_fillLightDirection[0], 0.1f, -1.0f, 1.0f);
    ImGui::DragFloat3("Fill Light Colour", &m_fillLightColour[0], 0.1f, 0.0f, 2.0f);
    ImGui::DragFloat3("Fill Light Ambient", &m_fillLightAmbient[0], 0.1f, 0.0f, 2.0f);
    ImGui::End();


    ImGui::Render();
}



void Application3D::draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    Gizmos::clear();
    Gizmos::addTransform(glm::mat4(1));
    glm::mat4 pv = m_camera.getProjectionMatrix(static_cast<float>(getWindowWidth()), static_cast<float>(getWindowHeight())) * m_camera.getViewMatrix();
    Gizmos::draw(pv);

    // Bind Phong shader
    m_phongShader.bind();
    m_phongShader.bindUniform("tilingFactor", 1.0f);
    m_phongShader.bindUniform("LightDirection", m_light.direction);
    m_phongShader.bindUniform("LightColour", m_light.colour);
    m_phongShader.bindUniform("AmbientColour", m_ambientLight);
    m_phongShader.bindUniform("cameraPosition", m_camera.getPosition());

    m_phongShader.bindUniform("FillLightColour", m_fillLightColour);
    glm::vec3 dir = glm::normalize(m_fillLightDirection);
    m_phongShader.bindUniform("FillLightDirection", m_fillLightDirection);
    m_phongShader.bindUniform("FillLightAmbient", m_fillLightAmbient);

   
    // Draw ship
    glm::mat4 pvm = pv * m_shipTransform;
    m_phongShader.bindUniform("ProjectionViewModel", pvm);
    m_phongShader.bindUniform("ModelMatrix", m_shipTransform);

    m_shipMesh.draw(&m_phongShader);


    // Draw ocean
    m_phongShader.bindUniform("tilingFactor", 5.0f);
    glm::mat4 oceanPVM = pv * m_oceanTransform;
    m_phongShader.bindUniform("ProjectionViewModel", oceanPVM);
    m_phongShader.bindUniform("ModelMatrix", m_oceanTransform);
    m_oceanMesh.draw(&m_phongShader);


    // Render ImGui
    ImGui::Render();
    glEnable(GL_CULL_FACE);
    glfwSwapBuffers(glfwGetCurrentContext());
    glfwPollEvents();
   
}





