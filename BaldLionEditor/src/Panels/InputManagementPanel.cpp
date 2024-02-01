#pragma once
#include <BaldLion.h>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include "InputManagementPanel.h"
#include "BaldLion/Core/Input.h"
#include "EditorUtils.h"

namespace BaldLion {

	namespace Editor {

		void InputManagementPanel::OnImGuiRender(float deltaTime)
		{			
			const char* inputSource[] = {
					"Mouse",
					"Keyboard",
					"Controller"
			};

			const char* inputType[] = {
				"Button",
				"Axis1D",
				"Axis2D"
			};

			const char* inputCode[] = {
				"NO_INPUT",
				"KEY_SPACE",
				"KEY_APOSTROPHE",
				"KEY_COMMA",
				"KEY_MINUS",
				"KEY_PERIOD",
				"KEY_SLASH",
				"KEY_0",
				"KEY_1",
				"KEY_2",
				"KEY_3",
				"KEY_4",
				"KEY_5",
				"KEY_6",
				"KEY_7",
				"KEY_8",
				"KEY_9",
				"KEY_SEMICOLON",
				"KEY_EQUAL",
				"KEY_A",
				"KEY_B",
				"KEY_C",
				"KEY_D",
				"KEY_E",
				"KEY_F",
				"KEY_G",
				"KEY_H",
				"KEY_I",
				"KEY_J",
				"KEY_K",
				"KEY_L",
				"KEY_M",
				"KEY_N",
				"KEY_O",
				"KEY_P",
				"KEY_Q",
				"KEY_R",
				"KEY_S",
				"KEY_T",
				"KEY_U",
				"KEY_V",
				"KEY_W",
				"KEY_X",
				"KEY_Y",
				"KEY_Z",
				"KEY_LEFT_BRACKET",
				"KEY_BACKSLASH",
				"KEY_RIGHT_BRACKET",
				"KEY_GRAVE_ACCENT",
				"KEY_WORLD_1",
				"KEY_WORLD_2",
				"KEY_ESCAPE",
				"KEY_ENTER",
				"KEY_TAB",
				"KEY_BACKSPACE",
				"KEY_INSERT",
				"KEY_DELETE",
				"KEY_RIGHT",
				"KEY_LEFT",
				"KEY_DOWN",
				"KEY_UP",
				"KEY_PAGE_UP",
				"KEY_PAGE_DOWN",
				"KEY_HOME",
				"KEY_END",
				"KEY_CAPS_LOCK",
				"KEY_SCROLL_LOCK",
				"KEY_NUM_LOCK",
				"KEY_PRINT_SCREEN",
				"KEY_PAUSE",
				"KEY_F1",
				"KEY_F2",
				"KEY_F3",
				"KEY_F4",
				"KEY_F5",
				"KEY_F6",
				"KEY_F7",
				"KEY_F8",
				"KEY_F9",
				"KEY_F10",
				"KEY_F11",
				"KEY_F12",
				"KEY_F13",
				"KEY_F14",
				"KEY_F15",
				"KEY_F16",
				"KEY_F17",
				"KEY_F18",
				"KEY_F19",
				"KEY_F20",
				"KEY_F21",
				"KEY_F22",
				"KEY_F23",
				"KEY_F24",
				"KEY_F25",
				"KEY_KP_0",
				"KEY_KP_1",
				"KEY_KP_2",
				"KEY_KP_3",
				"KEY_KP_4",
				"KEY_KP_5",
				"KEY_KP_6",
				"KEY_KP_7",
				"KEY_KP_8",
				"KEY_KP_9",
				"KEY_KP_DECIMAL",
				"KEY_KP_DIVIDE",
				"KEY_KP_MULTIPLY",
				"KEY_KP_SUBTRACT",
				"KEY_KP_ADD",
				"KEY_KP_ENTER",
				"KEY_KP_EQUAL",
				"KEY_LEFT_SHIFT",
				"KEY_LEFT_CONTROL",
				"KEY_LEFT_ALT",
				"KEY_LEFT_SUPER",
				"KEY_RIGHT_SHIFT",
				"KEY_RIGHT_CONTROL",
				"KEY_RIGHT_ALT",
				"KEY_RIGHT_SUPER",
				"KEY_MENU",
				"MOUSE_POS",
				"MOUSE_BUTTON_1",
				"MOUSE_BUTTON_2",
				"MOUSE_BUTTON_3",
				"MOUSE_BUTTON_4",
				"MOUSE_BUTTON_5",
				"MOUSE_BUTTON_6",
				"MOUSE_BUTTON_7",
				"MOUSE_BUTTON_8",
				"JOYSTICK_1",
				"JOYSTICK_2",
				"JOYSTICK_3",
				"JOYSTICK_4",
				"JOYSTICK_5",
				"JOYSTICK_6",
				"JOYSTICK_7",
				"JOYSTICK_8",
				"JOYSTICK_9",
				"JOYSTICK_10",
				"JOYSTICK_11",
				"JOYSTICK_12",
				"JOYSTICK_13",
				"JOYSTICK_14",
				"JOYSTICK_15",
				"JOYSTICK_16",
				"GAMEPAD_BUTTON_A",
				"GAMEPAD_BUTTON_B",
				"GAMEPAD_BUTTON_X",
				"GAMEPAD_BUTTON_Y",
				"GAMEPAD_BUTTON_LEFT_BUMPER",
				"GAMEPAD_BUTTON_RIGHT_BUMPER",
				"GAMEPAD_BUTTON_BACK",
				"GAMEPAD_BUTTON_START",
				"GAMEPAD_BUTTON_GUIDE",
				"GAMEPAD_BUTTON_LEFT_THUMB",
				"GAMEPAD_BUTTON_RIGHT_THUMB",
				"GAMEPAD_BUTTON_DPAD_UP",
				"GAMEPAD_BUTTON_DPAD_RIGHT",
				"GAMEPAD_BUTTON_DPAD_DOWN",
				"GAMEPAD_BUTTON_DPAD_LEFT",
				"GAMEPAD_AXIS_LEFT_X",
				"GAMEPAD_AXIS_LEFT_Y",
				"GAMEPAD_AXIS_RIGHT_X",
				"GAMEPAD_AXIS_RIGHT_Y",
				"GAMEPAD_AXIS_LEFT_TRIGGER",
				"GAMEPAD_AXIS_RIGHT_TRIGGER"
			};
			
			ImGui::Begin(BL_STRINGID_TO_STR_C(m_panelName));

			m_panelID = ImGui::GetCurrentWindow()->ID;
			
			if (ImGui::Button("New Input Entry"))
			{
				ImGui::OpenPopup("Create Input Entry");
			}

			if (ImGui::BeginPopupModal("Create Input Entry", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
			{
				static Input::InputSystem::InputEntry entry;

				static char entryName[64] = "";
				BL_IMGUI_LEFT_LABEL(ImGui::InputText, "Action Name", entryName, 64);

				int inputSourceIndex = (int)entry.inputSource;
				if (ImGui::Combo("Input Source", &inputSourceIndex, inputSource, 3))
				{
					entry.inputSource = (Input::InputSystem::InputSource)inputSourceIndex;
				}

				int inputTypeIndex = (int)entry.inputType;
				if (ImGui::Combo("Input Type", &inputTypeIndex, inputType, 3))
				{
					entry.inputType = (Input::InputSystem::InputType)inputTypeIndex;
				}

				int inputCode0Index = (int)entry.inputCodes[0];
				if (ImGui::Combo("Input Code 0", &inputCode0Index, inputCode, 167))
				{
					entry.inputCodes[0] = (Input::InputSystem::InputCode)inputCode0Index;
				}

				int inputCode1Index = (int)entry.inputCodes[1];
				if (ImGui::Combo("Input Code 1", &inputCode1Index, inputCode, 167))
				{
					entry.inputCodes[1] = (Input::InputSystem::InputCode)inputCode1Index;
				}

				int inputCode2Index = (int)entry.inputCodes[2];
				if (ImGui::Combo("Input Code 2", &inputCode2Index, inputCode, 167))
				{
					entry.inputCodes[2] = (Input::InputSystem::InputCode)inputCode2Index;
				}

				int inputCode3Index = (int)entry.inputCodes[3];
				if (ImGui::Combo("Input Code 3", &inputCode3Index, inputCode, 167))
				{
					entry.inputCodes[3] = (Input::InputSystem::InputCode)inputCode3Index;
				}

				static float deadZone = 0.0f;
				BL_IMGUI_LEFT_LABEL(ImGui::InputFloat, "Dead Zone", &deadZone);
				entry.deadZone = deadZone;
				
				if (ImGui::Button("Add Entry"))
				{
					Input::InputSystem::AddInputEntry(BL_STRING_TO_STRINGID(entryName), entry);

					memset(entryName, 0, sizeof(entryName));
					entry.inputCodes[0] = Input::InputSystem::InputCode::NO_INPUT;
					entry.inputCodes[1] = Input::InputSystem::InputCode::NO_INPUT;
					entry.inputCodes[2] = Input::InputSystem::InputCode::NO_INPUT;
					entry.inputCodes[3] = Input::InputSystem::InputCode::NO_INPUT;
					entry.deadZone = 0.0f;
					entry.inputType = Input::InputSystem::InputType::Axis1D;
					entry.inputSource = Input::InputSystem::InputSource::Keyboard;

					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("Cancel"))
				{
					memset(entryName, 0, sizeof(entryName));
					entry.inputCodes[0] = Input::InputSystem::InputCode::NO_INPUT;
					entry.inputCodes[1] = Input::InputSystem::InputCode::NO_INPUT;
					entry.inputCodes[2] = Input::InputSystem::InputCode::NO_INPUT;
					entry.inputCodes[3] = Input::InputSystem::InputCode::NO_INPUT;
					entry.deadZone = 0.0f;
					entry.inputType = Input::InputSystem::InputType::Axis1D;
					entry.inputSource = Input::InputSystem::InputSource::Keyboard;

					ImGui::CloseCurrentPopup();
				}


				ImGui::EndPopup();
			}

			ImGui::Separator();
			ImGui::Separator();

			auto& inputActions = Input::InputSystem::GetInputActions();
			ui32 imguiID = 0;
			ImGui::PushID(imguiID);
			BL_HASHTABLE_FOR(inputActions, it)
			{			
				ImGui::Text("ACTION: %s", BL_STRINGID_TO_STR_C(it.GetKey())); 
				ImGui::SameLine();
				if (ImGui::Button("X"))
				{					
					inputActions.Remove(it.GetKey());
					break;
				}

				auto& inputEntries = it.GetValue();
				
				++imguiID;
				ImGui::PushID(imguiID);
				BL_DYNAMICARRAY_FOREACH(inputEntries)
				{
					ImGui::PushID(i);
					Input::InputSystem::InputEntry& entry = inputEntries[i];
					
					int inputSourceIndex = (int)entry.inputSource;					
					if (ImGui::Combo("Input Source", &inputSourceIndex, inputSource, 3))
					{
						entry.inputSource = (Input::InputSystem::InputSource)inputSourceIndex;
					}

					int inputTypeIndex = (int)entry.inputType;					
					if (ImGui::Combo("Input Type", &inputTypeIndex, inputType, 3))
					{
						entry.inputType = (Input::InputSystem::InputType)inputTypeIndex;
					}

					int inputCode0Index = (int)entry.inputCodes[0];					
					if (ImGui::Combo("Input Code 0", &inputCode0Index, inputCode, 167))
					{
						entry.inputCodes[0] = (Input::InputSystem::InputCode)inputCode0Index;
					}

					int inputCode1Index = (int)entry.inputCodes[1];
					if (ImGui::Combo("Input Code 1", &inputCode1Index, inputCode, 167))
					{
						entry.inputCodes[1] = (Input::InputSystem::InputCode)inputCode1Index;
					}

					int inputCode2Index = (int)entry.inputCodes[2];
					if (ImGui::Combo("Input Code 2", &inputCode2Index, inputCode, 167))
					{
						entry.inputCodes[2] = (Input::InputSystem::InputCode)inputCode2Index;
					}

					int inputCode3Index = (int)entry.inputCodes[3];
					if (ImGui::Combo("Input Code 3", &inputCode3Index, inputCode, 167))
					{
						entry.inputCodes[3] = (Input::InputSystem::InputCode)inputCode3Index;
					}

					float deadZone = entry.deadZone;
					BL_IMGUI_LEFT_LABEL(ImGui::InputFloat, "Dead Zone", &deadZone);
					entry.deadZone = deadZone;

					if (ImGui::Button("X"))
					{
						inputEntries.RemoveAt(i);
						ImGui::PopID();
						break;
					}

					ImGui::PopID();
					ImGui::Separator();
				}
				ImGui::PopID();


				ImGui::Separator();				
			}
			ImGui::PopID();

			if (ImGui::Button("Save Input entries"))
			{
				Input::InputSystem::SerializeInputEntries();
			}


			ImGui::End();
		}
	}
}