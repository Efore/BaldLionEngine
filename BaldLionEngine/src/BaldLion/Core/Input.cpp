
#include "blpch.h"

#include "Input.h"
#include <yaml-cpp/yaml.h>



namespace BaldLion
{
	namespace Input
	{
		#define YAML_KEY_INPUT_SYSTEM		"InputSystem"
		#define YAML_KEY_ACTION_NAME		"ActionName"
		#define YAML_KEY_ACTION_BINDINGS	"ActionBindings"
		#define YAML_KEY_INPUT_TYPE			"InputType"
		#define YAML_KEY_INPUT_SOURCE		"InputSource"
		#define YAML_KEY_INPUT_CODE_0		"InputCode0"
		#define YAML_KEY_INPUT_CODE_1		"InputCode1"
		#define YAML_KEY_INPUT_CODE_2		"InputCode2"
		#define YAML_KEY_INPUT_CODE_3		"InputCode3"
		#define YAML_KEY_INPUT_DEADZONE		"InputDeadzone"

		#define INPUT_BINDINGS_PATH "assets/input/inputBindings.meta"

		HashTable<StringId, DynamicArray<InputSystem::InputEntry>> InputSystem::s_actionBindings;

		bool InputSystem::s_initialized = false;
		bool InputSystem::s_gamepadIsConected = false;

		InputSystem::InputCode InputSystem::s_keyboardInputs[349] =
		{
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::KEY_SPACE,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::KEY_APOSTROPHE,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::KEY_COMMA,
				InputCode::KEY_MINUS,
				InputCode::KEY_PERIOD,
				InputCode::KEY_SLASH,
				InputCode::KEY_0,
				InputCode::KEY_1,
				InputCode::KEY_2,
				InputCode::KEY_3,
				InputCode::KEY_4,
				InputCode::KEY_5,
				InputCode::KEY_6,
				InputCode::KEY_7,
				InputCode::KEY_8,
				InputCode::KEY_9,
				InputCode::NO_INPUT,
				InputCode::KEY_SEMICOLON,
				InputCode::NO_INPUT,
				InputCode::KEY_EQUAL,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::KEY_A,
				InputCode::KEY_B,
				InputCode::KEY_C,
				InputCode::KEY_D,
				InputCode::KEY_E,
				InputCode::KEY_F,
				InputCode::KEY_G,
				InputCode::KEY_H,
				InputCode::KEY_I,
				InputCode::KEY_J,
				InputCode::KEY_K,
				InputCode::KEY_L,
				InputCode::KEY_M,
				InputCode::KEY_N,
				InputCode::KEY_O,
				InputCode::KEY_P,
				InputCode::KEY_Q,
				InputCode::KEY_R,
				InputCode::KEY_S,
				InputCode::KEY_T,
				InputCode::KEY_U,
				InputCode::KEY_V,
				InputCode::KEY_W,
				InputCode::KEY_X,
				InputCode::KEY_Y,
				InputCode::KEY_Z,
				InputCode::KEY_LEFT_BRACKET,
				InputCode::KEY_BACKSLASH,
				InputCode::KEY_RIGHT_BRACKET,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::KEY_GRAVE_ACCENT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::KEY_WORLD_1,
				InputCode::KEY_WORLD_2,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::KEY_ESCAPE,
				InputCode::KEY_ENTER,
				InputCode::KEY_TAB,
				InputCode::KEY_BACKSPACE,
				InputCode::KEY_INSERT,
				InputCode::KEY_DELETE,
				InputCode::KEY_RIGHT,
				InputCode::KEY_LEFT,
				InputCode::KEY_DOWN,
				InputCode::KEY_UP,
				InputCode::KEY_PAGE_UP,
				InputCode::KEY_PAGE_DOWN,
				InputCode::KEY_HOME,
				InputCode::KEY_END,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::KEY_CAPS_LOCK,
				InputCode::KEY_SCROLL_LOCK,
				InputCode::KEY_NUM_LOCK,
				InputCode::KEY_PRINT_SCREEN,
				InputCode::KEY_PAUSE,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::KEY_F1,
				InputCode::KEY_F2,
				InputCode::KEY_F3,
				InputCode::KEY_F4,
				InputCode::KEY_F5,
				InputCode::KEY_F6,
				InputCode::KEY_F7,
				InputCode::KEY_F8,
				InputCode::KEY_F9,
				InputCode::KEY_F10,
				InputCode::KEY_F11,
				InputCode::KEY_F12,
				InputCode::KEY_F13,
				InputCode::KEY_F14,
				InputCode::KEY_F15,
				InputCode::KEY_F16,
				InputCode::KEY_F17,
				InputCode::KEY_F18,
				InputCode::KEY_F19,
				InputCode::KEY_F20,
				InputCode::KEY_F21,
				InputCode::KEY_F22,
				InputCode::KEY_F23,
				InputCode::KEY_F24,
				InputCode::KEY_F25,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::KEY_KP_0,
				InputCode::KEY_KP_1,
				InputCode::KEY_KP_2,
				InputCode::KEY_KP_3,
				InputCode::KEY_KP_4,
				InputCode::KEY_KP_5,
				InputCode::KEY_KP_6,
				InputCode::KEY_KP_7,
				InputCode::KEY_KP_8,
				InputCode::KEY_KP_9,
				InputCode::KEY_KP_DECIMAL,
				InputCode::KEY_KP_DIVIDE,
				InputCode::KEY_KP_MULTIPLY,
				InputCode::KEY_KP_SUBTRACT,
				InputCode::KEY_KP_ADD,
				InputCode::KEY_KP_ENTER,
				InputCode::KEY_KP_EQUAL,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::NO_INPUT,
				InputCode::KEY_LEFT_SHIFT,
				InputCode::KEY_LEFT_CONTROL,
				InputCode::KEY_LEFT_ALT,
				InputCode::KEY_LEFT_SUPER,
				InputCode::KEY_RIGHT_SHIFT,
				InputCode::KEY_RIGHT_CONTROL,
				InputCode::KEY_RIGHT_ALT,
				InputCode::KEY_RIGHT_SUPER,
				InputCode::KEY_MENU
		};

		InputSystem::InputCode InputSystem::s_gamepadInputAxes[6] = 
		{
			InputCode::GAMEPAD_AXIS_LEFT_X,
			InputCode::GAMEPAD_AXIS_LEFT_Y,
			InputCode::GAMEPAD_AXIS_RIGHT_X,
			InputCode::GAMEPAD_AXIS_RIGHT_Y,
			InputCode::GAMEPAD_AXIS_LEFT_TRIGGER,
			InputCode::GAMEPAD_AXIS_RIGHT_TRIGGER
		};

		InputSystem::InputCode InputSystem::s_gamepadInputButtons[15] =
		{
			InputCode::GAMEPAD_BUTTON_A,
			InputCode::GAMEPAD_BUTTON_B,
			InputCode::GAMEPAD_BUTTON_X,
			InputCode::GAMEPAD_BUTTON_Y,
			InputCode::GAMEPAD_BUTTON_LEFT_BUMPER,
			InputCode::GAMEPAD_BUTTON_RIGHT_BUMPER,
			InputCode::GAMEPAD_BUTTON_BACK,
			InputCode::GAMEPAD_BUTTON_START,
			InputCode::GAMEPAD_BUTTON_GUIDE,
			InputCode::GAMEPAD_BUTTON_LEFT_THUMB,
			InputCode::GAMEPAD_BUTTON_RIGHT_THUMB,
			InputCode::GAMEPAD_BUTTON_DPAD_UP,
			InputCode::GAMEPAD_BUTTON_DPAD_RIGHT,
			InputCode::GAMEPAD_BUTTON_DPAD_DOWN,
			InputCode::GAMEPAD_BUTTON_DPAD_LEFT
		};

		InputSystem::InputCode InputSystem::s_mouseInputs[8] = 
		{
			InputCode::MOUSE_BUTTON_1,
			InputCode::MOUSE_BUTTON_2,
			InputCode::MOUSE_BUTTON_3,
			InputCode::MOUSE_BUTTON_4,
			InputCode::MOUSE_BUTTON_5,
			InputCode::MOUSE_BUTTON_6,
			InputCode::MOUSE_BUTTON_7,
			InputCode::MOUSE_BUTTON_8
		};

		void InputSystem::Init()
		{			
			s_actionBindings = HashTable<StringId, DynamicArray<InputEntry>>(Memory::AllocationType::FreeList_Main, 64);
			s_initialized = true;
			s_gamepadIsConected = PlatformInput::GetGamepadIsConnected(0);

			DeserializeInputEntries();
		}

		InputSystem::InputValue InputSystem::GetActionValue(StringId inputAction)
		{
			InputValue resultValue;

			memset(&resultValue, 0, sizeof(InputValue));

			DynamicArray <InputEntry>* entriesBindedToAction = nullptr;

			if (!s_actionBindings.TryGet(inputAction, entriesBindedToAction))
			{
				return resultValue;
			}

			BL_DYNAMICARRAY_FOR(i, (*entriesBindedToAction), 0)
			{
				InputEntry& entry = (*entriesBindedToAction)[i];				

				if (memcmp(&entry.currentValue, &resultValue, sizeof(InputValue)) != 0)
				{
					return entry.currentValue;
				}
			}

			return resultValue;
		}

		void InputSystem::UpdateEntries()
		{
			BL_HASHTABLE_FOR(s_actionBindings, it)
			{
				DynamicArray<InputSystem::InputEntry>& entries = it.GetValue();
				BL_DYNAMICARRAY_FOREACH(entries)
				{
					InputEntry& entry = (entries)[i];					

					ui32 blKeyCode = 0;

					switch (entry.inputSource)
					{
					case InputSource::Mouse:
					{
						switch (entry.inputType)
						{
						case InputType::Button:
							blKeyCode = InputCodeToBlKeyCode(entry.inputSource, entry.inputType, entry.inputCodes[0]);
							entry.currentValue.valueButton = PlatformInput::IsMouseButtonPress(blKeyCode);
							break;
						case InputType::Axis1D:
							blKeyCode = InputCodeToBlKeyCode(entry.inputSource, entry.inputType, entry.inputCodes[0]);
							entry.currentValue.value1D = PlatformInput::IsMouseButtonPress(blKeyCode) ? 1.0f : 0.0f;
							break;
						case InputType::Axis2D:
							entry.currentValue.value2D.x = PlatformInput::GetMouseX();
							entry.currentValue.value2D.y = PlatformInput::GetMouseY();
							break;
						}
					}
					break;
					case InputSource::Keyboard:
					{
						switch (entry.inputType)
						{
						case InputType::Button:
							blKeyCode = InputCodeToBlKeyCode(entry.inputSource, entry.inputType, entry.inputCodes[0]);
							entry.currentValue.valueButton = PlatformInput::IsKeyPressed(blKeyCode);
							break;

						case InputType::Axis1D:
							blKeyCode = InputCodeToBlKeyCode(entry.inputSource, entry.inputType, entry.inputCodes[0]);
							entry.currentValue.value1D = PlatformInput::IsKeyPressed(blKeyCode) ? 1.0f : 0.0f;
							break;

						case InputType::Axis2D:
						{
							ui32 blKeyCode0 = InputCodeToBlKeyCode(entry.inputSource, entry.inputType, entry.inputCodes[0]);
							ui32 blKeyCode1 = InputCodeToBlKeyCode(entry.inputSource, entry.inputType, entry.inputCodes[1]);
							ui32 blKeyCode2 = InputCodeToBlKeyCode(entry.inputSource, entry.inputType, entry.inputCodes[2]);
							ui32 blKeyCode3 = InputCodeToBlKeyCode(entry.inputSource, entry.inputType, entry.inputCodes[3]);

							entry.currentValue.value2D.x = PlatformInput::IsKeyPressed(blKeyCode0) ? 1.0f : (PlatformInput::IsKeyPressed(blKeyCode1) ? -1.0f : 0.0f);
							entry.currentValue.value2D.y = PlatformInput::IsKeyPressed(blKeyCode2) ? 1.0f : (PlatformInput::IsKeyPressed(blKeyCode3) ? -1.0f : 0.0f);
						}
						break;
						}
					}
					break;
					case InputSource::Controller:
					{
						if (s_gamepadIsConected)
						{
							blKeyCode = InputCodeToBlKeyCode(entry.inputSource, entry.inputType, entry.inputCodes[0]);
							entry.previousValue = entry.currentValue;

							switch (entry.inputType)
							{
							case InputType::Button:
								entry.currentValue.valueButton = PlatformInput::GetGamepadButtonValue(0, blKeyCode);
								break;
							case InputType::Axis1D:
								entry.currentValue.value1D = PlatformInput::GetGamepadAxisValue(0, blKeyCode);
								break;
							case InputType::Axis2D:
							{
								ui32 blKeyCodeY = InputCodeToBlKeyCode(entry.inputSource, entry.inputType, entry.inputCodes[1]);

								float valueX = PlatformInput::GetGamepadAxisValue(0, blKeyCode);
								float valueY = PlatformInput::GetGamepadAxisValue(0, blKeyCodeY);

								entry.currentValue.value2D.x = abs(valueX) > entry.deadZone ? valueX : 0.0f;
								entry.currentValue.value2D.y = abs(valueY) > entry.deadZone ? valueY : 0.0f;
							}
							break;
							default:
								break;
							}
						}
						else
						{
							memset(&entry.currentValue, 0, sizeof(InputValue));
						}
					}
					break;

					default:
						break;

					}
				}
			}
		}

		void InputSystem::Stop()
		{			
			s_initialized = false;
			s_actionBindings.Delete();
		}

		InputSystem::InputEntry* InputSystem::FindEntryByActionAndInputCode(StringId inputAction, InputCode inputCode, ui32& inputCodeIndex)
		{		
			InputEntry* resultEntry = nullptr;

			DynamicArray <InputEntry>* entriesBindedToAction = nullptr;
			if (s_actionBindings.TryGet(inputAction, entriesBindedToAction))
			{
				BL_DYNAMICARRAY_FOREACH(*entriesBindedToAction)
				{
					resultEntry = &(*entriesBindedToAction)[i];

					for (ui32 i = 0; i < 4; ++i)
					{
						if (resultEntry->inputCodes[i] == inputCode)
						{
							inputCodeIndex = i;
							return resultEntry;
						}
					}
				}
			}

			return resultEntry;
		}

		ui32 InputSystem::InputCodeToBlKeyCode(InputSource inputSource, InputType inputType, InputCode code)
		{
			switch (inputSource)
			{
			case BaldLion::Input::InputSystem::InputSource::Mouse:
				for (ui32 i = 0; i < 8; ++i)
				{
					if (s_mouseInputs[i] == code)
					{
						return i;
					}
				}
				break;

			case BaldLion::Input::InputSystem::InputSource::Keyboard:
				for (ui32 i = 0; i < 348; ++i)
				{
					if (s_keyboardInputs[i] == code)
					{
						return i;
					}
				}
				break;

			case BaldLion::Input::InputSystem::InputSource::Controller:
				if (inputType == InputType::Button)
				{
					for (ui32 i = 0; i < 15; ++i)
					{
						if (s_gamepadInputButtons[i] == code)
						{
							return i;
						}
					}
				}
				else
				{
					for (ui32 i = 0; i < 6; ++i)
					{
						if (s_gamepadInputAxes[i] == code)
						{
							return i;
						}
					}
				}
				break;

			default:
				break;
			}

			return 0;
		}

		InputSystem::InputCode InputSystem::BlKeyCodeToInputCode(InputSource inputSource, InputType inputType, ui32 blKeyCode)
		{
			switch (inputSource)
			{
			case BaldLion::Input::InputSystem::InputSource::Mouse:
				return s_mouseInputs[blKeyCode];				

			case BaldLion::Input::InputSystem::InputSource::Keyboard:
				return s_keyboardInputs[blKeyCode];

			case BaldLion::Input::InputSystem::InputSource::Controller:
				if (inputType == InputType::Button)
				{
					return s_gamepadInputButtons[blKeyCode];
				}
				else
				{
					return s_gamepadInputAxes[blKeyCode];
				}

			default:
				break;
			}

			return InputCode::NO_INPUT;
		}

		bool InputSystem::GetGamepadIsConnected()
		{
			return s_gamepadIsConected;
		}

		void InputSystem::SetGamepadIsConnected(bool value)
		{
			s_gamepadIsConected = value;			
		}

		HashTable<StringId, DynamicArray<InputSystem::InputEntry>>& InputSystem::GetInputActions()
		{
			return s_actionBindings;
		}

		void InputSystem::AddInputEntry(StringId actionName, const InputEntry& entry)
		{
			DynamicArray <InputEntry>* entriesBindedToAction = nullptr;

			if (s_actionBindings.TryGet(actionName, entriesBindedToAction))
			{
				bool alreadyExists = false;
				BL_DYNAMICARRAY_FOREACH(*entriesBindedToAction)
				{
					if ((*entriesBindedToAction)[i] == entry)
					{
						alreadyExists = true;
						break;
					}
				}

				if (!alreadyExists)
				{
					(*entriesBindedToAction).PushBack(entry);
				}
			}
			else
			{	
				s_actionBindings.Emplace(actionName, AllocationType::FreeList_ECS, (int)InputSource::Count);
				entriesBindedToAction = &s_actionBindings.Get(actionName);
				entriesBindedToAction->PushBack(entry);
			}
		}

		void InputSystem::SerializeInputEntries()
		{
			YAML::Emitter out;
			out << YAML::BeginMap;

			out << YAML::Key << YAML_KEY_INPUT_SYSTEM << YAML::Value << YAML::BeginSeq;
			BL_HASHTABLE_FOR(s_actionBindings, it)
			{
				out << YAML::BeginMap;
				out << YAML::Key << YAML_KEY_ACTION_NAME << YAML::Value << BL_STRINGID_TO_STR_C(it.GetKey());
				out << YAML::Key << YAML_KEY_ACTION_BINDINGS << YAML::Value << YAML::BeginSeq;
				
				DynamicArray<InputSystem::InputEntry>& bindings = it.GetValue();

				BL_DYNAMICARRAY_FOREACH(bindings)
				{
					out << YAML::BeginMap;

					out << YAML::Key << YAML_KEY_INPUT_SOURCE << YAML::Value << (ui32)bindings[i].inputSource;
					out << YAML::Key << YAML_KEY_INPUT_TYPE << YAML::Value << (ui32)bindings[i].inputType;
					out << YAML::Key << YAML_KEY_INPUT_CODE_0 << YAML::Value << (ui32)bindings[i].inputCodes[0];
					out << YAML::Key << YAML_KEY_INPUT_CODE_1 << YAML::Value << (ui32)bindings[i].inputCodes[1];
					out << YAML::Key << YAML_KEY_INPUT_CODE_2 << YAML::Value << (ui32)bindings[i].inputCodes[2];
					out << YAML::Key << YAML_KEY_INPUT_CODE_3 << YAML::Value << (ui32)bindings[i].inputCodes[3];
					out << YAML::Key << YAML_KEY_INPUT_DEADZONE << YAML::Value << (float)bindings[i].deadZone;

					out << YAML::EndMap;
				}

				out << YAML::EndSeq;
				out << YAML::EndMap;
			}
			out << YAML::EndSeq;
			out << YAML::EndMap;

			std::ofstream fout(INPUT_BINDINGS_PATH);
			fout << out.c_str();
			fout.close();
		}

		void InputSystem::DeserializeInputEntries()
		{
			std::ifstream stream(INPUT_BINDINGS_PATH);
			std::stringstream strStream;

			strStream << stream.rdbuf();

			YAML::Node data = YAML::Load(strStream.str());

			if (!data[YAML_KEY_INPUT_SYSTEM])
				return;

			auto actions = data[YAML_KEY_INPUT_SYSTEM];
			
			if (actions)
			{
				for (auto action : actions)
				{
					StringId actionName = BL_STRING_TO_STRINGID(action[YAML_KEY_ACTION_NAME].as<std::string>());
					auto yamlBindings = action[YAML_KEY_ACTION_BINDINGS];

					for (auto binding : yamlBindings)
					{
						InputEntry entry;
						entry.inputSource = (InputSource)binding[YAML_KEY_INPUT_SOURCE].as<ui32>();
						entry.inputType = (InputType)binding[YAML_KEY_INPUT_TYPE].as<ui32>();
						entry.inputCodes[0] = (InputCode)binding[YAML_KEY_INPUT_CODE_0].as<ui32>();
						entry.inputCodes[1] = (InputCode)binding[YAML_KEY_INPUT_CODE_1].as<ui32>();
						entry.inputCodes[2] = (InputCode)binding[YAML_KEY_INPUT_CODE_2].as<ui32>();
						entry.inputCodes[3] = (InputCode)binding[YAML_KEY_INPUT_CODE_3].as<ui32>();
						entry.deadZone = binding[YAML_KEY_INPUT_DEADZONE].as<float>();

						AddInputEntry(actionName, entry);
					}

				}
			}
		}
	}
}