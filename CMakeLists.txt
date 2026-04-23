cmake_minimum_required(VERSION 3.11)
project(Nimonspoli LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(raylib REQUIRED)

add_executable(${PROJECT_NAME} src/core/Game.cpp
                               src/core/main.cpp
                               src/core/GameEngine.cpp
                               src/views/GUI.cpp
                               src/views/viewElement/View2D.cpp
                               src/views/viewElement/View3D.cpp
                               src/views/viewElement/Popup.cpp
                               src/views/viewElement/Interactable.cpp
                               src/views/viewElement/Entry.cpp
                               src/views/viewElement/MenuView.cpp
                               src/views/viewElement/GameHUDView.cpp
                               src/views/viewElement/board/TileView.cpp
                               src/views/viewElement/board/BoardView.cpp
                               src/views/viewElement/player/PlayerView.cpp
                               src/views/viewElement/player/PlayerProfileView.cpp
                               src/views/viewElement/player/PlayerInventoryPopup.cpp
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
                               src/exception/PlayerTurnException.cpp
                               src/exception/InvalidEntryInputException.cpp
                               src/exception/InvalidFileException.cpp
                               src/exception/PlayerTurn/PropertyManagementException.cpp
                               src/exception/PlayerTurn/PropertyManagement/PropertyNotMortgagedException.cpp
                               src/exception/PlayerTurn/PropertyManagement/InsufficientOwnedColorException.cpp
                               src/exception/PlayerTurn/PropertyManagement/InsufficientMoneyException.cpp
                               src/exception/PlayerTurn/PropertyManagement/ExistingHotelException.cpp
                               src/exception/PlayerTurn/InvalidTurnException.cpp
                               src/exception/PlayerTurn/SkillTurnException.cpp
                               src/exception/PlayerTurn/SkillTurn/DiceAlreadyRolledException.cpp
                               src/exception/PlayerTurn/SkillTurn/SkillCardUsedException.cpp
                               src/exception/InvalidEntryInput/InvalidDiceNumberException.cpp
                               src/exception/InvalidEntryInput/InvalidTileException.cpp
                               src/exception/InvalidFile/FailedToSaveException.cpp
                               src/exception/InvalidFile/FileNotFoundException.cpp
                               src/exception/InvalidFile/InvalidConfigException.cpp
                               src/exception/InvalidFile/UnreadableFileException.cpp
                               src/utils/data/ConfigLoader.cpp
                               src/utils/data/LogEntry.cpp
                               src/utils/data/TransactionLogger.cpp
                               )

target_link_libraries(${PROJECT_NAME} raylib)
target_include_directories(${PROJECT_NAME} PUBLIC include)