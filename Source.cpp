#define _USE_MATH_DEFINES  // just to retrieve value of PI

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <cmath>
#include <cstdlib>
#include <ctime>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>

#include "ShaderClass.h"

#include "stb/stb_image.h"
using namespace glm;

// make translation vectors(global)
vec3 transVplayer = vec3(0.0f, 0.0f, 0.0f);
vec3 transVball = vec3(0.0f, 0.0f, 0.0f);
vec3 transVEnemy = vec3(0.0f, 0.0f, 0.0f);
int currPos = 0;

class Polygon {
private:
    vec2 center;
    GLfloat width;
    GLfloat length;
    vec3 color;
    vec2 transPos = center;
    int pos;

public:
    Polygon(vec2 center, GLfloat width, GLfloat length, vec3 color)
        : center(center), width(width), length(length), color(color) {}

    void create(std::vector<GLfloat>& vertices) {
        pos = currPos;
        // top left
        vertices.push_back(center.x);
        vertices.push_back(center.y);
        vertices.push_back(0.0f);
        vertices.push_back(color.x);
        vertices.push_back(color.y);
        vertices.push_back(color.z);

        // top right
        vertices.push_back(center.x + width);
        vertices.push_back(center.y);
        vertices.push_back(0.0f);
        vertices.push_back(color.x);
        vertices.push_back(color.y);
        vertices.push_back(color.z);

        // bottom left
        vertices.push_back(center.x);
        vertices.push_back(center.y + length);
        vertices.push_back(0.0f);
        vertices.push_back(color.x);
        vertices.push_back(color.y);
        vertices.push_back(color.z);

        // bottom right
        vertices.push_back(center.x + width);
        vertices.push_back(center.y + length);
        vertices.push_back(0.0f);
        vertices.push_back(color.x);
        vertices.push_back(color.y);
        vertices.push_back(color.z);
        currPos += 4;
    }

    void render() {
        glDrawArrays(GL_TRIANGLES, pos, 3);

        glDrawArrays(GL_TRIANGLES, pos + 1, 3);
    }

    void updateTransPos(vec2 amount) { transPos += amount; }

    vec2 getTransPos() { return transPos; }
    GLfloat getWidth() { return width; }

    GLfloat getLength() { return length; }
    void checkCollision(vec3& transV) {
        if (transPos.y + length >= 0.97f || transPos.y <= -0.97f)
            transV = vec3(0.0f, 0.0f, 0.0f);
    }

    
};

class Circle {
private:
    vec2 center;
    GLfloat radius;
    GLuint segments;
    vec3 color;
    vec2 transPos = center;
    int pos;

public:
    Circle(vec2 center, GLfloat radius, GLuint segments, vec3 color)
        : center(center), radius(radius), segments(segments), color(color) {}

    void create(std::vector<GLfloat>& vertices) {  // function for organisation
        pos = currPos;

        const GLfloat theta = (2 * M_PI) / segments;

        for (int i = 0; i < segments; i++) {
            vertices.push_back((radius * sin(theta * i)) + center.x);
            vertices.push_back((radius * cos(theta * i)) + center.y);
            vertices.push_back(0.0f);
            vertices.push_back(color.x);
            vertices.push_back(color.y);
            vertices.push_back(color.z);
            currPos++;
        }
    }

    void render(GLenum DrawMode) { glDrawArrays(DrawMode, pos, segments); }

    void updateTransPos(vec2 amount) { transPos += amount; }

    void launch() {
        
        transVball = vec3(-0.01f, 0.01f, 0.0f);

    }

    void checkCollision(Polygon player, Polygon enemy) {

        //check collision with player
        if (this->transPos.y >= player.getTransPos().y && this->transPos.y <= (player.getTransPos().y + player.getLength())) {  // is this ball in the height range of the player?

            if (this->transPos.x - radius <= player.getTransPos().x + player.getWidth()) {
                // this piece of code is for debugging purposes
                /*std::cout << "Collision Detected!" << std::endl;
                std::cout << "Ball Position: (" << transPos.x << ", " << transPos.y << ")" << std::endl;
                std::cout << "Player Position: (" << player.getTransPos().x << ", " << player.getTransPos().y << ")" << std::endl; std::cout << "Player Width: " << player.getWidth() << std::endl;
                std::cout << "Radius: " << radius << std::endl;*/

                transVball.x = -transVball.x + 0.005;
            }
        }

        //check collision with enemy
        if (this->transPos.y >= enemy.getTransPos().y && this->transPos.y <= (enemy.getTransPos().y + enemy.getLength())) {  // is this ball in the height range of the player?

            if (this->transPos.x + radius >= enemy.getTransPos().x) {
                // this piece of code is for debugging purposes
                /*std::cout << "Collision Detected!" << std::endl;
                std::cout << "Ball Position: (" << transPos.x << ", " << transPos.y << ")" << std::endl;
                std::cout << "Player Position: (" << player.getTransPos().x << ", " << player.getTransPos().y << ")" << std::endl;
                std::cout << "Player Width: " << player.getWidth() << std::endl;
                std::cout << "Radius: " << radius << std::endl;*/

                transVball.x = -transVball.x - 0.005;
            }
        }

        //check collision with top and bottom borders
        if (this->transPos.y + this->radius >= 1.0f || this->transPos.y - radius <= -1.0f) transVball.y = -transVball.y;
        if (this->transPos.x + this->radius >= 1.0f || this->transPos.x - radius <= -1.0f) transVball.x = -transVball.x; 
    }
};

/*vec2 calculateCenter(GLfloat radius, GLfloat angle) {
   vec2 point;
   point.x = radius * sin(angle) + 0.0f;
   point.y = radius * cos(angle) + 0.0f;
   return point;

}*/

// modify translation matrix via callback event functions
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch (key) {
        case GLFW_KEY_UP:
            transVplayer = vec3(0.0f, 0.015f, 0.0f);
            break;

        case GLFW_KEY_DOWN:
            transVplayer = vec3(0.0f, -0.015f, 0.0f);
            break;

        case GLFW_KEY_W:
            transVEnemy = vec3(0.0f, 0.015f, 0.0f);
            break;

        case GLFW_KEY_S:
            transVEnemy = vec3(0.0f, -0.015f, 0.0f);
            break;
        }
    }
}

void gameEnd() {



}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(650, 650, "Game: Pong", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    gladLoadGL();
    glViewport(0, 0, 650, 650);

    Shader shaderProgram("default.vert", "default.frag");

    std::vector<GLfloat> vertices;

    Polygon player(vec2(-0.9f, 0.01f), 0.04f, 0.28f, vec3(1.0f, 1.0f, 1.0f));
    player.create(vertices);

    Polygon enemy(vec2(0.85f, 0.01f), 0.04f, 0.28f, vec3(1.0f, 1.0f, 1.0f));
    enemy.create(vertices);

    Circle ball(vec2(0.0f, 0.0f), 0.05f, 500, vec3(1.0f, 1.0f, 1.0f));
    ball.create(vertices);
    ball.launch();

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char* data = stbi_load("Space.jpeg", &width, &height, &nrChannels, 0);
    
    if (data) {

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

    }
    else {

        std::cout << "Failed to load texture" << std::endl;

    }

    stbi_image_free(data);
    
    




    // debugging purposes
    /*for (int i = 0; i < vertices.size(); i++) {

        std::cout << vertices[i] << " ";
        if ((i + 1) % 6 == 0) std::cout << std::endl;

    }*/

    // this section is for testing
    /*vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);
    mat4 trans = mat4(1.0f);
    trans = translate(trans, vec3(1.0f, 1.0f, 0.0f));
    vec = trans * vec;
    std::cout << vec.x << vec.y << vec.z << std::endl;*/

    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // give a pointer to the key_callback function to implement keyboard input functionality
    glfwSetKeyCallback(window, key_callback);

    const mat4 id = mat4(1.0f);
    mat4 transPlayer = mat4(1.0f);
    mat4 transBall = mat4(1.0f);
    mat4 transEnemy = mat4(1.0f);
    GLuint uniform = glGetUniformLocation(shaderProgram.ID, "transform");

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        shaderProgram.Activate();

        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(VAO);

        player.updateTransPos(transVplayer);
        player.checkCollision(transVplayer);
        transPlayer = translate(transPlayer, transVplayer);

        glUniformMatrix4fv(uniform, 1, GL_FALSE, value_ptr(transPlayer));

        player.render();

        

        transBall = translate(transBall, transVball);
        ball.updateTransPos(transVball);
        ball.checkCollision(player, enemy);
        glUniformMatrix4fv(uniform, 1, GL_FALSE, value_ptr(transBall));
        ball.render(GL_TRIANGLE_FAN);

        
        enemy.updateTransPos(transVEnemy);
        enemy.checkCollision(transVEnemy);
        transEnemy = translate(transEnemy, transVEnemy);
        glUniformMatrix4fv(uniform, 1, GL_FALSE, value_ptr(transEnemy));
        enemy.render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &VBO);
    shaderProgram.Delete();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}