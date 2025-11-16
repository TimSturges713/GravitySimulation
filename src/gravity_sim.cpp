#define _USE_MATH_DEFINES
#include <iostream>
#include <GLFW/glfw3.h>
#include <cmath>
#include <vector>
#include <chrono>

using namespace std;

float GRAVITATIONAL_CONSTANT = 6.6743 * pow(10, -11);
float ASTRO_UNIT = 149597870700; // multiply this to the location on the screen

GLFWwindow* StartGLFW();



struct Object{
    float radius;
    float centerx;
    float centery;
    float massKg;
    float velocity;
};

void DrawCircle(int triangles, Object circle){
    glBegin(GL_TRIANGLE_FAN);   // tells GL to make a fan of little triangles from these vertices
    glVertex2f(circle.centerx, circle.centery);   // set center vertex first

    for(int i = 0; i <= triangles; i++){    // sets the rest of the triangles around the whole circle's circumference
        float theta = i * 2.0f * M_PI / triangles;  // divides the circle into the arc of the triangle in radians, 2pi rad / how many triangles
        float x = circle.centerx + circle.radius * cos(theta);    // gets the correct coordinates for the two outer vertices 
        float y = circle.centery + circle.radius * sin(theta);
        glVertex2f(x, y);       // sets the vertices
    }

    glEnd();
}

int main(){
    Object circle1;
        circle1.radius = 0.25f;
        circle1.centerx = 0.0f;
        circle1.centery = 0.0f;
        circle1.massKg = 5.972 * pow(10, 24); // mass of earth
        circle1.velocity = 0;

    Object circle2;
        circle2.radius = 0.25f;
        circle2.centerx = 0.5f;
        circle2.centery = 0.5f;
        circle2.massKg = 5.972 * pow(10, 24);
        circle2.velocity = 0;

    float previousFrameTime = glfwGetTime();

    
    GLFWwindow* window = StartGLFW();   // starts the window up
    while(!glfwWindowShouldClose(window)){  // main GLFW loop for frames
        
        float currentTime = glfwGetTime();
        float timeDiff = currentTime - previousFrameTime;
        if (timeDiff > 0.02 ){
            timeDiff = 0.02;
        }
        previousFrameTime = currentTime;
        
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glColor3f(1.0f, 1.0f, 1.0f);    // sets the color to white
        

        DrawCircle(100, circle1);  // draws a circle with specified radius, center, range of window is [-1.0, 1.0] for floats
        

        

        
        
        float acceleration = -9.81;
        // note center is in AU
        
        
        circle1.velocity += acceleration * timeDiff;
        circle1.centery += (circle1.velocity * timeDiff);
        
        
        if(circle1.centery - circle1.radius <= -1.0){
            circle1.centery = -1.0 + circle1.radius;
            circle1.velocity = -circle1.velocity * 0.9;

        }
        

        glfwSwapBuffers(window);
        glfwPollEvents();
        
    }
}

GLFWwindow* StartGLFW(){
    if(!glfwInit()){    // if the window fails to start this is the backup
        std::cerr<<"failed to initialize GFLW"<<std::endl;
        glfwTerminate();
        return nullptr;
    }

    GLFWwindow* window = glfwCreateWindow(800, 600, "gravity_sim", NULL, NULL); // makes a window with a width of 800 and a height of 600 

    glfwMakeContextCurrent(window); // Applies what is happening with the code to this rendered window
    return window;
}

