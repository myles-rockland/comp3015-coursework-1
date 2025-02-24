#include "scenebasic_uniform.h"

#include <cstdio>
#include <cstdlib>

#include <string>
using std::string;

#include <sstream>
#include <iostream>
using std::cerr;
using std::endl;

#include "helper/glutils.h"
#include "helper/texture.h"

using namespace glm;

SceneBasic_Uniform::SceneBasic_Uniform() : 
    plane(100.0f, 100.0f, 1, 1),
    tPrev(0),
    angle(0.0f),
    rotSpeed(pi<float>()/8.0f)
{
    gun = ObjMesh::load("media/38-special-revolver/source/rev_anim.obj.obj", false, true);
}

void SceneBasic_Uniform::initScene()
{
    compile();

    glEnable(GL_DEPTH_TEST);

    // Set MVP matrices
    model = glm::mat4(1.0f);
    view = glm::lookAt(vec3(1.0f, 1.25f, 1.25f), vec3(0.0f, 0.2f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    projection = glm::mat4(1.0f);

    // Set light uniforms for gun shader programs
    gunProg.setUniform("Light.L", vec3(1.0f)); // Diffuse + Specular
    gunProg.setUniform("Light.La", vec3(0.5f)); // Ambient

    // Set light uniforms for plane shader programs
    planeProg.use();
    planeProg.setUniform("Light.L", vec3(1.0f)); // Diffuse + Specular
    planeProg.setUniform("Light.La", vec3(0.5f)); // Ambient

    // Set fog uniforms
    gunProg.use();
    gunProg.setUniform("Fog.MaxDist", 10.0f);
    gunProg.setUniform("Fog.MinDist", 1.0f);
    gunProg.setUniform("Fog.Colour", vec3(0.5f));

    // Load textures first
    GLuint diffuseTexture = Texture::loadTexture("media/38-special-revolver/textures/rev_d.tga.png");
    GLuint normalTexture = Texture::loadTexture("media/38-special-revolver/textures/rev_n.tga.png");

    // Load skybox texture
    GLuint skyboxTexture = Texture::loadHdrCubeMap("media/space_skybox/space");

    // Set active texture unit and bind loaded texture ids to texture buffers
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normalTexture);
    
    // Set texture unit to 0 and bind cubemap
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
}

void SceneBasic_Uniform::compile()
{
	try {
        // Compile Shaders
        // Gun shader
        gunProg.compileShader("shader/basic_uniform.vert");
        gunProg.compileShader("shader/basic_uniform.frag");
        // Skybox shader
        skyboxProg.compileShader("shader/skybox.vert");
        skyboxProg.compileShader("shader/skybox.frag");
        // Plane shader
        planeProg.compileShader("shader/plane.vert");
        planeProg.compileShader("shader/plane.frag");

        // Link Shaders
        gunProg.link();
        skyboxProg.link();
        planeProg.link();

        // Use gun shader to begin
        gunProg.use();
	} catch (GLSLProgramException &e) {
		cerr << e.what() << endl;
		exit(EXIT_FAILURE);
	}
}

void SceneBasic_Uniform::update( float t )
{
    float deltaT = t - tPrev;
    if (tPrev == 0.0f)
    {
        deltaT = 0.0f;
    }
    tPrev = t;
    
    if (this->m_animate)
    {
        angle += rotSpeed * deltaT;
        if (angle >= two_pi<float>())
        {
            angle -= two_pi<float>();
        }
    }
}

void SceneBasic_Uniform::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set camera position and view matrix
    vec3 cameraPos = vec3(15.0f * sin(angle), 3.0f, 15.0f * cos(angle));
    view = glm::lookAt(cameraPos, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

    // Skybox rendering
    skyboxProg.use();

    model = mat4(1.0f);

    setMatrices(skyboxProg);
    skybox.render();

    // Gun rendering
    gunProg.use();

    // Set light uniforms
    gunProg.setUniform("Light.Position", view * vec4(100.0f * cos(angle), 0.0f, 100.0f * sin(angle), 1.0f));

    // Set material uniforms
    gunProg.setUniform("Material.Kd", vec3(0.2f, 0.55f, 0.9f));
    gunProg.setUniform("Material.Ks", vec3(0.95f, 0.95f, 0.95f));
    gunProg.setUniform("Material.Ka", vec3(0.2f * 0.3f, 0.55f * 0.3f, 0.9f * 0.3f));
    gunProg.setUniform("Material.Shininess", 100.0f);

    // Set gun model matrix
    model = mat4(1.0f);
    model = translate(model, vec3(0.0f, 0.0f, -5.0f));
    model = rotate(model, radians(180.0f), vec3(0.0f, 1.0f, 0.0f));
    model = rotate(model, radians(-90.0f), vec3(0.0f, 0.0f, 1.0f));
    model = scale(model, vec3(0.1f));

    // Set MVP matrix uniforms and render gun
    setMatrices(gunProg);
    gun->render();

    // Plane rendering
    planeProg.use();

    // Set light uniforms
    planeProg.setUniform("Light.Position", view * vec4(100.0f * cos(angle), 0.0f, 100.0f * sin(angle), 1.0f));

    // Set plane material uniforms
    planeProg.setUniform("Material.Kd", vec3(0.5f));
    planeProg.setUniform("Material.Ks", vec3(0.0f));
    planeProg.setUniform("Material.Ka", vec3(1.0f));
    planeProg.setUniform("Material.Shininess", 1.0f);

    // Set plane model matrix
    model = mat4(1.0f);
    model = translate(model, vec3(0.0f, -10.0f, 0.0f));

    // Set MVP matrix uniforms and render plane
    setMatrices(planeProg);
    plane.render();
}

void SceneBasic_Uniform::resize(int w, int h)
{
    glViewport(0, 0, w, h);
    width = w;
    height = h;
    projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 100.0f);
}

void SceneBasic_Uniform::setMatrices(GLSLProgram& p)
{
    glm::mat4 mv = view * model;
    p.setUniform("ModelMatrix", model);
    p.setUniform("ModelViewMatrix", mv);
    p.setUniform("MVP", projection * mv);
    p.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
}