#version 460

in vec3 LightDir;
in vec3 ViewDir;
in vec2 TexCoord;
in vec3 Position;

layout (location = 0) out vec4 FragColor;

layout (binding = 0) uniform sampler2D DiffuseTexture;
layout (binding = 1) uniform sampler2D NormalTexture;

uniform struct LightInfo
{
    vec4 Position;
    vec3 La;
    vec3 L;
} Light;

uniform struct MaterialInfo
{
    vec3 Kd;
    vec3 Ka;
    vec3 Ks;
    float Shininess;
} Material;

uniform struct FogInfo
{
    float MaxDist;
    float MinDist;
    vec3 Colour;
} Fog;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 MVP;

vec3 blinnphong(vec3 n)
{
    // Get base texture colour from diffuse texture
    vec3 texColour = texture(DiffuseTexture, TexCoord).rgb;

    // Ambient
    vec3 ambient = Light.La * texColour;

    // Diffuse
    vec3 s = normalize(LightDir);
    float sDotN = max(dot(s, n), 0.0);
    vec3 diffuse = texColour * sDotN;

    // Specular
    vec3 specular = vec3(0.0f);
    if (sDotN > 0.0)
    {
        vec3 v = normalize(ViewDir);
        vec3 h = normalize(v + s);
        specular = Material.Ks * pow(max(dot(h,n),0.0), Material.Shininess);
    }

    return ambient + (diffuse + specular) * Light.L;
}

vec3 fog(vec3 n, vec3 bp)
{
    float dist = abs(Position.z);
    float fogFactor = (Fog.MaxDist - dist)/(Fog.MaxDist - Fog.MinDist);
    fogFactor = clamp(fogFactor, 0.0, 1.0);
    vec3 colour = mix(Fog.Colour, bp, fogFactor);
    return colour;
}

void main() {
    // Calculate normal direction from normal map texture
    vec3 norm = texture(NormalTexture, TexCoord).xyz;
    norm.xy = 2.0f * norm.xy - 1.0f; // - 1.0f applies to each component in the vector! https://learnwebgl.brown37.net/12_shader_language/glsl_mathematical_operations.html

    // Calculate blinnphong
    vec3 bp = blinnphong(normalize(norm));

    // Calculate fog
    vec3 colour = fog(norm, bp);

    FragColor = vec4(colour, 1.0);
}
