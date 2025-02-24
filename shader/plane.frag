#version 460

in vec3 LightDir;
in vec3 ViewDir;
in vec3 Position;
in vec3 Normal;

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
    // Ambient
    vec3 ambient = Light.La * Material.Ka;

    // Diffuse
    vec3 s = normalize(LightDir);
    float sDotN = max(dot(s, n), 0.0);
    vec3 diffuse = Material.Kd * sDotN;

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

void main() {
    FragColor = vec4(blinnphong(normalize(Normal)), 1.0);
}
