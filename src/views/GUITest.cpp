/*******************************************************************************************
*
*   raylib [core] example - 3d camera mode
*
*   Example complexity rating: [★☆☆☆] 1/4
*
*   Example originally created with raylib 1.0, last time updated with raylib 1.0
*
*   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software
*
*   Copyright (c) 2014-2025 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"
#include "raymath.h"
#include <math.h>
#include <numbers>
#include <string>
#include <iostream>
#include <vector>
#include <limits>
#include <chrono>
using namespace std;

void printVector3(const Vector3& v) {
    printf("(%.5f, %.5f, %.5f)\n", v.x, v.y, v.z);
}



vector<Vector3> getMinimumPoints(const BoundingBox& bb, Matrix transformation) {
    Vector3 pivot = {(bb.min.x + bb.max.x)/2, (bb.min.y + bb.max.y)/2, (bb.min.z + bb.max.z)/2};
    Matrix rotationMatrix = transformation;
    std::vector<Vector3> bbCorners = {
        {bb.min.x, bb.min.y, bb.min.z},
        {bb.min.x, bb.min.y, bb.max.z},
        {bb.min.x, bb.max.y, bb.min.z},
        {bb.min.x, bb.max.y, bb.max.z},
        {bb.max.x, bb.min.y, bb.min.z},
        {bb.max.x, bb.min.y, bb.max.z},
        {bb.max.x, bb.max.y, bb.min.z},
        {bb.max.x, bb.max.y, bb.max.z}
    };
    vector<Vector3> rotatedBBCorners;
    float minY = std::numeric_limits<float>::max();
    for (Vector3 corner : bbCorners) {
        Vector3 transformedVector = Vector3Add(Vector3Transform(Vector3Subtract(corner, pivot), rotationMatrix), pivot);
        minY = std::min<float>(minY, transformedVector.y);
        rotatedBBCorners.push_back(transformedVector);
    }
    vector<Vector3> minPoints;
    for (Vector3& corner : rotatedBBCorners) {
        if (abs(corner.y -  minY) <= 0.05) {
            minPoints.push_back(corner);
        }
    }
    //cout<<n<<endl;
    
    return minPoints;
}

Vector3 getMinimumPoint(const BoundingBox& bb, Matrix transformation) {
    float totalX = 0;
    float totalY = 0;
    float totalZ = 0;
    int n = 0;
    for (Vector3 corner: getMinimumPoints(bb, transformation)) {
            totalX += corner.x;
            totalY += corner.y;
            totalZ += corner.z;
            n++;
    }
    //cout<<n<<endl;
    return {totalX/n, totalY/n, totalZ/n};
}

BoundingBox rotateBoundingBox(const BoundingBox& bb, Matrix transformation) {
    Vector3 pivot = {(bb.min.x + bb.max.x)/2, (bb.min.y + bb.max.y)/2, (bb.min.z + bb.max.z)/2};
    Matrix rotationMatrix = transformation;
    std::vector<Vector3> bbCorners = {
        {bb.min.x, bb.min.y, bb.min.z},
        {bb.min.x, bb.min.y, bb.max.z},
        {bb.min.x, bb.max.y, bb.min.z},
        {bb.min.x, bb.max.y, bb.max.z},
        {bb.max.x, bb.min.y, bb.min.z},
        {bb.max.x, bb.min.y, bb.max.z},
        {bb.max.x, bb.max.y, bb.min.z},
        {bb.max.x, bb.max.y, bb.max.z}
    };
    std::vector<Vector3> bbCornersRotated;
    float minX = std::numeric_limits<float>::max(), minY = std::numeric_limits<float>::max(), minZ = std::numeric_limits<float>::max();
    float maxX = -minX, maxY = -minY, maxZ = -minZ;
    for (Vector3 corner : bbCorners) {
        Vector3 transformedVector = Vector3Add(Vector3Transform(Vector3Subtract(corner, pivot), rotationMatrix), pivot);
        minX = std::min<float>(minX, transformedVector.x);
        minY = std::min<float>(minY, transformedVector.y);
        minZ = std::min<float>(minZ, transformedVector.z);
        maxX = std::max<float>(maxX, transformedVector.x);
        maxY = std::max<float>(maxY, transformedVector.y);
        maxZ = std::max<float>(maxZ, transformedVector.z);
    }
    return (BoundingBox) {{minX, minY, minZ}, {maxX, maxY, maxZ}};

}

Vector3 applyFriction(const Vector3& force, float mass, float frictionCoeff) {
    float newForceX = abs(force.x) - mass*9.80655*frictionCoeff;
    int forceXdir = force.x < 0 ? -1 : 1;
    if (newForceX < 0) {
        newForceX = 0;
    }
    float newForceZ = abs(force.z) - mass*9.80655*frictionCoeff;
    int forceZdir = force.z < 0 ? -1 : 1;
    if (newForceZ < 0) {
        newForceZ = 0;
    }
    return {forceXdir*newForceX, force.y, forceZdir*newForceZ};
}

bool isConsideredStopped(const Vector3& velocity, const BoundingBox& cubeBB, int fps) {
    
    if (std::abs(velocity.y) <= 9.80655*(1.0f/fps) && cubeBB.min.y < 0) {
        return true;
    }
    return false;
}

void printBoundingBox(const BoundingBox& bb) {
    printf("min : (%.2f, %.2f, %.2f)\n", bb.min.x, bb.min.y, bb.min.z);
    printf("max : (%.2f, %.2f, %.2f)\n", bb.max.x, bb.max.y, bb.max.z);
}


//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - 3d camera mode");
    float cameraXZPos = 30.0f;
    float cameraYPos = 10.0f;
    // Define the camera to look into our 3d world
    Camera3D camera = { 0 };
    camera.position = (Vector3){ cameraXZPos, cameraYPos, 0.0f };  // Camera position
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };      // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera mode type
    

    float cubeStartingY = 7.0f;
    Vector3 cubePosition = { 0.0f, cubeStartingY, -20.0f };
    Vector3 startingCubePosition = cubePosition;
    float startingCubeRotation = 0;
    float cubeEdges = 2.0f;
    Model cubeModel = LoadModelFromMesh(GenMeshCube(cubeEdges, cubeEdges, cubeEdges));
    float cubeMass = 0.05f;
    float cubeInertia = cubeMass*std::pow(cubeEdges,2)/6;
    Matrix cubeRotation = MatrixRotateXYZ({0,0,0});
    float cubeRestitution = 0.65;
    const float gravityAcc = 9.80655;
    Vector3 cubeCollisionPoint ={0,0,0};
    Vector3 throwForce = { 1.0f, -1, 2.5f };
    Vector3 armLength = { 0, -cubeEdges/2, -cubeEdges/2 };
    Vector3 throwTorque = Vector3CrossProduct(armLength, throwForce);
    Vector3 rotationAxis = Vector3Normalize(throwTorque);
    float cubeAngularVelocity = (Vector3Length(throwTorque)*0.2)/cubeInertia;
    float cubeAngularAcc = (Vector3Length(throwTorque))/cubeInertia;

    Vector3 cubeVelocity = Vector3Scale(throwForce, 0.2/cubeMass);
    Vector3 linearAcc = { 0, -gravityAcc, 0 };

    bool cameraMoving = false;
    int cameraRotationDegree = 0;
    auto start = std::chrono::steady_clock::now();
    bool cubeLandedLastLoop = false;
    bool cubeLanded = false;
    float gravityAngularVelocity = 0;
    Vector3 gravityRotationAxis = {0,0,0};
    SetTargetFPS(120);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------
    cout<<cubeAngularVelocity<<endl;
    printVector3(cubeVelocity);
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        cubeLandedLastLoop = cubeLanded;
        cubeLanded = false;
        float deltaTime = (1/120.0f);
        if (IsKeyDown(KEY_RIGHT)) {
            cameraRotationDegree = (cameraRotationDegree - 2) % 360;
        } else if (IsKeyDown(KEY_LEFT)) {
            cameraRotationDegree = ((cameraRotationDegree + 2) % 360 + 360) % 360;
        }
        float radian = cameraRotationDegree * (M_PI/180.0f);
        float currX = std::cos(radian)*cameraXZPos;
        float currZ = std::sin(radian)*cameraXZPos;
        camera.position = (Vector3) {currX, cameraYPos, currZ};
        UpdateCamera(&camera, 0);
        BoundingBox cubeBoundingBox = (BoundingBox) {Vector3SubtractValue(cubePosition, cubeEdges/2), 
                                                    Vector3AddValue(cubePosition, cubeEdges/2)};
        if (!Vector3Equals(cubeVelocity, {0,0,0}) || cubeAngularVelocity != 0 || getMinimumPoints(cubeBoundingBox, cubeModel.transform).size() < 3) {
                cubeBoundingBox = (BoundingBox) {Vector3SubtractValue(cubePosition, cubeEdges/2), 
                                                        Vector3AddValue(cubePosition, cubeEdges/2)};
                BoundingBox nonRotatedBB = cubeBoundingBox;
                cubeBoundingBox = rotateBoundingBox(cubeBoundingBox, cubeModel.transform);
            if (cubeBoundingBox.min.y < 0.01) {
                cubeLanded = true;
                
                //cubePosition.y += std::abs(cubeBoundingBox.min.y);
                //cubeBoundingBox.min.y = 0;
                Vector3 lastLinearVelocity = cubeVelocity;
                cubeVelocity = Vector3Scale(cubeVelocity, cubeRestitution);
                cubeVelocity.y *= -1;
                Vector3 momentum = Vector3Scale(cubeVelocity - lastLinearVelocity, cubeMass);
                Vector3 collisionForce = Vector3Scale(momentum, 1/deltaTime);
                collisionForce = applyFriction(collisionForce, cubeMass, 0.5);
                printVector3(cubeVelocity);
                
                Vector3 collisionPoint = getMinimumPoint(nonRotatedBB, cubeModel.transform);
                cubePosition.y -= collisionPoint.y;
                collisionPoint.y -= collisionPoint.y;
                
                cubeCollisionPoint = collisionPoint;
                collisionPoint = cubeCollisionPoint - cubePosition;
                
                Vector3 collisionTorque = Vector3CrossProduct(collisionForce, collisionPoint);
                float impulse = Vector3Length(collisionTorque)*deltaTime;
                
                cubeAngularVelocity = (impulse)/cubeInertia;

                rotationAxis = Vector3Normalize(collisionTorque);
                Vector3 armLengthCOM = Vector3Scale(collisionPoint, -1);
                Vector3 gravityTorque = Vector3CrossProduct(armLengthCOM, Vector3Scale(linearAcc, cubeMass));

                gravityRotationAxis = Vector3Normalize(gravityTorque);
                if (!cubeLandedLastLoop) {
                    gravityAngularVelocity = 0;
                } 
                gravityAngularVelocity += (Vector3Length(gravityTorque)*deltaTime)/cubeInertia;
                Matrix gravityTransform = MatrixTranslate(armLengthCOM.x, armLengthCOM.y, armLengthCOM.z);
                gravityTransform = MatrixRotateXYZ(gravityRotationAxis*gravityAngularVelocity*deltaTime)*gravityTransform;
                gravityTransform = MatrixTranslate(-armLengthCOM.x, -armLengthCOM.y, -armLengthCOM.z)*gravityTransform;
                
                cubeModel.transform *= gravityTransform;
                
                cubePosition *= gravityTransform;
                cubeBoundingBox = (BoundingBox) {Vector3SubtractValue(cubePosition, cubeEdges/2), 
                                                    Vector3AddValue(cubePosition, cubeEdges/2)};

                cubePosition.y -= getMinimumPoint(cubeBoundingBox, cubeModel.transform).y;
                cubeBoundingBox = (BoundingBox) {Vector3SubtractValue(cubePosition, cubeEdges/2), 
                                                    Vector3AddValue(cubePosition, cubeEdges/2)};
                
                if (abs(cubeVelocity.y) <= abs(linearAcc.y*deltaTime)) {
                    cubeVelocity.y = 0;
                }
                if (abs(cubeAngularVelocity) < 1) {
                    cubeAngularVelocity = 0;
                }
                cubePosition += cubeVelocity*deltaTime;
                cubeBoundingBox = (BoundingBox) {Vector3SubtractValue(cubePosition, cubeEdges/2), 
                                                    Vector3AddValue(cubePosition, cubeEdges/2)};
                
                // if (getMinimumPoints(cubeBoundingBox, cubeModel.transform).size() == 2) {
                //     Vector3 res = getMinimumPoints(cubeBoundingBox, cubeModel.transform)[1] - getMinimumPoints(cubeBoundingBox, cubeModel.transform)[0];
                //     printVector3(res);
                //     printVector3(gravityRotationAxis);
                // }
                
            }
            else {
                cubeRotation = MatrixRotate(rotationAxis, cubeAngularVelocity*deltaTime);
                cubeVelocity += linearAcc*deltaTime;
                cubePosition += cubeVelocity*deltaTime;
                cubeModel.transform = cubeRotation*cubeModel.transform;
            }
            //cout<<cubeLanded<<endl;
        }
        BeginDrawing();

            ClearBackground(RAYWHITE);

            BeginMode3D(camera);

                DrawModel(cubeModel, cubePosition, 1.0f ,RED);
                DrawGrid(50, 1.0f);

            EndMode3D();

            DrawText("Welcome to the third dimension!", 10, 40, 20, DARKGRAY);
            DrawFPS(10, 10);

        EndDrawing();
        
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}