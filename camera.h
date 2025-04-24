// 6DOF camera for CS 330
// By Seungman Chang

/* The original version of this file originated from website LearnOpenGL.com, which distributes the code
with the following information regarding licensing:

All code samples, unless explicitly stated otherwise, are licensed under the terms 
of the CC BY-NC 4.0 license as published by Creative Commons, either version 4 of 
the License, or (at your option) any later version. You can find a human-readable format of the license

https://creativecommons.org/licenses/by-nc/4.0/

and the full license

https://creativecommons.org/licenses/by-nc/4.0/legalcode
*/


#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#include <iostream>


// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN,
};

// Default camera values
const float SPEED       =  2.5f;
const float SENSITIVITY =  0.01f;
const float ZOOM        =  45.0f;

//const glm::mat4 ORIENTATION =
//{
//    {-1, 0, 0, 0},
//    {0, 1, 0, 0},   
//    {0, 0, -1, 0},
//    {0, 0, 0, 1}
//};

// The above orientation matrix SHOULD be equivalent to the quaternion [0, 0, 1, 0] ([w, x, y, z])
// What order does glm::quat() use? wxyz or xyzw? (Update: looks like wxyz)
// When should I be normalizing?

const glm::quat ORIENTATION = { 0, 0, 1, 0 };


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
     
    // rotation matrix
    /*glm::mat4 Orientation;*/
    
    // orientation quaternion
    glm::quat Orientation;

    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;
  

    // constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
        glm::quat orientation = ORIENTATION) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Orientation = orientation;

      updateCameraVectors(glm::quat{1, 0, 0, 0});
    }
    // constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, 
        glm::quat orientation = ORIENTATION): Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Orientation = orientation;

        updateCameraVectors(glm::quat{ 1, 0, 0, 0 });
    }

    // returns the view matrix calculated using the rotation matrix and the LookAt Matrix
    glm::mat4 GetViewMatrix() const
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
        if (direction == UP)
            Position += Up * velocity;
        if (direction == DOWN)
            Position -= Up * velocity;
    }

    // Replace function below with "toQuaternion()" to convert angle-axis form directly to quaternion form

    /* glm::quat [etc.]
    * 
    * - Angle, axis.x, axis.y, and axis.z translate to the four components of a quaternion
    * - May need to be inverted before applying to camera
    
    
    */
    glm::quat toQuaternion(float angle, glm::vec3 axis)
    {
        glm::vec3 axisNormalized = glm::normalize(axis);


        float quatAngle = cos(glm::radians(angle / 2));
        float quatx = axisNormalized.x * sin(glm::radians(angle / 2));
        float quaty = axisNormalized.y * sin(glm::radians(angle / 2));
        float quatz = axisNormalized.z * sin(glm::radians(angle / 2));

        glm::quat converted = { quatAngle, quatx, quaty, quatz };

        return converted;

    }


    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, float zspin, GLboolean constrainPitch = true)
    {       

        yoffset *= MouseSensitivity;
        /*glm::mat4 rot1 = toMatrix(yoffset, Right);*/
        glm::quat rot1 = toQuaternion(yoffset, Right);

        glm::quat rot2;
        /*glm::mat4 rot3 = glm::mat4(1);    */
        glm::quat rot3 = { 1, 0, 0, 0 };

        // check whether roll is being modified and calculate the appropriate matrix
        // preserve XYZ (pitch -> yaw -> roll) order
        if (zspin != 0.0)
        {
            zspin *= MouseSensitivity;
            rot2 = toQuaternion(zspin, Front);
            rot3 = rot1 * rot2;
        }
        else
        {
            xoffset *= MouseSensitivity;
            rot2 = toQuaternion(xoffset, Up);
            rot3 = rot2 * rot1;
        }
     
        // update Front, Right and Up Vectors using the calculated rotation quaternion
        updateCameraVectors(rot3);

    }

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset)
    {
        MovementSpeed -= (float)yoffset;
        if (MovementSpeed < 0.1f)
            MovementSpeed = 0.1f;
        if (MovementSpeed > 45.0f)
            MovementSpeed = 45.0f;
    }


private:
  
    // calculates the front vector from the Camera's (updated) rotation matrix
    void updateCameraVectors(glm::quat rotation)
    {
        // perform rotations at origin
        glm::vec3 prevPos = Position;
        Position = glm::vec3(0.0);
        /*Orientation = glm::inverse(rotation) * Orientation;*/
        Orientation = rotation * Orientation;
        glm::mat4 orientMatrix = glm::mat4(Orientation);

   

     Front = glm::vec3(orientMatrix[2].x, orientMatrix[2].y, orientMatrix[2].z);
     Right = -glm::vec3(orientMatrix[0].x, orientMatrix[0].y, orientMatrix[0].z);
     Up = glm::cross(Right, Front);

     
        // return to previous position
        Position = prevPos;

    }

};
#endif