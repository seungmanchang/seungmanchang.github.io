/* This file originated from website LearnOpenGL.com, which distributes the code
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
const glm::mat4 ORIENTATION =
{
    {-1, 0, 0, 0},
    {0, 1, 0, 0},
    {0, 0, -1, 0},
    {0, 0, 0, 1}
};
   
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
    glm::mat4 Orientation;

    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;
  

    // constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
        glm::mat4 orientation = ORIENTATION) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Orientation = orientation;

      updateCameraVectors(glm::mat4(1));
    }
    // constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, 
        glm::mat4 orientation = ORIENTATION): Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Orientation = orientation;

        updateCameraVectors(glm::mat4(1));
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

    // Converts rotation from axis-angle form to matrix form
    glm::mat4 toMatrix(float angle, glm::vec3 axis)
    {
        float acos = cos(glm::radians(angle));
        float asin = sin(glm::radians(angle));
        float at = 1 - acos;
        float ax = axis.x;
        float ay = axis.y;
        float az = axis.z;

        glm::mat4 converted = {
            {at * ax * ax + acos, at * ax * ay - (az * asin), at * ax * az + ay * asin, 0},
            {at * ax * ay + az * asin, at * ay * ay + acos, at * ay * az - (ax * asin), 0},
            {at * ax * az - (ay * asin), at * ay * az + ax * asin, at * az * az + acos, 0},
            {0, 0, 0, 1}
        };

        return converted;
    }

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, float zspin, GLboolean constrainPitch = true)
    {       

        yoffset *= MouseSensitivity;
        glm::mat4 rot1 = toMatrix(yoffset, Right);

        glm::mat4 rot2;
        glm::mat4 rot3 = glm::mat4(1);

        // check whether roll is being modified and calculate the appropriate matrix
        // preserve XYZ (pitch -> yaw -> roll) order
        if (zspin != 0.0)
        {
            zspin *= MouseSensitivity;
            rot2 = toMatrix(zspin, Front);
            rot3 = rot1 * rot2;
        }
        else
        {
            xoffset *= MouseSensitivity;
            rot2 = toMatrix(xoffset, Up);
            rot3 = rot2 * rot1;
        }
     
        // update Front, Right and Up Vectors using the calculated rotation matrix
        
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
    void updateCameraVectors(glm::mat4 rotation)
    {
        // perform rotations at origin
        glm::vec3 prevPos = Position;
        Position = glm::vec3(0.0);
        Orientation = glm::inverse(rotation) * Orientation;

        // calculate Front, Right, and Up vectors
        Front = glm::normalize(glm::vec3(Orientation[2].x, Orientation[2].y, Orientation[2].z ));
        Right = -glm::normalize(glm::vec3(Orientation[0].x, Orientation[0].y, Orientation[0].z));
        Up = glm::normalize(glm::cross(Right, Front));

        // return to previous position
        Position = prevPos;
    }

};
#endif