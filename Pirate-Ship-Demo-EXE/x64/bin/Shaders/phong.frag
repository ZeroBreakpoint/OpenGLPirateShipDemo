#version 410

// Inputs from vertex shader
in vec4 vPosition;
in vec3 vNormal;
in vec2 vTexCoords;

// Camera & Light Data
uniform vec3 cameraPosition;
uniform vec3 AmbientColour;      // Sun ambient light
uniform vec3 FillLightAmbient;   // Fill light ambient
uniform vec3 LightColour;        // Sun (primary) light colour
uniform vec3 LightDirection;     // Sun (primary) light direction
uniform vec3 FillLightColour;    // Fill (secondary) light colour
uniform vec3 FillLightDirection; // Fill (secondary) light direction

// Material properties
uniform vec3 Ka; // Ambient reflectance
uniform vec3 Kd; // Diffuse reflectance
uniform vec3 Ks; // Specular reflectance
uniform float specularPower; // Shininess

// Texture Sampling
uniform sampler2D diffuseTex; // Diffuse texture map
uniform float tilingFactor; // Texture scaling

out vec4 FragColour; // Output final pixel colour

void main() {
    vec3 N = normalize(vNormal);

    // Sample texture with tiling
    vec3 textureColour = texture(diffuseTex, vTexCoords * tilingFactor).rgb;
    
    // View direction
    vec3 V = normalize(cameraPosition - vPosition.xyz);
    
    // ---- Sun (Primary Light)  ----
    vec3 L1 = normalize(LightDirection);
    float lambertTerm1 = max(0.0, dot(N, -L1));
    vec3 R1 = reflect(L1, N);
    float specularTerm1 = pow(max(0.0, dot(R1, V)), specularPower);
    
    // ---- Fill Light (Secondary Light) ----
    vec3 L2 = normalize(FillLightDirection);
    float lambertTerm2 = max(0.0, dot(N, -L2));
    vec3 R2 = reflect(L2, N);
    float specularTerm2 = pow(max(0.0, dot(R2, V)), specularPower);
    
    // Combine lighting effects
    vec3 ambient = (AmbientColour + FillLightAmbient) * Ka * textureColour;
    
    // Diffuse and specular contributions
    vec3 diffuse1 = LightColour * Kd * lambertTerm1 * textureColour;
    vec3 specular1 = LightColour * Ks * specularTerm1;
    vec3 diffuse2 = FillLightColour * Kd * lambertTerm2 * textureColour;
    vec3 specular2 = FillLightColour * Ks * specularTerm2;
    
    vec3 finalColour = ambient + diffuse1 + specular1 + diffuse2 + specular2;
    FragColour = vec4(finalColour, 1.0);
}



