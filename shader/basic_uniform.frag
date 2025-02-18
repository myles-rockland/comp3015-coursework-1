#version 460

in vec3 LightDir;
in vec3 ViewDir;
in vec2 TexCoord;

layout (binding = 0) uniform sampler2D ColourTex;
layout (binding = 1) uniform sampler2D NormalMapTex;
layout (location = 0) out vec4 FragColor;

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

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 MVP;

vec3 blinnphong(vec3 n)
{
    vec3 diffuse = vec3(0), specular = vec3(0);

    vec3 texColour = texture(ColourTex, TexCoord).rgb;

    // Ambient
    vec3 ambient = Light.La * texColour;

    // Diffuse
    vec3 s = normalize(LightDir);
    float sDotN = max(dot(s, n), 0.0);
    diffuse = texColour * sDotN;

    // Specular
    if (sDotN > 0.0)
    {
        vec3 v = normalize(ViewDir);
        vec3 h = normalize(v + s);
        specular = Material.Ks * pow(max(dot(h,n),0.0), Material.Shininess);
    }

    return ambient + (diffuse + specular) * Light.L;
}

void main() {
    vec3 norm = texture(NormalMapTex, TexCoord).xyz;
    norm.xy = 2.0f * norm.xy - 1.0f;

    FragColor = vec4(blinnphong(normalize(norm)), 1.0);
}
