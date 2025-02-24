#version 460

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

vec3 blinnphong(vec3 position, vec3 n)
{
    vec3 ambient = vec3(0.0f), diffuse = vec3(0.0f), specular = vec3(0.0f);

    // Ambient
    ambient = Light.La * Material.Ka;

    // Diffuse
    vec3 s = normalize(Light.Position.xyz - position);
    float sDotN = max(dot(s, n), 0.0);
    diffuse = Material.Kd * sDotN;

    // Specular
    if (sDotN > 0.0)
    {
        vec3 v = normalize(-position.xyz);
        vec3 h = normalize(v + s);
        specular = Material.Ks * pow(max(dot(h,n),0.0), Material.Shininess);
    }

    return ambient + (diffuse + specular) * Light.L;
}

void main() {
    FragColor = vec4(blinnphong(Position,normalize(Normal)), 1.0);
}
