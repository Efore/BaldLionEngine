#pragma once

//For use by BaldLion application

#include "BaldLion/Core/Application.h"
#include "BaldLion/Core/GameStateLayer.h"
#include "BaldLion/Core/Log.h"
#include "BaldLion/Core/Time.h"
#include "BaldLion/Core/Input.h"
#include "BaldLion/Core/BLKeyCodes.h"
#include "BaldLion/Core/MouseButtonCodes.h"
#include "BaldLion/Core/EventManager.h"
#include "BaldLion/ImGui/ImGuiLayer.h"

// --------------- Renderer -----------------------

#include "BaldLion/Rendering/Renderer.h"
#include "BaldLion/Rendering/Framebuffer.h"
#include "BaldLion/Rendering/Shapes/PlaneMesh.h"
#include "BaldLion/Rendering/Shapes/CubeMesh.h"
#include "BaldLion/Rendering/Shapes/SphereMesh.h"


// --------------- Animation -----------------------
#include "BaldLion/Animation/AnimationManager.h"

//---------------- Memory ---------------------------

#include "BaldLion/Core/Memory/MemoryManager.h"

//---------------- Containers ------------------

#include "BaldLion/Core/Containers/DynamicArray.h"
#include "BaldLion/Core/Containers/HashTable.h"
#include "BaldLion/Core/Containers/HashMap.h"
#include "BaldLion/Core/Containers/Heap.h"

//---------------- Threading ------------------

#include "BaldLion/Core/Threading/TaskScheduler.h"

//---------------- ECS ------------------

#include "BaldLion/ECS/ECSManager.h"
#include "BaldLion/ECS/ECSSystemsInclude.h"
#include "BaldLion/ECS/ECSComponentsInclude.h"
#include "BaldLion/ECS/SingletonSystems/CameraSystem.h"
#include "BaldLion/ECS/SingletonSystems/LightningSystem.h"

#include "BaldLion/SceneManagement/SceneManager.h"

//---------------- Others ------------------

#include "BaldLion/Physics/PhysicsManager.h"

#include "BaldLion/Utils/MathUtils.h"

using namespace BaldLion::Rendering;