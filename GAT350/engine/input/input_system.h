#pragma once

#include "../framework/system.h"

class InputSystem : public System
{
public:
	OBJECT_DECLARATION(InputSystem, System)

	~InputSystem() {}

	bool Initialize();
	void Shutdown();

	void Update();

	bool GetKey(SDL_Scancode scancode);

private:
	const Uint8* m_keyboard_state = nullptr;
};
