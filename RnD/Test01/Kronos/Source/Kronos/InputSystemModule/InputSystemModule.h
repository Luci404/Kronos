#pragma once

#include "Kronos/Core/Module.h"

#include <cstdint>
#include <string>

namespace Kronos
{
	class InputSystemModule final : public StaticModule
	{
	};

	enum EInputKeyState : uint8_t
	{
		Pressed,
		Released
	};

	enum EInputKey : uint8_t
	{
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
		KEY_Z
	};

	class InputManager
	{
	public:
		template <typename T>
		static void BindKey(EInputKey key, EInputKeyState state, T *instance, void (T::*function)()) {}

		template <typename T>
		static void BindAction(const std::string &name, EInputKeyState state, T *instance, void (T::*function)()) {}
	};
}