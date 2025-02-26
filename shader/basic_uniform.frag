#version 460

in vec3 LightDir;
in vec3 ViewDir;
in vec2 TexCoord;
in vec3 Position;

layout (location = 0) out vec4 FragColor;

layout (binding = 0) uniform sampler2D DiffuseTexture;
layout (binding = 1) uniform sampler2D NormalTexture;
layout (binding = 2) uniform sampler2D AlphaTexture;

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

uniform struct SpotLightInfo
{
    vec4 Position;
    vec3 La;
    vec3 L;
    vec3 Direction;
    float Exponent;
    float Cutoff;
} Spotlights[3];

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

vec3 blinnphongSpot(vec3 n, int lightIndex)
{
    vec3 ambient = vec3(0), diffuse = vec3(0), specular = vec3(0);

    // Get base texture colour from diffuse texture
    vec3 texColour = texture(DiffuseTexture, TexCoord).rgb;

    // Ambient
    ambient = Spotlights[lightIndex].La * texColour;

    // Diffuse
    vec3 s = normalize(Spotlights[lightIndex].Position.xyz); //vec3 s = normalize(Spot.Position - position);

    float cosAngle = dot(-s, normalize(Spotlights[lightIndex].Direction));
    float angle = acos(cosAngle);
    float spotScale;

    if (angle >= 0.0f && angle < Spotlights[lightIndex].Cutoff)
    {
        spotScale = pow(cosAngle, Spotlights[lightIndex].Exponent);
        float sDotN = max(dot(s, n), 0.0);
        diffuse = texColour * sDotN;

        // Specular
        if (sDotN > 0.0)
        {
            vec3 v = normalize(ViewDir);
            vec3 h = normalize(v + s);
            specular = Material.Ks * pow(max(dot(h,n),0.0), Material.Shininess);
        }
    }

    return ambient + spotScale * Spotlights[lightIndex].L * (diffuse + specular);
}

void main() {
    // Calculate normal direction from normal map texture
    vec3 norm = texture(NormalTexture, TexCoord).xyz;
    norm.xy = 2.0f * norm.xy - 1.0f; // - 1.0f applies to each component in the vector! https://learnwebgl.brown37.net/12_shader_language/glsl_mathematical_operations.html

    vec3 Colour = vec3(0.0f, 0.0f, 0.0f);

    // Discard using alpha map
    vec4 alphaMap = texture(AlphaTexture, TexCoord);

    if (alphaMap.a < 0.15f)
    {
        discard;
    }
    else
    {
        if (gl_FrontFacing)
        {
            for (int i = 0; i < 3; i++)
            {
                Colour += blinnphongSpot(normalize(norm), i);
            }
        }
        else
        {
            for (int i = 0; i < 3; i++)
            {
                Colour += blinnphongSpot(normalize(-norm), i);
            }
        }
    }

    // Apply Colour to FragColor
    FragColor = vec4(Colour, 1.0);
}
