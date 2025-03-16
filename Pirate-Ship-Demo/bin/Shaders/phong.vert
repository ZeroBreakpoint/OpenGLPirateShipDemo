#version 410

// Vertex attributes from mesh
layout(location = 0) in vec4 Position;
layout(location = 1) in vec4 Normal;
layout(location = 2) in vec2 TexCoords;

// Outputs to fragment shader
out vec4 vPosition; 
out vec3 vNormal;
out vec2 vTexCoords;

// Transformation matrices
uniform mat4 ProjectionViewModel;
uniform mat4 ModelMatrix;

void main() {
    vPosition = ModelMatrix * Position; // Transform vertex position to world space
    vNormal = normalize((ModelMatrix * Normal).xyz); // Convert normal to world space
    vTexCoords = TexCoords; // Pass texture coordinates to fragment shader
    gl_Position = ProjectionViewModel * Position; // Transform to clip space
}

