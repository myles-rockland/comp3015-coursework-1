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
    tPrev(0),
    angle(0.0f),
    rotSpeed(pi<float>()/8.0f)
{
    revolver = ObjMesh::load("media/38-special-revolver/source/rev_anim.obj.obj", false, true);
}

void SceneBasic_Uniform::initScene()
{
    compile();

    glEnable(GL_DEPTH_TEST);

    // Set MVP matrices
    model = glm::mat4(1.0f);
    view = glm::lookAt(vec3(1.0f, 1.25f, 1.25f), vec3(0.0f, 0.2f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    projection = glm::mat4(1.0f);

    // Set light uniforms
    prog.setUniform("Light.L", vec3(1.0f)); // Diffuse + Specular
    prog.setUniform("Light.La", vec3(0.05f)); // Ambient

    // Set fog uniforms
    prog.setUniform("Fog.MaxDist", 50.0f);
    prog.setUniform("Fog.MinDist", 1.0f);
    prog.setUniform("Fog.Colour", vec3(0.5f));

    // Load textures first
    GLuint diffuseTexture = Texture::loadTexture("media/38-special-revolver/textures/rev_d.tga.png");
    GLuint normalTexture = Texture::loadTexture("media/38-special-revolver/textures/rev_n.tga.png");

    // Set active texture unit and bind loaded texture ids to texture buffers
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normalTexture);
}

void SceneBasic_Uniform::compile()
{
	try {
		prog.compileShader("shader/basic_uniform.vert");
		prog.compileShader("shader/basic_uniform.frag");
		prog.link();
		prog.use();
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
    angle += 0.1f * deltaT;
    
    if (this->m_animate)
    {
        angle += rotSpeed * deltaT;
        if (angle > two_pi<float>())
        {
            angle -= two_pi<float>();
        }
    }
}

void SceneBasic_Uniform::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    vec3 cameraPos = vec3(-10.0f, 5.0f, 10.0f);
    view = glm::lookAt(cameraPos, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    prog.setUniform("Light.Position", view * vec4(100.0f * cos(angle), 0.0f, 100.0f * sin(angle), 1.0f));

    prog.setUniform("Material.Kd", vec3(0.2f, 0.55f, 0.9f));
    prog.setUniform("Material.Ks", vec3(0.95f, 0.95f, 0.95f));
    prog.setUniform("Material.Ka", vec3(0.2f * 0.3f, 0.55f * 0.3f, 0.9f * 0.3f));
    prog.setUniform("Material.Shininess", 100.0f);

    model = mat4(1.0f);
    model = translate(model, vec3(0.0f, 0.0f, -5.0f));
    model = rotate(model, radians(180.0f), vec3(0.0f, 1.0f, 0.0f));
    model = rotate(model, radians(-90.0f), vec3(0.0f, 0.0f, 1.0f));
    model = scale(model, vec3(0.1f));

    setMatrices();
    revolver->render();
}

void SceneBasic_Uniform::resize(int w, int h)
{
    glViewport(0, 0, w, h);
    width = w;
    height = h;
    projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 100.0f);
}

void SceneBasic_Uniform::setMatrices()
{
    glm::mat4 mv = view * model;
    prog.setUniform("ModelViewMatrix", mv);
    prog.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    prog.setUniform("MVP", projection * mv);
}