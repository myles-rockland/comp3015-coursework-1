#version 460

// In variables
layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec4 VertexTangent;

// Out variables
out vec3 LightDir;
out vec3 ViewDir;
out vec3 Position;
out vec3 Normal;

// Uniforms
uniform struct LightInfo
{
    vec4 Position;
    vec3 La;
    vec3 L;
} Light;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 MVP;

void main()
{
    // Transform normal and tangent to eye space
    Normal = normalize(NormalMatrix * VertexNormal);
    vec3 tangent = normalize(NormalMatrix * vec3(VertexTangent));
    vec3 binormal = normalize(cross(Normal, tangent)) * VertexTangent.w; // Compute binormal
    Position = (ModelViewMatrix * vec4(VertexPosition, 1.0f)).xyz;

    // Set matrix for transformation to tangent space
    mat3 toObjectLocal = mat3(
        tangent.x, binormal.x, Normal.x,
        tangent.y, binormal.y, Normal.y,
        tangent.z, binormal.z, Normal.z
    );

    // Get light direction and view direction in tangent space
    LightDir = toObjectLocal * (Light.Position.xyz - Position);
    ViewDir = toObjectLocal * normalize(-Position);

    // Set gl_Position for next stage in pipeline (fragment shader)
    gl_Position = MVP * vec4(VertexPosition,1.0);
}
