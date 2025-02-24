#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "helper/scene.h"

#include <glad/glad.h>
#include "helper/glslprogram.h"
// GLM
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
// Helper files
#include "helper/torus.h"
#include "helper/teapot.h"
#include "helper/plane.h"
#include "helper/objmesh.h"
#include "helper/cube.h"
#include "helper/skybox.h"

class SceneBasic_Uniform : public Scene
{
private:
    GLSLProgram gunProg;
    GLSLProgram planeProg;
    GLSLProgram skyboxProg;
    glm::mat4 rotationMatrix;

    std::unique_ptr<ObjMesh> gun;
    Plane plane;
    SkyBox skybox;
    //Torus torus;
    //Teapot teapot;
    //Cube cube;

    float tPrev;
    float angle;
    float rotSpeed;

    void compile();

public:
    SceneBasic_Uniform();

    void initScene();
    void update( float t );
    void render();
    void resize(int, int);
    void setMatrices(GLSLProgram& p);
};

#endif // SCENEBASIC_UNIFORM_H
