#pragma once

#include "BaldLion/Core/Core.h"
#include <glm/glm.hpp>
#include "BaldLion/Core/Containers/HashTable.h"
#include "BaldLion/Core/Containers/DynamicArray.h"
#include "BaldLion/Core/Threading/TaskScheduler.h"

namespace BaldLion
{
	namespace Input
	{		
		class InputSystem
		{

		public:
			union InputValue
			{
				ui32 valueButton;
				float value1D;
				glm::vec2 value2D;
			};

			enum class InputSource : ui8
			{
				Mouse,
				Keyboard,
				Controller,

				Count
			};

			enum class InputType : ui8
			{
				Button,
				Axis1D,
				Axis2D
			};

			enum class InputCode : ui16
			{
				NO_INPUT,

				KEY_SPACE,
				KEY_APOSTROPHE,
				KEY_COMMA,
				KEY_MINUS,
				KEY_PERIOD,
				KEY_SLASH,
				KEY_0,
				KEY_1,
				KEY_2,
				KEY_3,
				KEY_4,
				KEY_5,
				KEY_6,
				KEY_7,
				KEY_8,
				KEY_9,
				KEY_SEMICOLON,
				KEY_EQUAL,
				KEY_A,
				KEY_B,
				KEY_C,
				KEY_D,
				KEY_E,
				KEY_F,
				KEY_G,
				KEY_H,
				KEY_I,
				KEY_J,
				KEY_K,
				KEY_L,
				KEY_M,
				KEY_N,
				KEY_O,
				KEY_P,
				KEY_Q,
				KEY_R,
				KEY_S,
				KEY_T,
				KEY_U,
				KEY_V,
				KEY_W,
				KEY_X,
				KEY_Y,
				KEY_Z,
				KEY_LEFT_BRACKET,
				KEY_BACKSLASH,
				KEY_RIGHT_BRACKET,
				KEY_GRAVE_ACCENT,
				KEY_WORLD_1,
				KEY_WORLD_2,

				KEY_ESCAPE,
				KEY_ENTER,
				KEY_TAB,
				KEY_BACKSPACE,
				KEY_INSERT,
				KEY_DELETE,
				KEY_RIGHT,
				KEY_LEFT,
				KEY_DOWN,
				KEY_UP,
				KEY_PAGE_UP,
				KEY_PAGE_DOWN,
				KEY_HOME,
				KEY_END,
				KEY_CAPS_LOCK,
				KEY_SCROLL_LOCK,
				KEY_NUM_LOCK,
				KEY_PRINT_SCREEN,
				KEY_PAUSE,
				KEY_F1,
				KEY_F2,
				KEY_F3,
				KEY_F4,
				KEY_F5,
				KEY_F6,
				KEY_F7,
				KEY_F8,
				KEY_F9,
				KEY_F10,
				KEY_F11,
				KEY_F12,
				KEY_F13,
				KEY_F14,
				KEY_F15,
				KEY_F16,
				KEY_F17,
				KEY_F18,
				KEY_F19,
				KEY_F20,
				KEY_F21,
				KEY_F22,
				KEY_F23,
				KEY_F24,
				KEY_F25,
				KEY_KP_0,
				KEY_KP_1,
				KEY_KP_2,
				KEY_KP_3,
				KEY_KP_4,
				KEY_KP_5,
				KEY_KP_6,
				KEY_KP_7,
				KEY_KP_8,
				KEY_KP_9,
				KEY_KP_DECIMAL,
				KEY_KP_DIVIDE,
				KEY_KP_MULTIPLY,
				KEY_KP_SUBTRACT,
				KEY_KP_ADD,
				KEY_KP_ENTER,
				KEY_KP_EQUAL,
				KEY_LEFT_SHIFT,
				KEY_LEFT_CONTROL,
				KEY_LEFT_ALT,
				KEY_LEFT_SUPER,
				KEY_RIGHT_SHIFT,
				KEY_RIGHT_CONTROL,
				KEY_RIGHT_ALT,
				KEY_RIGHT_SUPER,
				KEY_MENU,

				MOUSE_POS,
				MOUSE_BUTTON_1,
				MOUSE_BUTTON_2,
				MOUSE_BUTTON_3,
				MOUSE_BUTTON_4,
				MOUSE_BUTTON_5,
				MOUSE_BUTTON_6,
				MOUSE_BUTTON_7,
				MOUSE_BUTTON_8,

				JOYSTICK_1,
				JOYSTICK_2,
				JOYSTICK_3,
				JOYSTICK_4,
				JOYSTICK_5,
				JOYSTICK_6,
				JOYSTICK_7,
				JOYSTICK_8,
				JOYSTICK_9,
				JOYSTICK_10,
				JOYSTICK_11,
				JOYSTICK_12,
				JOYSTICK_13,
				JOYSTICK_14,
				JOYSTICK_15,
				JOYSTICK_16,

				GAMEPAD_BUTTON_A,
				GAMEPAD_BUTTON_B,
				GAMEPAD_BUTTON_X,
				GAMEPAD_BUTTON_Y,
				GAMEPAD_BUTTON_LEFT_BUMPER,
				GAMEPAD_BUTTON_RIGHT_BUMPER,
				GAMEPAD_BUTTON_BACK,
				GAMEPAD_BUTTON_START,
				GAMEPAD_BUTTON_GUIDE,
				GAMEPAD_BUTTON_LEFT_THUMB,
				GAMEPAD_BUTTON_RIGHT_THUMB,
				GAMEPAD_BUTTON_DPAD_UP,
				GAMEPAD_BUTTON_DPAD_RIGHT,
				GAMEPAD_BUTTON_DPAD_DOWN,
				GAMEPAD_BUTTON_DPAD_LEFT,				

				GAMEPAD_AXIS_LEFT_X,
				GAMEPAD_AXIS_LEFT_Y,
				GAMEPAD_AXIS_RIGHT_X,
				GAMEPAD_AXIS_RIGHT_Y,
				GAMEPAD_AXIS_LEFT_TRIGGER,
				GAMEPAD_AXIS_RIGHT_TRIGGER,

				Count
			};

			struct InputEntry
			{
				InputType inputType;
				InputSource inputSource;

				InputCode inputCodes[4]; //x, -x, y, -y 

				InputValue previousValue;
				InputValue currentValue;

				float deadZone;

				bool operator== (const InputEntry& other)
				{
					const bool inputCodesEqual = memcmp(&inputCodes, &other.inputCodes, sizeof(this->inputCodes)) == 0;
					return inputCodesEqual && inputType == other.inputType && inputSource == other.inputSource;
				}
			};

		public:

			static void Init();
			static void Stop();

			static InputValue GetActionValue(StringId inputAction);

			static void UpdateEntries();
			static bool GetGamepadIsConnected();
			static void SetGamepadIsConnected(bool value);

			static HashTable<StringId, DynamicArray<InputEntry>>& GetInputActions();

			static void SerializeInputEntries();
			static void AddInputEntry(StringId actionName, const InputEntry& entry);


		private:			

			static void DeserializeInputEntries();
			static bool CompareInputCodes(InputCode* codes1, InputCode* codes2)
			{
				for (ui32 i = 0; i < 4; ++i)
				{
					if (codes1[i] != codes2[i])
					{
						return false;
					}
				}

				return true;
			}
			static InputEntry* FindEntryByActionAndInputCode(StringId inputAction, InputCode inputCode, ui32& inputCodeIndex);

			static ui32 InputCodeToBlKeyCode(InputSource inputSource, InputType inputType, InputCode code);
			static InputCode BlKeyCodeToInputCode(InputSource inputSource, InputType inputType, ui32 blKeyCode);

		private:
			
			static HashTable<StringId, DynamicArray<InputEntry>> s_actionBindings;

			static InputCode s_keyboardInputs[349];
			static InputCode s_mouseInputs[8];
			static InputCode s_gamepadInputButtons[15];
			static InputCode s_gamepadInputAxes[6];

			static bool s_initialized;
			static bool s_gamepadIsConected;
		};

		class PlatformInput
		{
		public:
			static bool IsKeyPressed(int keycode);

			static bool IsMouseButtonPress(int button);
			static bool IsMouseButtonReleased(int button);
			static std::pair<float, float> GetMousePosition();
			static float GetMouseX();
			static float GetMouseY();

			static float GetGamepadAxisValue(int gamepadIndex, int button);
			static bool GetGamepadButtonValue(int gamepadIndex, int button);
			static bool GetGamepadIsConnected(int gamepadIndex);

		};
	}
}