#include "scenebasic_uniform.h"

#include <cstdio>
#include <cstdlib>

#include <string>
using std::string;

#include <sstream>

#include <iostream>
using std::cerr;
using std::endl;

#include <glm/gtc/matrix_transform.hpp>

using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;

#include "helper/glutils.h"

SceneBasic_Uniform::SceneBasic_Uniform() :
    // Load mesh here
    tPrev(0),
    plane(50.f ,50.0f, 1, 1), 
    teapot(14, glm::mat4(1.0f)), 
    torus(1.75f * 0.75f, 0.75f * 0.75f, 50, 50) 
{
    // Load obj here
    city = ObjMesh::load("media/monu3.obj", true);
    cloud = ObjMesh::load("media/clouds.obj", true);
}

void SceneBasic_Uniform::initScene()
{
    compile();
    glEnable(GL_DEPTH_TEST);

    view = glm::lookAt(vec3(3.0f, 5.0f, 3.0f), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

    projection = mat4(1.0f);
    angle = 0.0f;

    // Lights 1
    prog.setUniform("Spot.L", vec3(0.0f, 0.0f, 0.8f));
    prog.setUniform("Spot.La", vec3(0.3f));
    prog.setUniform("Spot.Exponent", 20.0f);
    prog.setUniform("Spot.Cutoff", glm::radians(4.0f));
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
    if (tPrev == 0.0f) deltaT = 0.0f;
    tPrev = t;
    angle += 0.7 * deltaT;
    if (angle > glm::two_pi<float>())angle -= glm::two_pi<float>();
        
}

void SceneBasic_Uniform::render()
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    vec4 lightPos = vec4(20.0f * cos(angle), 10.0f, 20.0f * sin(angle), 1.0f);
    prog.setUniform("Spot.Position", vec3(view * lightPos));
    mat3 normalMatrix = mat3(vec3(view[0]), vec3(view[1]), vec3(view[2]));
    prog.setUniform("Spot.Direction", normalMatrix * vec3(-lightPos)); 

    //                                   R     G     B
    prog.setUniform("Material.Kd", vec3(0.5f, 0.5f, 0.5f));
    prog.setUniform("Material.Ks", vec3(0.95f, 0.95f, 0.95f));
    prog.setUniform("Material.Ka", vec3(0.3f, 0.3f, 0.3f));
    prog.setUniform("Material.Shininess", 200.0f);

    model = mat4(1.0f);
    model = glm::scale(model, glm::vec3(0.9f, 0.9f, 0.9f));
    model = glm::translate(model, vec3(0.0f, 5.0f, 0.0f));

    setMatrices();
    city->render();

    //                                   R     G     B
    prog.setUniform("Material.Kd", vec3(1.0f, 1.0f, 1.0f));
    prog.setUniform("Material.Ks", vec3(0.95f, 0.95f, 0.95f));
    prog.setUniform("Material.Ka", vec3(1.0f, 1.0f, 1.0f));
    prog.setUniform("Material.Shininess", 200.0f);

    model = mat4(1.0f);
    model = glm::scale(model, glm::vec3(1.2f, 1.2f, 1.2f));
    model = glm::translate(model, vec3(-3.5f, 4.2f, 0.0f));

    setMatrices();
    cloud->render();

    //                                   R     G     B
    prog.setUniform("Material.Kd", vec3(1.0f, 1.0f, 1.0f));
    prog.setUniform("Material.Ks", vec3(0.95f, 0.95f, 0.95f));
    prog.setUniform("Material.Ka", vec3(1.0f, 1.0f, 1.0f));
    prog.setUniform("Material.Shininess", 200.0f);

    model = mat4(1.0f);
    model = glm::scale(model, glm::vec3(1.2f, 1.2f, 1.2f));
    model = glm::translate(model, vec3(0.5f, 4.2f, -5.0f));

    setMatrices();
    cloud->render();

    //                                   R     G     B
    prog.setUniform("Material.Kd", vec3(0.7f, 0.7f, 0.7f));
    prog.setUniform("Material.Ks", vec3(0.9f, 0.9f, 0.9f));
    prog.setUniform("Material.Ka", vec3(0.2f, 0.2f, 0.2f));
    prog.setUniform("Material.Shininess", 180.0f);

    model = mat4(1.0f);
    setMatrices();
    plane.render();
}

void SceneBasic_Uniform::resize(int w, int h)
{
    glViewport(0, 0, w, h);
    width = w;
    height = h;

    projection = glm::perspective(glm::radians(90.0f), (float)w / h, 0.3f, 100.0f);
}

void SceneBasic_Uniform::setMatrices()
{
    mat4 mv = view * model;
   prog.setUniform("ModelViewMatrix", mv);
   prog.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
   prog.setUniform("MVP", projection * mv);
}
