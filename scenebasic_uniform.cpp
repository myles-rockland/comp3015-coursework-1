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

#include "GLFW/glfw3.h"
#include "glad/glad.h"

using namespace glm;

SceneBasic_Uniform::SceneBasic_Uniform() :
    plane(100.0f, 100.0f, 1, 1),
    tPrev(0),
    angle(0.0f),
    rotSpeed(pi<float>() / 8.0f),
    cameraPosition(0.0f, 0.0f, 10.0f),
    cameraForward(0.0f, 0.0f, 1.0f),
    cameraUp(0.0f, 1.0f, 0.0f),
    cameraYaw(-90.0f),
    cameraPitch(0.0f),
    cameraSpeed(5.0f),
    cameraSensitivity(0.025f),
    mouseFirstEntry(true),
    lastXPos(width / 2.0f),
    lastYPos(height / 2.0f)
{
    gun = ObjMesh::load("media/38-special-revolver/source/rev_anim.obj.obj", false, true);
}

void SceneBasic_Uniform::initScene()
{
    compile();

    // Hide the cursor
    GLFWwindow* windowContext = glfwGetCurrentContext();
    glfwSetInputMode(windowContext, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glEnable(GL_DEPTH_TEST);

    // Set MVP matrices
    model = glm::mat4(1.0f);
    view = glm::lookAt(vec3(1.0f, 1.25f, 1.25f), vec3(0.0f, 0.2f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    projection = glm::mat4(1.0f);

    // Set spotlights ambient diffuse specular uniforms
    // Gun program
    gunProg.use();
    gunProg.setUniform("Spotlights[0].La", vec3(0.2f, 0.0f, 0.0f)); //0.2, 0, 0
    gunProg.setUniform("Spotlights[0].L", vec3(0.8f, 0.0f, 0.0f)); //0.8, 0, 0
    gunProg.setUniform("Spotlights[1].La", vec3(0.0f, 0.2f, 0.0f)); //0, 0.2, 0
    gunProg.setUniform("Spotlights[1].L", vec3(0.0f, 0.8f, 0.0f)); //0, 0.8, 0
    gunProg.setUniform("Spotlights[2].La", vec3(0.0f, 0.0f, 0.2f)); //0, 0, 0.2
    gunProg.setUniform("Spotlights[2].L", vec3(0.0f, 0.0f, 0.8f)); //0, 0, 0.8
    // Plane program
    planeProg.use();
    planeProg.setUniform("Spotlights[0].La", vec3(0.2f, 0.0f, 0.0f)); //0.2, 0, 0
    planeProg.setUniform("Spotlights[0].L", vec3(0.8f, 0.0f, 0.0f)); //0.8, 0, 0
    planeProg.setUniform("Spotlights[1].La", vec3(0.0f, 0.2f, 0.0f)); //0, 0.2, 0
    planeProg.setUniform("Spotlights[1].L", vec3(0.0f, 0.8f, 0.0f)); //0, 0.8, 0
    planeProg.setUniform("Spotlights[2].La", vec3(0.0f, 0.0f, 0.2f)); //0, 0, 0.2
    planeProg.setUniform("Spotlights[2].L", vec3(0.0f, 0.0f, 0.8f)); //0, 0, 0.8

    // Set spotlights exponent uniforms
    for (int i = 0; i < 3; i++)
    {
        std::stringstream exponentName;
        exponentName << "Spotlights[" << i << "].Exponent";
        gunProg.use();
        gunProg.setUniform(exponentName.str().c_str(), 50.0f);
        planeProg.use();
        planeProg.setUniform(exponentName.str().c_str(), 50.0f);
    }

    // Set spotlights cutoff uniforms
    for (int i = 0; i < 3; i++)
    {
        std::stringstream cutoffName;
        cutoffName << "Spotlights[" << i << "].Cutoff";
        gunProg.use();
        gunProg.setUniform(cutoffName.str().c_str(), radians(15.0f));
        planeProg.use();
        planeProg.setUniform(cutoffName.str().c_str(), radians(15.0f));
    }

    // Load textures first
    GLuint diffuseTexture = Texture::loadTexture("media/38-special-revolver/textures/rev_d.tga.png");
    GLuint normalTexture = Texture::loadTexture("media/38-special-revolver/textures/rev_n.tga.png");
    GLuint alphaTexture = Texture::loadTexture("media/texture/dots_alpha.png");

    // Load skybox texture
    GLuint skyboxTexture = Texture::loadHdrCubeMap("media/space_skybox/space");

    // Set active texture unit and bind loaded texture ids to texture buffers
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normalTexture);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, alphaTexture);
    
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
    
    // Increase angle for rotation
    angle += rotSpeed * deltaT;
    if (angle >= two_pi<float>())
    {
        angle -= two_pi<float>();
    }

    // Get keyboard input
    GLFWwindow* windowContext = glfwGetCurrentContext();
    if (glfwGetKey(windowContext, GLFW_KEY_W) == GLFW_PRESS)
    {
        cameraPosition += cameraSpeed * deltaT * cameraForward;
    }
    if (glfwGetKey(windowContext, GLFW_KEY_S) == GLFW_PRESS)
    {
        cameraPosition -= cameraSpeed * deltaT * cameraForward;
    }
    if (glfwGetKey(windowContext, GLFW_KEY_A) == GLFW_PRESS)
    {
        cameraPosition -= normalize(cross(cameraForward, cameraUp)) * cameraSpeed * deltaT;
    }
    if (glfwGetKey(windowContext, GLFW_KEY_D) == GLFW_PRESS)
    {
        cameraPosition += normalize(cross(cameraForward, cameraUp)) * cameraSpeed * deltaT;
    }
    if (glfwGetKey(windowContext, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        cameraPosition += cameraUp * cameraSpeed * deltaT;
    }
    if (glfwGetKey(windowContext, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    {
        cameraPosition -= cameraUp * cameraSpeed * deltaT;
    }

    // Get mouse input (movement)
    double xpos = 0.0, ypos = 0.0;
    glfwGetCursorPos(windowContext, &xpos, &ypos);
    //Initially no last positions, so sets last positions to current positions
    if (mouseFirstEntry)
    {
        lastXPos = (float)xpos;
        lastYPos = (float)ypos;
        mouseFirstEntry = false;
    }

    //Sets values for change in position since last frame to current frame
    float xOffset = (float)xpos - lastXPos;
    float yOffset = lastYPos - (float)ypos;

    //Sets last positions to current positions for next frame
    lastXPos = (float)xpos;
    lastYPos = (float)ypos;

    //Moderates the change in position based on sensitivity value
    xOffset *= cameraSensitivity;
    yOffset *= cameraSensitivity;

    //Adjusts yaw & pitch values against changes in positions
    cameraYaw += xOffset;
    cameraPitch += yOffset;

    //Prevents turning up & down beyond 90 degrees to look backwards
    if (cameraPitch > 89.0f)
    {
        cameraPitch = 89.0f;
    }
    else if (cameraPitch < -89.0f)
    {
        cameraPitch = -89.0f;
    }

    //Modification of direction vector based on mouse turning
    vec3 direction;
    direction.x = cos(radians(cameraYaw)) * cos(radians(cameraPitch));
    direction.y = sin(radians(cameraPitch));
    direction.z = sin(radians(cameraYaw)) * cos(radians(cameraPitch));
    cameraForward = normalize(direction);
}

void SceneBasic_Uniform::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set camera position and view matrix
    //vec3 cameraPos = vec3(15.0f * sin(angle), 10.0f, 15.0f * cos(angle));
    view = lookAt(cameraPosition, cameraPosition + cameraForward, cameraUp);
    mat3 normalMatrix = mat3(vec3(view[0]), vec3(view[1]), vec3(view[2]));

    // Set spotlight uniforms
    for (int i = 0; i < 3; i++)
    {
        // Position
        std::stringstream positionName;
        positionName << "Spotlights[" << i << "].Position";
        float x = 50.0f * sin(angle + (two_pi<float>() / 3) * i); // 100
        float z = 50.0f * cos(angle + (two_pi<float>() / 3) * i);
        vec4 lightPos = vec4(x, 20.0f, z, 1.0f);
        gunProg.use();
        gunProg.setUniform(positionName.str().c_str(), view * lightPos);
        planeProg.use();
        planeProg.setUniform(positionName.str().c_str(), view * lightPos);

        // Direction
        std::stringstream directionName;
        directionName << "Spotlights[" << i << "].Direction";
        gunProg.use();
        gunProg.setUniform(directionName.str().c_str(), normalMatrix * vec3(-lightPos.x, -lightPos.y, -lightPos.z)); // -lightPos.x, y and z
        planeProg.use();
        planeProg.setUniform(directionName.str().c_str(), normalMatrix * vec3(-lightPos.x, -lightPos.y, -lightPos.z)); // -lightPos.x, y and z
    }

    // Skybox rendering
    skyboxProg.use();

    model = mat4(1.0f);

    setMatrices(skyboxProg);
    skybox.render();

    // Gun rendering
    gunProg.use();

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
    model = scale(model, vec3(0.05f));

    // Set MVP matrix uniforms and render gun
    setMatrices(gunProg);
    gun->render();

    // Plane rendering
    planeProg.use();

    // Set plane material uniforms
    planeProg.setUniform("Material.Kd", vec3(0.5f));
    planeProg.setUniform("Material.Ks", vec3(0.0f));
    planeProg.setUniform("Material.Ka", vec3(0.5f));
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