#pragma once

//For use by BaldLion application

#include "BaldLion/Core/Application.h"
#include "BaldLion/Core/Layer.h"
#include "BaldLion/Core/Log.h"

#include "BaldLion/Core/TimeStep.h"

#include "BaldLion/Core/Input.h"
#include "BaldLion/Core/KeyCodes.h"
#include "BaldLion/Core/MouseButtonCodes.h"

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

//---------------- Threading ------------------

#include "BaldLion/Core/JobManagement/JobManager.h"

//---------------- ECS ------------------

#include "BaldLion/ECS/ECSManager.h"
#include "BaldLion/ECS/Systems/ECSCameraMovementSystem.h"
#include "BaldLion/ECS/ComponentsSingleton/ECSProjectionCameraSingleton.h"
#include "BaldLion/ECS/ComponentsSingleton/ECSLightSingleton.h"


//---------------- Others ------------------

#include "BaldLion/Utils/MathUtils.h"

using namespace BaldLion::Rendering;