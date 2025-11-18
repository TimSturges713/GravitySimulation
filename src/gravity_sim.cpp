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

class Object{
public:
    float radius;
    vector<float> center;
    float massKg;
    vector<float> velocity;
    vector<float> color;

    Object(float radius, vector<float> center, float massKg, vector<float> velocity, vector<float> color){
        this->radius = radius;
        this->center = center;
        this->massKg = massKg;
        this->velocity = velocity;
        this->color = color;
    }

};

void DrawCircle(int triangles, Object circle){
    glColor3f(circle.color[0], circle.color[1], circle.color[2]);

    glBegin(GL_TRIANGLE_FAN);   // tells GL to make a fan of little triangles from these vertices
    glVertex2f(circle.center[0], circle.center[1]);   // set center vertex first

    for(int i = 0; i <= triangles; i++){    // sets the rest of the triangles around the whole circle's circumference
        float theta = i * 2.0f * M_PI / triangles;  // divides the circle into the arc of the triangle in radians, 2pi rad / how many triangles
        float x = circle.center[0] + circle.radius * cos(theta);    // gets the correct coordinates for the two outer vertices 
        float y = circle.center[1] + circle.radius * sin(theta);
        glVertex2f(x, y);       // sets the vertices
    }

    glEnd();
}

void Collides(Object& object1, Object& object2){
    float distance = sqrt(pow(object1.center[0] - object2.center[0], 2) + pow(object1.center[1] - object2.center[1], 2));
    float unitVectorx = (object2.center[0] - object1.center[0]) / distance;
    float unitVectory = (object2.center[1] - object1.center[1]) / distance;

    float xvel = object1.velocity[0] - object2.velocity[0];
    float yvel = object1.velocity[1] - object2.velocity[1];

    float vector =
        xvel * unitVectorx +
        yvel * unitVectory;
    float totalInvMass = 1/object1.massKg + 1/object2.massKg;

    float impulse = (-(1 + .9) * vector) / (totalInvMass);
    float impulsex = unitVectorx * impulse;
    float impulsey = unitVectory * impulse;

    object1.velocity[0] += impulsex * (1/object1.massKg);
    object1.velocity[1] += impulsey * (1/object1.massKg);
    object2.velocity[0] -= impulsex * (1/object2.massKg);
    object2.velocity[1] -= impulsey * (1/object2.massKg);
    
    
    float penetration = object1.radius + object2.radius - distance;
    if(penetration > 0){
        float correctionPercent = 0.98f; 
        float slop = 0.001f; 

        float correction = max(penetration - slop, 0.0f) 
            * correctionPercent;

        object1.center[0] -= unitVectorx * correction * ((1/object1.massKg) / totalInvMass);
        object1.center[1] -= unitVectory * correction * ((1/object1.massKg) / totalInvMass);

        object2.center[0] += unitVectorx * correction * ((1/object2.massKg) / totalInvMass);
        object2.center[1] += unitVectory * correction * ((1/object2.massKg) / totalInvMass);
    }

}

void CollisionDetect(Object& object, vector<Object>& objects){
    for(int i = 0; i < objects.size(); i++){
        float distance = sqrt(pow(object.center[0] - objects[i].center[0], 2) + pow(object.center[1] - objects[i].center[1], 2));
        if(distance <= object.radius + objects[i].radius && distance != 0){
            Collides(object, objects[i]);
        }
    }
    if(object.center[1] - object.radius <= -1.0){     // bottom screen
            object.center[1] = -1.0 + object.radius;
            object.velocity[1] = -object.velocity[1] * 0.95;
    }
    else if(object.center[1] + object.radius >= 1.0){ // top screen
        object.center[1] = 1.0 - object.radius;
        object.velocity[1] = -object.velocity[1] * 0.95;
    }
    else if(object.center[0] + object.radius >= 1.0){ // right screen
        object.center[0] = 1.0 - object.radius;
        object.velocity[0] = -object.velocity[0] * 0.95;
    }
    else if(object.center[0] - object.radius <= -1.0){ // left screen
        object.center[0] = -1.0 + object.radius;
        object.velocity[0] = -object.velocity[0] * 0.95;
    }
    else{
        return;
    }
}

int main(){
    
    Object circle1(
        0.1f,
    {0.5f, 0.2f},
    5.0f,
    {0.0f, 0.0f},
    {0.0f, 1.0f, 1.0f});


    Object circle2(
        0.1f,
    {-0.5f, -0.2f},
    5.0f,
    {4.0f, 0.0f},
    {1.0f, 0.0f, 0.0f});

    Object circle3(0.1f,
    {0.0f, 0.0f},
    5.0f,
    {4.0f, 0.0f},
    {0.0f, 1.0f, 0.0f});

    vector<Object> circles = {circle1, circle2, circle3};
    
    
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
            circles[i].velocity[1] += accelerationy * timeDiff * 2;
            circles[i].velocity[0] += accelerationx * timeDiff * 2;
            circles[i].center[1] += (circles[i].velocity[1] * timeDiff);
            circles[i].center[0] += (circles[i].velocity[0] * timeDiff);
            
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

