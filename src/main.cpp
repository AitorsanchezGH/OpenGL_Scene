#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Shaders.h"
#include "Model.h"

void configScene();
void renderScene();
void drawObject(Model &model, glm::vec3 color, glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawSuelo (glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawAspa  (glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawHelice(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawMolino(glm::mat4 P, glm::mat4 V, glm::mat4 M);

void funFramebufferSize(GLFWwindow* window, int width, int height);
void funKey            (GLFWwindow* window, int key  , int scancode, int action, int mods);
void funTimer          (double seconds, double &t0);

// Shaders
   Shaders shaders;

// Modelos
   Model plane;
   Model triangle;
   Model cylinder;

// Viewport
   int w = 500;
   int h = 500;

// Animaciones
   float rotZ = 0.0;
   float desZ = 0.0;
   float rotY = 0.0;

int main() {

 // Inicializamos GLFW
    if(!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

 // Creamos la ventana
    GLFWwindow* window;
    window = glfwCreateWindow(w, h, "Sesion 5", NULL, NULL);
    if(!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);

 // Inicializamos GLEW
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if(GLEW_OK != err) {
        std::cout << "Error: " << glewGetErrorString(err) << std::endl;
        return -1;
    }
    std::cout << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;
    const GLubyte *oglVersion = glGetString(GL_VERSION);
    std::cout <<"This system supports OpenGL Version: " << oglVersion << std::endl;

 // Configuramos los CallBacks
    glfwSetFramebufferSizeCallback(window, funFramebufferSize);
    glfwSetKeyCallback(window, funKey);

 // Entramos en el bucle de renderizado
    configScene();
    double t0 = glfwGetTime();
    while(!glfwWindowShouldClose(window)) {
        renderScene();
        glfwSwapBuffers(window);
        glfwPollEvents();
        funTimer(0.01,t0);
    }
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

void configScene() {

 // Test de profundidad
    glEnable(GL_DEPTH_TEST);
    glPolygonOffset(1.0,1.0);

 // Shaders
    shaders.initShaders("resources/shaders/vshader.glsl","resources/shaders/fshader.glsl");

 // Modelos
    plane.initModel("resources/models/plane.obj");
    triangle.initModel("resources/models/triangle.obj");
    cylinder.initModel("resources/models/cylinder.obj");

}

void renderScene() {

 // Borramos el buffer de color
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

 // Indicamos los shaders a utilizar
    shaders.useShaders();

 // Matriz P
    float fovy   = 60.0;
    float nplane =  0.1;
    float fplane = 25.0;
    float aspect = (float)w/(float)h;
    glm::mat4 P = glm::perspective(glm::radians(fovy), aspect, nplane, fplane);

 // Matriz V
    glm::vec3 eye   (0.0, 3.0, 10.0);
    glm::vec3 center(0.0, 0.0, -1.0);
    glm::vec3 up    (0.0, 1.0,  0.0);
    glm::mat4 V = glm::lookAt(eye, center, up);

 // Dibujamos la escena
    drawSuelo(P,V,I);

    glm::mat4 R = glm::rotate(I, glm::radians(rotY), glm::vec3(0.0, 1.0, 0.0));
    glm::mat4 T = glm::translate(I, glm::vec3(0.0, 0.0, desZ));
    drawMolino(P,V,T*R);

}

void drawObject(Model &model, glm::vec3 color, glm::mat4 P, glm::mat4 V, glm::mat4 M) {

    shaders.setMat4("uPVM",P*V*M);

    glEnable(GL_POLYGON_OFFSET_FILL);
        shaders.setVec3("uColor",color);
        model.renderModel(GL_FILL);
    glDisable(GL_POLYGON_OFFSET_FILL);

    shaders.setVec3("uColor",glm::vec3(1.0, 1.0, 1.0));
    model.renderModel(GL_LINE);

}

void drawSuelo(glm::mat4 P, glm::mat4 V, glm::mat4 M) {

    glm::mat4 S = glm::scale(I, glm::vec3(5.0, 1.0, 5.0));
    drawObject(plane, glm::vec3(0.0, 0.0, 1.0), P, V, M*S);

}

void drawAspa(glm::mat4 P, glm::mat4 V, glm::mat4 M) {

    glm::mat4 T = glm::translate(I, glm::vec3(0.0, -1.0, 0.0));
    glm::mat4 S = glm::scale    (I, glm::vec3(0.2,  0.4 ,1.0));
    drawObject(triangle,glm::vec3(1.0, 0.0, 0.0),P,V,M*S*T);

}

void drawHelice(glm::mat4 P, glm::mat4 V, glm::mat4 M) {
    
    glm::mat4 Rz120 = glm::rotate(I, glm::radians(120.0f), glm::vec3(0, 0, 1));
    glm::mat4 Rauto = glm::rotate(I, glm::radians(rotZ)  , glm::vec3(0, 0, 1));
    drawAspa(P,V,M*Rauto);
    drawAspa(P,V,M*Rauto*Rz120);
    drawAspa(P,V,M*Rauto*Rz120*Rz120);

}

void drawMolino(glm::mat4 P, glm::mat4 V, glm::mat4 M) {

    glm::mat4 T1 = glm::translate(I, glm::vec3(0.0, 1.0, 0.0));
    glm::mat4 S1 = glm::scale    (I, glm::vec3(0.1, 1.0, 0.1));
    drawObject(cylinder,glm::vec3(0.0, 1.0 , 0.0),P,V,M*S1*T1);

    glm::mat4 T2 = glm::translate(I, glm::vec3(0.0, 2.0, 0.0));
    drawHelice(P,V,M*T2);

}

void funFramebufferSize(GLFWwindow* window, int width, int height) {

 // Configuracion del Viewport
    glViewport(0, 0, width, height);

 // Actualizacion de w y h
    w = width;
    h = height;

}

void funKey(GLFWwindow* window, int key  , int scancode, int action, int mods) {

    switch(key) {
        case GLFW_KEY_UP:    desZ -= 0.1; break;
        case GLFW_KEY_DOWN:  desZ += 0.1; break;
        case GLFW_KEY_LEFT:  rotY += 5.0; break;
        case GLFW_KEY_RIGHT: rotY -= 5.0; break;
        default:
            desZ = 0.0;
            rotY = 0.0;
    }

}

void funTimer(double seconds, double &t0) {

    double t1 = glfwGetTime();
    if(t1-t0 > seconds) {
        rotZ -= 2.0;
        t0 = t1;
    }

}
