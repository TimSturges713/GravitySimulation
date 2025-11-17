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
    float velocityy;
    float velocityx;
    float red;
    float green;
    float blue;
};

void DrawCircle(int triangles, Object circle){
    glColor3f(circle.red, circle.green, circle.blue);

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

void Collides(Object& object1, Object& object2){
    float distance = sqrt(pow(object1.centerx - object2.centerx, 2) + pow(object1.centery - object2.centery, 2));
    float unitVectorx = (object2.centerx - object1.centerx) / distance;
    float unitVectory = (object2.centery - object1.centery) / distance;

    float xvel = object1.velocityx - object2.velocityx;
    float yvel = object1.velocityy - object2.velocityy;

    float vector =
        xvel * unitVectorx +
        yvel * unitVectory;
    float totalInvMass = 1/object1.massKg + 1/object2.massKg;

        float impulse = (-(1 + .8) * vector) / (totalInvMass);

        float impulsex = unitVectorx * impulse;
        float impulsey = unitVectory * impulse;

        object1.velocityx += impulsex * (1/object1.massKg);
        object1.velocityy += impulsey * (1/object1.massKg);
        object2.velocityx -= impulsex * (1/object2.massKg);
        object2.velocityy -= impulsey * (1/object2.massKg);
    
    
    float penetration = object1.radius + object2.radius - distance;
    if(penetration > 0){
        float correctionPercent = 0.98f; 
        float slop = 0.001f; 

        float correction = max(penetration - slop, 0.0f) 
            * correctionPercent;

        object1.centerx -= unitVectorx * correction * ((1/object1.massKg) / totalInvMass);
        object1.centery -= unitVectory * correction * ((1/object1.massKg) / totalInvMass);

        object2.centerx += unitVectorx * correction * ((1/object2.massKg) / totalInvMass);
        object2.centery += unitVectory * correction * ((1/object2.massKg) / totalInvMass);
    }

}

void CollisionDetect(Object& object, vector<Object>& objects){
    for(int i = 0; i < objects.size(); i++){
        float distance = sqrt(pow(object.centerx - objects[i].centerx, 2) + pow(object.centery - objects[i].centery, 2));
        if(distance <= object.radius + objects[i].radius && distance != 0){
            Collides(object, objects[i]);
        }
    }
    if(object.centery - object.radius <= -1.0){     // bottom screen
            object.centery = -1.0 + object.radius;
            object.velocityy = -object.velocityy * 0.8;
    }
    else if(object.centery + object.radius >= 1.0){ // top screen
        object.centery = 1.0 - object.radius;
        object.velocityy = -object.velocityy * 0.8;
    }
    else if(object.centerx + object.radius >= 1.0){ // right screen
        object.centerx = 1.0 - object.radius;
        object.velocityx = -object.velocityx * 0.8;
    }
    else if(object.centerx - object.radius <= -1.0){ // left screen
        object.centerx = -1.0 + object.radius;
        object.velocityx = -object.velocityx * 0.8;
    }
    else{
        return;
    }
}

int main(){
    Object circle1;
        circle1.radius = 0.1f;
        circle1.centerx = -0.5f;
        circle1.centery = -0.5f;
        circle1.massKg = 5; // mass of earth
        circle1.velocityy = 0;
        circle1.velocityx = -4;


    Object circle2;
        circle2.radius = 0.1f;
        circle2.centerx = 0.5f;
        circle2.centery = 0.5f;
        circle2.massKg = 5;
        circle2.velocityy = 0;
        circle2.velocityx = 0;

    Object circle3;
        circle3.radius = 0.1f;
        circle3.centerx = 0.5f;
        circle3.centery = 0.2f;
        circle3.massKg = 5;
        circle3.velocityy = 0;
        circle3.velocityx = 4;

    vector<Object> circles = {circle1, circle2, circle3};
    float red = 1.0f;
    float green = 1.0f;
    float blue = 1.0f;
    int cycle = 0;
    for(int i = 0; i < circles.size(); i++){
        circles[i].red = red;
        circles[i].green = green;
        circles[i].blue = blue;
        if(red - 0.3 <= 0){
            red = 1.0f;
        }
        if(cycle == 0){
            red -= 0.3;
            cycle = 1;
            continue;
        }
        if(blue - 0.3 <= 0){
            blue = 1.0f;
        }
        if(cycle == 1){
            blue -= 0.3;
            cycle = 2;
            continue;
        }
        if(green - 0.3 <= 0){
            green = 1.0f;
        }
        if(cycle == 2){
            green -= 0.3;
            cycle = 0;
            continue;
        }
    }
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


        float accelerationy = -9.81;
        float accelerationx = 0;

        for(int i = 0; i < circles.size(); i++){    // velocity and position change loop for all circles
            DrawCircle(100, circles[i]);  // draws a circle with specified radius, center, range of window is [-1.0, 1.0] for floats
            circles[i].velocityy += accelerationy * timeDiff * 2;
            circles[i].velocityx += accelerationx * timeDiff * 2;
            circles[i].centery += (circles[i].velocityy * timeDiff);
            circles[i].centerx += (circles[i].velocityx * timeDiff);
            
        }

        
        // note center is in AU
        
        
        
        for (int i = 0; i < circles.size(); i++){   // collision detection loop for all circles
            CollisionDetect(circles[i], circles);
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

