#include "../include/views/animation/physicsEngine/PhysicsEngine.hpp"
#include <cstring>

void printVector3(const Vector3& v) {
    printf("(%.5f, %.5f, %.5f)\n", v.x, v.y, v.z);
}

Mesh CopyMesh(Mesh original) {
    Mesh copy = { 0 };
    copy.vertexCount = original.vertexCount;
    copy.triangleCount = original.triangleCount;

    if (original.vertices != nullptr) {
        int size = original.vertexCount * 3 * sizeof(float);
        copy.vertices = (float *)MemAlloc(size);
        std::memcpy(copy.vertices, original.vertices, size);
    }

    if (original.texcoords != nullptr) {
        int size = original.vertexCount * 2 * sizeof(float);
        copy.texcoords = (float *)MemAlloc(size);
        std::memcpy(copy.texcoords, original.texcoords, size);
    }

    if (original.normals != nullptr) {
        int size = original.vertexCount * 3 * sizeof(float);
        copy.normals = (float *)MemAlloc(size);
        std::memcpy(copy.normals, original.normals, size);
    }

    // Allocate and copy indices (if applicable)
    if (original.indices != nullptr) {
        int size = original.triangleCount * 3 * sizeof(unsigned short);
        copy.indices = (unsigned short *)MemAlloc(size);
        std::memcpy(copy.indices, original.indices, size);
    }

    // Upload to GPU after copying CPU data
    UploadMesh(&copy, false);
    return copy;
}

Model createDiceModel(float diceEdges) {
    Mesh cubeMesh = GenMeshCube(diceEdges, diceEdges, diceEdges);
    int i = 0;
    vector<float> Xloc = {0.125, 0.375, 0.625};
    vector<float> Yloc = {0.25, 0.75};
    for (float y : Yloc) {
        for (float x : Xloc) {
            cubeMesh.texcoords[i*8] = x - 0.125;
            cubeMesh.texcoords[i*8 + 1] = y - 0.25;

            cubeMesh.texcoords[i*8 + 2] = x + 0.125;
            cubeMesh.texcoords[i*8 + 3] = y - 0.25;

            cubeMesh.texcoords[i*8 + 4] = x + 0.125;
            cubeMesh.texcoords[i*8 + 5] = y + 0.25;

            cubeMesh.texcoords[i*8 + 6] = x - 0.125;
            cubeMesh.texcoords[i*8 + 7] = y + 0.25;
            i++;
        }
    }
    Model cubeModel = LoadModelFromMesh(CopyMesh(cubeMesh));
    UnloadMesh(cubeMesh);

    Texture2D texture = LoadTexture("data/GUIAssets/dicefaces.png");
    cubeModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
    return cubeModel;
}

int main() {
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
    SetTargetFPS(120);               
    int cameraRotationDegree = 0;

    float cubeEdges = 2.0f;
    
    
    


    Vector3 cubeOrigin = { 20.0f, 7.0f, 0.0f };
    BoundingBox cubeBoundingBox = (BoundingBox) {Vector3SubtractValue(cubeOrigin, cubeEdges/2), 
                                                    Vector3AddValue(cubeOrigin, cubeEdges/2)};
    Model diceModel = createDiceModel(cubeEdges);
    PhysicsObject cube(View3D(cubeOrigin, diceModel, WHITE), 
                       0.05f, 0.65f, pow(cubeEdges,2)/6);
        
    
    PhysicsEngine physicsEngine(9.80655, 0.5, 0, 120.0f);
    cube.applyInitialMomentum({ -2.0f, -1, 1.0f }, { 0, -cubeEdges/2, -cubeEdges/2 }, 0.2);
    cout<<cube.getAngularVelocity()<<endl;
    physicsEngine.addObject("cube1", cube);
    printVector3(physicsEngine.getObject("cube1")->getLinearVelocity());
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
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
        if (!physicsEngine.hasCubeObjectStopped("cube1")) {
            if (physicsEngine.isObjectCollidingFloor("cube1")) {
                physicsEngine.handleObjectFloorCollision("cube1");
            } else {
                physicsEngine.getObject("cube1")->setObjectTouchingSurface(false);
                physicsEngine.getObject("cube1")->updateObject(physicsEngine.getLowestDeltaTime());
            }
        }
        BeginDrawing();

            ClearBackground(RAYWHITE);

            BeginMode3D(camera);

                physicsEngine.getObject("cube1")->render();
                DrawGrid(50, 1.0f);
            EndMode3D();

            DrawText("Welcome to the third dimension!", 10, 40, 20, DARKGRAY);
            DrawFPS(10, 10);

        EndDrawing();
    }
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
    return 0;
}