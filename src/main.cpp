#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Shaders.h"
#include "Model.h"

void configScene();
void renderScene();
void setLights (glm::mat4 P, glm::mat4 V);
void drawObject(Model &model, Material &material, glm::mat4 P, glm::mat4 V, glm::mat4 M);

void funFramebufferSize(GLFWwindow* window, int width, int height);
void funKey            (GLFWwindow* window, int key  , int scancode, int action, int mods);
void funScroll         (GLFWwindow* window, double xoffset, double yoffset);
void funCursorPos      (GLFWwindow* window, double xpos, double ypos);


// Shaders
   Shaders shaders;

// Modelos
   Model sphere;
   Model plane;

// Luces y materiales
   #define   NLD 1
   #define   NLP 1
   #define   NLF 1
   Light     lightG;
   Light     lightD[NLD];
   Light     lightP[NLP];
   Light     lightF[NLF];
   Material  mluz;
   Material  ruby;
   Material  gold;
   Material  cyan_plastic;

// Viewport
   int w = 500;
   int h = 500;

// Movimiento de camara
   float fovy   = 60.0;
   float lastX = 0.0;
   float lastY = 0.0;
   float alphaX = 0.0;
   float alphaY = 20.0;
   bool mouseLeftPressed = false;

// Interaccion con las luces
   float onOff  = 1.0;
   float potD   = 1.0;
   float rotY   = 0.0;

int main() {

 // Inicializamos GLFW
    if(!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

 // Creamos la ventana
    GLFWwindow* window;
    window = glfwCreateWindow(w, h, "Sesion 6", NULL, NULL);
    if(!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

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
    glfwSetKeyCallback      (window, funKey);
    glfwSetScrollCallback   (window, funScroll);
    glfwSetCursorPosCallback(window, funCursorPos);

 // Entramos en el bucle de renderizado
    configScene();
    while(!glfwWindowShouldClose(window)) {
        renderScene();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

void configScene() {

 // Test de profundidad
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

 // Shaders
    shaders.initShaders("resources/shaders/vshader.glsl","resources/shaders/fshader.glsl");
    //shaders.initShaders("resources/shaders/vshader.glsl","resources/shaders/fshader_cartoon.glsl");
    //shaders.initShaders("resources/shaders/vshader.glsl","resources/shaders/fshader_procedural.glsl");
    //shaders.initShaders("resources/shaders/vshader.glsl","resources/shaders/fshader_discard.glsl");

 // Modelos
    sphere.initModel("resources/models/sphere.obj");
    plane.initModel("resources/models/plane.obj");

 // Luz ambiental global
    lightG.ambient = glm::vec3(0.5, 0.5, 0.5);

 // Luces direccionales
    lightD[0].direction = glm::vec3(0.0, 1.0, 0.0);

 // Luces posicionales
    lightP[0].ambient     = glm::vec3(0.2, 0.2, 0.2);
    lightP[0].diffuse     = glm::vec3(0.9, 0.9, 0.9);
    lightP[0].specular    = glm::vec3(0.9, 0.9, 0.9);
    lightP[0].c0          = 1.00;
    lightP[0].c1          = 0.22;
    lightP[0].c2          = 0.20;

 // Luces focales
    lightF[0].position    = glm::vec3(5.0, 3.0, 5.0);
    lightF[0].direction   = glm::vec3(2.0, 0.0, 2.0) - lightF[0].position;
    lightF[0].innerCutOff = 7.0;
    lightF[0].outerCutOff = lightF[0].innerCutOff + 0.1;
    lightF[0].c0          = 1.000;
    lightF[0].c1          = 0.090;
    lightF[0].c2          = 0.032;

 // Materiales
    mluz.ambient   = glm::vec4(0.0, 0.0, 0.0, 1.0);
    mluz.diffuse   = glm::vec4(0.0, 0.0, 0.0, 1.0);
    mluz.specular  = glm::vec4(0.0, 0.0, 0.0, 1.0);
    mluz.emissive  = glm::vec4(1.0, 1.0, 1.0, 1.0);
    mluz.shininess = 1.0;
    ruby.ambient   = glm::vec4(0.174500, 0.011750, 0.011750, 0.55);
    ruby.diffuse   = glm::vec4(0.614240, 0.041360, 0.041360, 0.55);
    ruby.specular  = glm::vec4(0.727811, 0.626959, 0.626959, 0.55);
    ruby.emissive  = glm::vec4(0.000000, 0.000000, 0.000000, 1.00);
    ruby.shininess = 76.8;
    gold.ambient   = glm::vec4(0.24725, 0.1995, 0.0745, 1.00 );
    gold.diffuse   = glm::vec4(0.75164, 0.60648, 0.22648, 1.00);
    gold.specular  = glm::vec4(0.628281, 0.555802, 0.366065, 1.00);
    gold.emissive  = glm::vec4(0.000000, 0.000000, 0.000000, 1.00);
    gold.shininess = 51.2;
    cyan_plastic.ambient   = glm::vec4(0.00,0.10,0.06 ,1.00 );
    cyan_plastic.diffuse   = glm::vec4(0.00,0.50980392,0.50980392,1.00);
    cyan_plastic.specular  = glm::vec4(0.50196078,0.50196078,0.50196078,1.00);
    cyan_plastic.emissive  = glm::vec4(0.000000, 0.000000, 0.000000, 1.00);
    cyan_plastic.shininess = 32.0;

}

void renderScene() {

 // Borramos el buffer de color
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

 // Indicamos los shaders a utilizar
    shaders.useShaders();

 // Matriz P
    float nplane =  0.1;
    float fplane = 25.0;
    float aspect = (float)w/(float)h;
    glm::mat4 P = glm::perspective(glm::radians(fovy), aspect, nplane, fplane);

 // Matriz V
    float x = 10.0f*glm::cos(glm::radians(alphaY))*glm::sin(glm::radians(alphaX));
    float y = 10.0f*glm::sin(glm::radians(alphaY));
    float z = 10.0f*glm::cos(glm::radians(alphaY))*glm::cos(glm::radians(alphaX));
    glm::vec3 eye   (  x,   y,   z);
    glm::vec3 center(0.0, 0.0,  0.0);
    glm::vec3 up    (0.0, 1.0,  0.0);
    glm::mat4 V = glm::lookAt(eye, center, up);
    shaders.setVec3("ueye",eye);

 // Fijamos las luces
    setLights(P,V);

 // Dibujamos la escena
    glm::mat4 S = glm::scale (I, glm::vec3(5.5, 1.0, 5.5));
    glm::mat4 R = glm::rotate(I,glm::radians(180.0f), glm::vec3(1, 0, 0));
    drawObject(plane,cyan_plastic,P,V,S);
    drawObject(plane,cyan_plastic,P,V,S*R);
    glm::mat4 T = glm::translate(I, glm::vec3(0.0, 3.0, 0.0));
    drawObject(sphere,gold,P,V,T);

}

void setLights(glm::mat4 P, glm::mat4 V) {

    shaders.setLight("ulightG",lightG);

 // Luz direccional
    lightD[0].ambient  = potD*glm::vec3(0.1,  0.1, 0.1);
    lightD[0].diffuse  = potD*glm::vec3(0.7,  0.7, 0.7);
    lightD[0].specular = potD*glm::vec3(0.7,  0.7, 0.7);
    shaders.setLight("ulightD[0]",lightD[0]);

 // Fijamos la luz posicional
    lightP[0].position  = glm::vec3(-2.0, 0.5, 0.0);
    glm::mat4 R = glm::rotate(I, glm::radians(rotY), glm::vec3(1.0, 0.0, 0.0));
    lightP[0].position = R*glm::vec4(lightP[0].position,1);
    shaders.setLight("ulightP[0]",lightP[0]);
    glm::mat4 Mp = glm::translate(I,lightP[0].position) * glm::scale(I,glm::vec3(0.025));
    drawObject(sphere,mluz,P,V,Mp);

 // Fijamos la luz focal
    lightF[0].ambient  = onOff*glm::vec3( 0.2,  0.2,  0.2);
    lightF[0].diffuse  = onOff*glm::vec3( 0.9,  0.9,  0.9);
    lightF[0].specular = onOff*glm::vec3( 0.9,  0.9,  0.9);
    shaders.setLight("ulightF[0]",lightF[0]);
    glm::mat4 Mf = glm::translate(I,lightF[0].position) * glm::scale(I,glm::vec3(0.025));
    drawObject(sphere,mluz,P,V,Mf);

}

void drawObject(Model &model, Material &material, glm::mat4 P, glm::mat4 V, glm::mat4 M) {

    shaders.setMat4("uN"  ,glm::transpose(glm::inverse(M)));
    shaders.setMat4("uM"  ,M);
    shaders.setMat4("uPVM",P*V*M);
    shaders.setMaterial("umaterial",material);
    model.renderModel(GL_FILL);

}

void funFramebufferSize(GLFWwindow* window, int width, int height) {

 // Configuracion del Viewport
    glViewport(0, 0, width, height);

 // Actualizacion de w y h
    w = width;
    h = height;

}

void funScroll(GLFWwindow* window, double xoffset, double yoffset) {

    if(yoffset>0) fovy -= fovy>10.0f ? 5.0f : 0.0f;
    if(yoffset<0) fovy += fovy<90.0f ? 5.0f : 0.0f;

}


void funCursorPos(GLFWwindow* window, double xpos, double ypos) {
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        if (!mouseLeftPressed) {
            // Se presionó el botón izquierdo, almacenar la posición inicial
            lastX = xpos;
            lastY = ypos;
            mouseLeftPressed = true;
        }

        float limY = 89.0;
        alphaX += 90.0 * (2.0 * (xpos - lastX) / (float)w);
        alphaY += 90.0 * (2.0 * (lastY - ypos) / (float)h);
        if (alphaY < -limY) alphaY = limY;
        if (alphaY > limY) alphaY = -limY;

        // Actualizar la posición inicial para el próximo movimiento
        lastX = xpos;
        lastY = ypos;
    } else {
        // Se soltó el botón izquierdo, restablecer el estado
        mouseLeftPressed = false;
    }
}

void funKey(GLFWwindow* window, int key, int scancode, int action, int mods) {

    switch(key) {
        case GLFW_KEY_F:
            if(action==GLFW_PRESS) onOff = onOff==0.0f ? 1.00f : 0.0f;
            break;
        case GLFW_KEY_UP:   potD += potD < 1.0f ? 0.05f : 0.0f; break;
        case GLFW_KEY_DOWN: potD -= potD > 0.0f ? 0.05f : 0.0f; break;
            break;
        case GLFW_KEY_P:
            if(action==GLFW_REPEAT) {
                if(mods==GLFW_MOD_SHIFT) rotY -= 5.0;
                else                     rotY += 5.0;
            }
            break;
        default:
            fovy   = 60.0;
            alphaX =  0.0;
            alphaY = 20.0;
            onOff  =  1.0;
            potD   =  1.0;
            rotY   =  0.0;
    }

}