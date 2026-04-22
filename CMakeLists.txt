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
#                                src/views/animation/physicsEngine/PhysicsObject.cpp
#                                src/views/animation/physicsEngine/PhysicsEngine.cpp
#                                src/views/viewElement/View3D.cpp)
add_executable(${PROJECT_NAME} src/views/ViewTesting.cpp
                               src/views/GUI.cpp
                               src/views/viewElement/View2D.cpp
                               src/views/viewElement/View3D.cpp
                               src/views/viewElement/Popup.cpp
                               src/views/viewElement/Interactable.cpp
                               src/views/viewElement/Entry.cpp
                               src/views/viewElement/MenuView.cpp
                               src/views/viewElement/board/TileView.cpp
                               src/views/viewElement/board/BoardView.cpp
                               src/views/viewElement/player/PlayerView.cpp
                               src/views/animation/ViewAnimation.cpp
                               src/views/animation/camera/CameraManager.cpp
                               src/views/animation/camera/CameraMovement.cpp
                               src/views/animation/camera/View3DCamera.cpp
                               
                               src/models/Player/Player.cpp
                               src/models/Property/Property.cpp
                               src/models/Property/StreetProperty.cpp
                               src/models/Property/UtilityProperty.cpp
                               src/models/Property/RailroadProperty.cpp
                               src/models/BoardAndTiles/Tile.cpp
                               src/models/BoardAndTiles/ActionTile.cpp
                               src/models/BoardAndTiles/SpecialTile.cpp
                               src/models/BoardAndTiles/PropertyTile.cpp
                               src/models/BoardAndTiles/Board.cpp
                               src/models/BoardAndTiles/ActionTile/CommunityChestTile.cpp
                               src/models/BoardAndTiles/ActionTile/ChanceTile.cpp
                               src/models/BoardAndTiles/ActionTile/FestivalTile.cpp
                               src/models/BoardAndTiles/ActionTile/TaxTile.cpp
                               src/models/BoardAndTiles/ActionTile/TaxTile/IncomingTaxTile.cpp
                               src/models/BoardAndTiles/ActionTile/TaxTile/LuxuryTaxTile.cpp
                               src/models/BoardAndTiles/SpecialTile/GoTile.cpp
                               src/models/BoardAndTiles/SpecialTile/FreeParkingTile.cpp
                               src/models/BoardAndTiles/SpecialTile/JailTile.cpp
                               src/models/BoardAndTiles/SpecialTile/GoToJailTile.cpp
                               src/models/BoardAndTiles/PropertyTile/StreetTile.cpp
                               src/models/BoardAndTiles/PropertyTile/UtilityTile.cpp
                               src/models/BoardAndTiles/PropertyTile/RailroadTile.cpp
                               src/models/CardAndDeck/ActionCard.cpp
                               src/models/CardAndDeck/Card.cpp
                               src/models/CardAndDeck/ChanceCard.cpp
                               src/models/CardAndDeck/CommunityChestCard.cpp
                               src/models/CardAndDeck/DemolitionCard.cpp
                               src/models/CardAndDeck/DiscountCard.cpp
                               src/models/CardAndDeck/LassoCard.cpp
                               src/models/CardAndDeck/MoveCard.cpp
                               src/models/CardAndDeck/ShieldCard.cpp
                               src/models/CardAndDeck/SkillCard.cpp
                               src/models/CardAndDeck/TeleportCard.cpp
                               src/exception/GameException.cpp
                               src/exception/InvalidEntryInputException.cpp
                               src/exception/InvalidFileException.cpp
                               src/exception/InvalidEntryInput/InvalidTileException.cpp
                               src/exception/InvalidEntryInput/InvalidDiceNumberException.cpp
                               src/exception/InvalidFile/InvalidConfigException.cpp
                               src/utils/data/ConfigLoader.cpp
                               src/core/Game.cpp)

# 4. Link raylib to your executable
target_link_libraries(${PROJECT_NAME} raylib)
target_include_directories(${PROJECT_NAME} PUBLIC include)
