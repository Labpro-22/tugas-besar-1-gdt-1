cmake_minimum_required(VERSION 3.11)
project(Nimonspoli LANGUAGES CXX)

# Use C++17 or higher (recommended for modern wrappers)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# 1. Declare raylib dependency
FetchContent_Declare(
  raylib
  GIT_REPOSITORY https://github.com/raysan5/raylib.git
  GIT_TAG        master # Or a specific version like 5.0
)

# 2. Make raylib available to the project
FetchContent_MakeAvailable(raylib)

# 3. Create your executable
# add_executable(${PROJECT_NAME} src/views/GUITest.cpp)
# add_executable(${PROJECT_NAME} src/views/ThrowDice.cpp 
#                                src/views/physicsEngine/PhysicsObject.cpp
#                                src/views/physicsEngine/PhysicsEngine.cpp
#                                src/views/viewElement/View3D.cpp)
add_executable(${PROJECT_NAME} src/views/ViewTesting.cpp
                               src/views/GUI.cpp
                               src/views/viewElement/View2D.cpp
                               src/views/viewElement/Popup.cpp
                               src/views/viewElement/Interactable.cpp
                               src/views/viewElement/Entry.cpp
                               src/views/viewElement/MenuView.cpp
                               src/views/animation/ViewAnimation.cpp
                               src/views/animation/camera/CameraManager.cpp
                               src/views/animation/camera/CameraMovement.cpp
                               src/views/animation/camera/View3DCamera.cpp)

# 4. Link raylib to your executable
target_link_libraries(${PROJECT_NAME} raylib)
target_include_directories(${PROJECT_NAME} PUBLIC include)
