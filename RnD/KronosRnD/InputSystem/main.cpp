#include <cstdint>
#include <string>

enum EInputActionState : uint8_t
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

class InputSystem
{
public:
	// void BindAction(const FName ActionName, const EInputEvent KeyEvent, UserClass* Object, typename FInputActionHandlerSignature::TUObjectMethodDelegate< UserClass >::FMethodPtr Func)
	template<typename T>
	static void BindKey(EInputKey key, EInputActionState state, T* instance, void (T::* function)()) {}

	template<typename T>
	static void BindAction(const std::string& name, EInputActionState state, T* instance, void (T::*function)()) {}

private:
	
};

class PlayerEntity
{
public:
	PlayerEntity()
	{
		InputSystem::BindKey(EInputKey::A, EInputActionState::Pressed, this, &PlayerEntity::OnAPressed);
		InputSystem::BindAction("ActionName", EInputActionState::Released, this, &PlayerEntity::OnActionNameReleased);
	}

private:
	void OnAPressed() {}

	void OnActionNameReleased() {}
};

int main(int argc, char* argv[]) {}