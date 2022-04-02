#pragma once

#include "Kronos/Core/Module.h"

#include <cstdint>
#include <string>
#include <map>

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

	class Delegate
	{
	public:
		void Invoke()
		{
			for (auto pair : m_ListenerMap)
			{
				pair.first->pair.second();
			}
		}

		template<typename T>
		void RegisterListener(T* instance, void(T::*function)()) {}

		template<typename T>
		void UnregisterListener(T* instance) {}

	private:
		std::map<void*, void(*)()> m_ListenerMap;
	};

	class DelegateTest1
	{
	public:
		DelegateTest1()
		{
		}

		void Tick()
		{
			m_TickDelegate.Invoke();
		}

		Delegate& GetTickDelegate() { return m_TickDelegate; }

	private:
		Delegate m_TickDelegate;
	};

	class DelegateTest2
	{
	public:
		DelegateTest2(DelegateTest1 other)
		{
			other.GetTickDelegate().RegisterListener(this, OnTick);
		}

		void OnTick()
		{

		}
	};


	class InputManager
	{
	public:
		template <typename T>
		static void BindKey(EInputKey key, EInputKeyState state, T *instance, void (T::*function)()) {}
		static void InvokeKey(EInputKey key, EInputKeyState state) {}
	};
}