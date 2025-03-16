#pragma once
#include "glad.h" 
#include <memory>       
#include <cassert>      
#include <glm/glm.hpp>  
      

namespace aie {

    // Enum representing different shader stages (types of shaders)
    enum eShaderStage {
        VERTEX = 1,                  // Vertex Shader
        TESSELLATION_EVALUATION,     // (Unused, but reserved for tessellation)
        TESSELLATION_CONTROL,        // (Unused, but reserved for tessellation)
        GEOMETRY,                    // Geometry Shader
        FRAGMENT,                    // Fragment (Pixel) Shader
        SHADER_STAGE_Count           // Count of shader stages (internal use)
    };

    class Shader {
    public:
        // Destructor
        ~Shader();

        // Loads a shader from a file and compiles it
        bool loadShader(unsigned int stage, const char* filename);

        // Creates a shader from source code string and compiles it
        bool createShader(unsigned int stage, const char* string);

        // Returns the OpenGL shader handle
        unsigned int getHandle() const { return m_handle; }

        // Returns the shader stage
        unsigned int getStage() const { return m_stage; }

    private:
        // OpenGL shader handle
        unsigned int m_handle{ 0 };
        // Stores the shader stage type
        unsigned int m_stage{ 0 };
        // Stores the last error message (if any)
        char* m_lastError{ nullptr };
    };

    class ShaderProgram {
    public:
        // Destructor
        ~ShaderProgram();

        // Loads and compiles a shader from file
        bool loadShader(unsigned int stage, const char* filename);

        // Creates a shader from source code string
        bool createShader(unsigned int stage, const char* string);

        // Attaches an existing shader to this program
        void attachShader(const std::shared_ptr<Shader>& shader);

        // Links all attached shaders into a complete program
        bool link();

        // Activates this shader program for rendering
        void bind() const;

        // Retrieves the location of a uniform variable in the shader
        int getUniform(const char* name) const;

        // Uniform binding functions (set variables inside the shader)
        bool bindUniform(const char* name, int value) const;
        bool bindUniform(const char* name, float value) const;
        bool bindUniform(const char* name, const glm::vec2& value) const;
        bool bindUniform(const char* name, const glm::vec3& value) const;
        bool bindUniform(const char* name, const glm::vec4& value) const;
        bool bindUniform(const char* name, const glm::mat2& value) const;
        bool bindUniform(const char* name, const glm::mat3& value) const;
        bool bindUniform(const char* name, const glm::mat4& value) const;

        // Bind uniforms for array values
        bool bindUniform(const char* name, int count, int* value) const;
        bool bindUniform(const char* name, int count, float* value) const;
        bool bindUniform(const char* name, int count, const glm::vec2* value) const;
        bool bindUniform(const char* name, int count, const glm::vec3* value) const;
        bool bindUniform(const char* name, int count, const glm::vec4* value) const;
        bool bindUniform(const char* name, int count, const glm::mat2* value) const;
        bool bindUniform(const char* name, int count, const glm::mat3* value) const;
        bool bindUniform(const char* name, int count, const glm::mat4* value) const;

        
        void bindUniform(int ID, int value) const;
        void bindUniform(int ID, float value) const;
        void bindUniform(int ID, const glm::vec2& value) const;
        void bindUniform(int ID, const glm::vec3& value) const;
        void bindUniform(int ID, const glm::vec4& value) const;
        void bindUniform(int ID, const glm::mat2& value) const;
        void bindUniform(int ID, const glm::mat3& value) const;
        void bindUniform(int ID, const glm::mat4& value) const;

        void bindUniform(int ID, int count, int* value) const;
        void bindUniform(int ID, int count, float* value) const;
        void bindUniform(int ID, int count, const glm::vec2* value) const;
        void bindUniform(int ID, int count, const glm::vec3* value) const;
        void bindUniform(int ID, int count, const glm::vec4* value) const;
        void bindUniform(int ID, int count, const glm::mat2* value) const;
        void bindUniform(int ID, int count, const glm::mat3* value) const;
        void bindUniform(int ID, int count, const glm::mat4* value) const;

    private:
        // OpenGL program handle
        unsigned int m_program{ 0 };

        // Shader storage
        std::shared_ptr<Shader> m_shaders[SHADER_STAGE_Count];

        // Stores the last linking error (if any)
        char* m_lastError{ nullptr };
    };

}
