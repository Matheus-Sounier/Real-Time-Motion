cmake_minimum_required(VERSION 3.10)

project(JumpKingIRL)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)

include_directories(
    ${OpenCV_INCLUDE_DIRS}

    Detection
    Detection/Var

    Graphics
    Graphics/Var

    IO
    IO/Var

    MapKeys
)

add_executable(JumpKingIRL

    main.cpp

    Detection/DetectionFunctions.cpp
    Detection/Var/DetectionValues.cpp

    Graphics/GraphicsFunctions.cpp
    Graphics/Var/GraphicsValues.cpp

    IO/IOFunctions.cpp
    IO/Var/IOValues.cpp

    MapKeys/MapKeys.cpp
)

target_link_libraries(
    JumpKingIRL
    ${OpenCV_LIBS}
)