# BaldLion Engine

![Product Name Screen Shot][product-screenshot]

<!-- ABOUT THE PROJECT -->
## About The Project

Greetings, fellow visitor.

BaldLionEngine is a work-in-progress game engine built for the sole purpose of learning C++. Hence, the code that you will find here is far from perfect, and the way some challenges were approached might not be the most optimal, but I have learnt a lot in the process, and therefore I consider that a victory :)

## Implemented systems so far

* 3D Render System
* ECS Gameplay architecture
* Resource Management
* Navigation System
* Locomotion System
* Animation System
* 3D Physics System
* Job System & Thread Pool
* Memory Manager
* Editor with Scene Serialziation support

(The following graph is outdated, but it helps to get the idea of how the architecture is implemented)
![Engine Architecture][architecture]

## Plans for the near future (non specific order)

* Improve compiling times
* Improve NavigationMesh generation
* Clean up Third Parties folder structure
* Overhaul renderer
* Implement support for some scripting language

<!-- GETTING STARTED -->
## Getting Started

### Installation

The project has been tested to compile in Visual Studio 2022

1. Clone the repo
   ```sh
   git clone https://github.com/Efore/BaldLionEngine
   git submodule init
   git submodule update
   ```
2. After cloning is finished, click on "GenerateProjects.bat"
3. After generation is finished, open the .sln file and build the Solution (you may need to retarget it).

## Third Party Libraries

* [Dear ImGui](https://github.com/ocornut/imgui)
* [ImGuizmo](https://github.com/CedricGuillemet/ImGuizmo)
* [debug-draw](https://github.com/glampert/debug-draw)
* [glm](https://github.com/g-truc/glm)
* [Glad](https://glad.dav1d.de/)
* [GLFW](https://github.com/glfw/glfw)
* [YAML](https://github.com/jbeder/yaml-cpp)
* [Optick](https://github.com/bombomby/optick)
* [ReactPhysics3D](https://github.com/DanielChappuis/reactphysics3d)
* [spdlog](https://github.com/gabime/spdlog)
* [stb_image](https://github.com/nothings/stb)
* [assimp](https://github.com/assimp/assimp)
* [recast](https://github.com/benjamn/recast)


[product-screenshot]: Docs/screenshot.png
[architecture]: Docs/architecture.png
