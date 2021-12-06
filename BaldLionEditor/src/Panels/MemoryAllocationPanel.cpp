#pragma once
#include <BaldLion.h>
#include <imgui/imgui.h>
#include "MemoryAllocationPanel.h"


namespace BaldLion {

	namespace Editor {

		void MemoryAllocationPanel::OnImGuiRender()
		{
			ImGui::Begin("Memory");

			ImGui::Text("Total Memory: %zu", MemoryManager::GetMemorySize());
			ImGui::Text("Free List Main Allocator: %zu / %zu", MemoryManager::GetAllocatorUsedMemory(AllocationType::FreeList_Main), MemoryManager::GetAllocatorSize(AllocationType::FreeList_Main));
			ImGui::Text("Free List Renderer Allocator: %zu / %zu", MemoryManager::GetAllocatorUsedMemory(AllocationType::FreeList_Renderer), MemoryManager::GetAllocatorSize(AllocationType::FreeList_Renderer));
			ImGui::Text("Free List ECS Allocator: %zu / %zu", MemoryManager::GetAllocatorUsedMemory(AllocationType::FreeList_ECS), MemoryManager::GetAllocatorSize(AllocationType::FreeList_ECS));
			ImGui::Text("Linear Frame Allocator: %zu / %zu", MemoryManager::GetAllocatorUsedMemory(AllocationType::Linear_Frame), MemoryManager::GetAllocatorSize(AllocationType::Linear_Frame));
			ImGui::Text("Stack Scope Allocator: %zu / %zu", MemoryManager::GetAllocatorUsedMemory(AllocationType::Stack), MemoryManager::GetAllocatorSize(AllocationType::Stack));

			ImGui::End();
		}

	}
}
