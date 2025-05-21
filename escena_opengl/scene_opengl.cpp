#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <numbers>

// const double pi = 3.14159265358979323846;
const double pi = acos(-1.0);

// Vertex shader source code
const char* vertexShaderSource = R"(
#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
out vec3 fragNormal;
out vec3 fragPosition;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main() {
    fragPosition = vec3(model * vec4(position, 1.0));
    fragNormal = mat3(transpose(inverse(model))) * normal;
    gl_Position = projection * view * model * vec4(position, 1.0);
}
)";

// Fragment shader source code
const char* fragmentShaderSource = R"(
#version 330 core
in vec3 fragNormal;
in vec3 fragPosition;
out vec4 color;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 objectColor;
void main() {
    // Ambient lighting
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse lighting
    vec3 norm = normalize(fragNormal);
    vec3 lightDir = normalize(lightPos - fragPosition);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular lighting
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - fragPosition);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    // Combine results
    vec3 result = (ambient + diffuse + specular) * objectColor;
    color = vec4(result, 1.0);
}
)";

// Utility functions for shader compilation and linking
GLuint compileShader(GLenum type, const char* src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    // Check for compilation errors
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    return shader;
}
GLuint createProgram(const char* vs, const char* fs) {
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vs);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fs);
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    // Clean up shaders as they're no longer needed
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return program;
}

// Cube data (positions and normals)
float cubeVertices[] = {
    // Positions          // Normals
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f, 0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f, 0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f,
     0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  1.0f,
     0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f,

    -0.5f,  0.5f,  0.5f, -1.0f, 0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f,  0.5f, -1.0f, 0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f, 0.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
};


// Función para generar una malla de cono (posiciones y normales intercaladas)
void generateCone(std::vector<float>& vertices, int segments = 32, float height = 1.0f, float radius = 0.5f) {
    glm::vec3 baseCenter(0.0f, 0.0f, 0.0f);
    glm::vec3 apex(0.0f, height, 0.0f);
    glm::vec3 baseNormal(0.0f, -1.0f, 0.0f);

    // Base (fan)
    for (int i = 0; i < segments; ++i) {
        float theta0 = 2.0f * pi * i / segments;
        float theta1 = 2.0f * pi * (i + 1) / segments;
        float x0 = radius * cos(theta0);
        float z0 = radius * sin(theta0);
        float x1 = radius * cos(theta1);
        float z1 = radius * sin(theta1);

        // Base triangle (center, edge0, edge1)
        vertices.push_back(baseCenter.x); vertices.push_back(baseCenter.y); vertices.push_back(baseCenter.z);
        vertices.push_back(baseNormal.x); vertices.push_back(baseNormal.y); vertices.push_back(baseNormal.z);
        vertices.push_back(x1); vertices.push_back(0.0f); vertices.push_back(z1);
        vertices.push_back(baseNormal.x); vertices.push_back(baseNormal.y); vertices.push_back(baseNormal.z);
        vertices.push_back(x0); vertices.push_back(0.0f); vertices.push_back(z0);
        vertices.push_back(baseNormal.x); vertices.push_back(baseNormal.y); vertices.push_back(baseNormal.z);
    }

    // Sides
    for (int i = 0; i < segments; ++i) {
        float theta0 = 2.0f * pi * i / segments;
        float theta1 = 2.0f * pi * (i + 1) / segments;
        float x0 = radius * cos(theta0);
        float z0 = radius * sin(theta0);
        float x1 = radius * cos(theta1);
        float z1 = radius * sin(theta1);

        glm::vec3 p0(x0, 0.0f, z0);
        glm::vec3 p1(x1, 0.0f, z1);

        // Calculate normals for smooth shading
        glm::vec3 n0 = glm::normalize(glm::vec3(x0, radius / height, z0));
        glm::vec3 n1 = glm::normalize(glm::vec3(x1, radius / height, z1));
        glm::vec3 apexNormal = glm::normalize(glm::vec3((x0 + x1) / 2.0f, radius / height, (z0 + z1) / 2.0f));

        // Side triangle (apex, p1, p0)
        vertices.push_back(apex.x); vertices.push_back(apex.y); vertices.push_back(apex.z);
        vertices.push_back(apexNormal.x); vertices.push_back(apexNormal.y); vertices.push_back(apexNormal.z);
        vertices.push_back(p1.x); vertices.push_back(p1.y); vertices.push_back(p1.z);
        vertices.push_back(n1.x); vertices.push_back(n1.y); vertices.push_back(n1.z);
        vertices.push_back(p0.x); vertices.push_back(p0.y); vertices.push_back(p0.z);
        vertices.push_back(n0.x); vertices.push_back(n0.y); vertices.push_back(n0.z);
    }
}

// Generar una malla de esfera
void generateSphere(std::vector<float>& vertices, int sectorCount = 32, int stackCount = 16, float radius = 0.5f) {
    for (int i = 0; i < stackCount; ++i) {
        float stackAngle1 = pi / 2 - i * pi / stackCount;
        float stackAngle2 = pi / 2 - (i + 1) * pi / stackCount;
        float xy1 = radius * cosf(stackAngle1);
        float z1 = radius * sinf(stackAngle1);
        float xy2 = radius * cosf(stackAngle2);
        float z2 = radius * sinf(stackAngle2);

        for (int j = 0; j < sectorCount; ++j) {
            float sectorAngle1 = j * 2 * pi / sectorCount;
            float sectorAngle2 = (j + 1) * 2 * pi / sectorCount;

            float x1 = xy1 * cosf(sectorAngle1);
            float y1 = xy1 * sinf(sectorAngle1);
            float x2 = xy2 * cosf(sectorAngle1);
            float y2 = xy2 * sinf(sectorAngle1);
            float x3 = xy2 * cosf(sectorAngle2);
            float y3 = xy2 * sinf(sectorAngle2);
            float x4 = xy1 * cosf(sectorAngle2);
            float y4 = xy1 * sinf(sectorAngle2);

            // Primer triángulo
            vertices.push_back(x1); vertices.push_back(y1); vertices.push_back(z1);
            glm::vec3 n1 = glm::normalize(glm::vec3(x1, y1, z1));
            vertices.push_back(n1.x); vertices.push_back(n1.y); vertices.push_back(n1.z);

            vertices.push_back(x2); vertices.push_back(y2); vertices.push_back(z2);
            glm::vec3 n2 = glm::normalize(glm::vec3(x2, y2, z2));
            vertices.push_back(n2.x); vertices.push_back(n2.y); vertices.push_back(n2.z);

            vertices.push_back(x3); vertices.push_back(y3); vertices.push_back(z2);
            glm::vec3 n3 = glm::normalize(glm::vec3(x3, y3, z2));
            vertices.push_back(n3.x); vertices.push_back(n3.y); vertices.push_back(n3.z);

            // Segundo triángulo
            vertices.push_back(x1); vertices.push_back(y1); vertices.push_back(z1);
            vertices.push_back(n1.x); vertices.push_back(n1.y); vertices.push_back(n1.z);

            vertices.push_back(x3); vertices.push_back(y3); vertices.push_back(z2);
            vertices.push_back(n3.x); vertices.push_back(n3.y); vertices.push_back(n3.z);

            vertices.push_back(x4); vertices.push_back(y4); vertices.push_back(z1);
            glm::vec3 n4 = glm::normalize(glm::vec3(x4, y4, z1));
            vertices.push_back(n4.x); vertices.push_back(n4.y); vertices.push_back(n4.z);
        }
    }
}


void createObject(GLuint& VAO, GLuint& VBO, float* vertices, size_t vertCount) {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertCount * sizeof(float), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
}

void createObjectVec(GLuint& VAO, GLuint& VBO, const std::vector<float>& vertices) {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
}

// Camera selection state
int currentCamera = 0;

// Camera positions and targets
struct Camera {
    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 up;
};

// Crear unas cámaras alternativas para la escena, de frente, inclinada y lateral
std::vector<Camera> cameras = {
    // Cámara 1: Vista frontal (centrada en X, mirando al cubo desde Z+)
    { 
        glm::vec3(-1.5f, 2.0f, 5.0f), // desplazada a la izquierda (X negativo)
        glm::vec3(-1.0f, 0.5f, 0.0f), 
        glm::vec3(0.0f, 1.0f, 0.0f) 
    },
    // Cámara 2: Vista entre cenital, frontal y lateral
    { 
        glm::vec3(2.0f, 4.0f, 2.0f), 
        glm::vec3(0.0f, 0.5f, 0.0f), 
        glm::vec3(0.0f, 1.0f, 0.0f) 
    },
    // Cámara 3: Vista inclinada entre lateral y frontal
    { 
        glm::vec3(4.0f, 2.0f, 4.0f), 
        glm::vec3(0.0f, 0.5f, 0.0f), 
        glm::vec3(0.0f, 1.0f, 0.0f) 
    }
};

// Variables para controlar la rotación de la cámara alrededor del cubo
float cameraAngle = 0.0f; // ángulo en radianes
float cameraRadius = 5.0f; // distancia al centro
glm::vec3 cameraTarget = glm::vec3(-1.0f, 0.5f, 0.0f); // centro del cubo

// Callback para teclas: flechas izquierda/derecha rotan la cámara
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        if (key == GLFW_KEY_1) currentCamera = 0;
        if (key == GLFW_KEY_2) currentCamera = 1;
        if (key == GLFW_KEY_3) currentCamera = 2;
        if (key == GLFW_KEY_LEFT)  cameraAngle -= 0.1f;
        if (key == GLFW_KEY_RIGHT) cameraAngle += 0.1f;
    }
}

// Función para calcular la posición de la cámara rotando alrededor del cubo
glm::vec3 getOrbitCameraPosition(float angle, float radius, glm::vec3 target, float height = 2.0f) {
    float camX = target.x + sin(angle) * radius;
    float camZ = target.z + cos(angle) * radius;
    return glm::vec3(camX, height, camZ);
}

int main() {
    // Inicialización de la ventana y OpenGL
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(800, 600, "Cube Scene - Cameras", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    glewInit();

    // Controlador de cámara
    glfwSetKeyCallback(window, key_callback);

    // Compilación y enlace de los shaders
    GLuint shader = createProgram(vertexShaderSource, fragmentShaderSource);

    // Creación del cubo y el cono
    GLuint cubeVAO, cubeVBO;
    createObject(cubeVAO, cubeVBO, cubeVertices, sizeof(cubeVertices)/sizeof(float));

    // Generación de los datos del cono
    std::vector<float> coneVertices;
    generateCone(coneVertices, 32, 1.0f, 0.5f);
    GLuint coneVAO, coneVBO;
    createObjectVec(coneVAO, coneVBO, coneVertices);

    // Generación de los datos de la esfera
    std::vector<float> sphereVertices;
    generateSphere(sphereVertices, 32, 16, 0.5f);
    GLuint sphereVAO, sphereVBO;
    createObjectVec(sphereVAO, sphereVBO, sphereVertices);

    glEnable(GL_DEPTH_TEST);

    // Bucle de renderizado
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.1f,0.1f,0.1f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader);

        Camera cam;
        if (currentCamera == 0) {
            // Cámara orbital controlada por flechas
            cam.position = getOrbitCameraPosition(cameraAngle, cameraRadius, cameraTarget, 2.0f);
            cam.target = cameraTarget;
            cam.up = glm::vec3(0.0f, 1.0f, 0.0f);
        } else {
            // Cámaras fijas
            cam = cameras[currentCamera];
        }
        glm::mat4 view = glm::lookAt(cam.position, cam.target, cam.up);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f/600.0f, 0.1f, 100.0f);

        // Matrices y parámetros de iluminación a los shaders
        glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniform3f(glGetUniformLocation(shader, "lightPos"), 2.0f, 3.0f, 2.0f);
        glUniform3f(glGetUniformLocation(shader, "viewPos"), cam.position.x, cam.position.y, cam.position.z);
        glUniform3f(glGetUniformLocation(shader, "lightColor"), 1.0f, 1.0f, 1.0f);

        // Definicion del cubo
        glm::mat4 cubeModel = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f,0.5f,0.0f));

        // Añadir rotación al cubo
        float angle = glfwGetTime() * 50.0f; // 50 grados por segundo
        cubeModel = glm::rotate(cubeModel, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(cubeModel));
        glBindVertexArray(cubeVAO);

        // Añadir colores por cara
        glm::vec3 faceColors[6] = {
            glm::vec3(1.0f, 0.0f, 0.0f), // Rojo
            glm::vec3(0.0f, 1.0f, 0.0f), // Verde
            glm::vec3(0.0f, 0.0f, 1.0f), // Azul
            glm::vec3(1.0f, 1.0f, 0.0f), // Amarillo
            glm::vec3(1.0f, 0.0f, 1.0f), // Magenta
            glm::vec3(0.0f, 1.0f, 1.0f)  // Cyan
        };
        for (int i = 0; i < 6; ++i) {
            glUniform3f(glGetUniformLocation(shader, "objectColor"),
                faceColors[i].r, faceColors[i].g, faceColors[i].b);
            glDrawArrays(GL_TRIANGLES, i * 6, 6);
        }


        // Dibujar el cono
        glm::mat4 coneModel = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f,0.0f,0.0f));
        glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(coneModel));
        
        // Colorear de color verde
        glUniform3f(glGetUniformLocation(shader, "objectColor"), 0.0f, 1.0f, 0.0f);
        glBindVertexArray(coneVAO);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(coneVertices.size() / 6));

        // Dibujar la esfera
        glm::mat4 sphereModel = glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f, 0.5f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(sphereModel));

        // Colorear de color naranja
        glUniform3f(glGetUniformLocation(shader, "objectColor"), 1.0f, 0.5f, 0.0f); // naranja
        glBindVertexArray(sphereVAO);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(sphereVertices.size() / 6));

        // Intercambia los buffers y procesa eventos
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Limpieza de recursos
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteVertexArrays(1, &coneVAO);
    glDeleteBuffers(1, &coneVBO);
    glDeleteVertexArrays(1, &sphereVAO);
    glDeleteBuffers(1, &sphereVBO);
    glDeleteProgram(shader);
    glfwTerminate();
    return 0;
}
