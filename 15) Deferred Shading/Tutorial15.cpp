#include "../nclgl/Window.h"
#include "Renderer.h"

#include <iostream>

int main() {
	Window w("Deferred Rendering!", 1280,720,false); //This is all boring win32 window creation stuff!
	if(!w.HasInitialised()) {
		return -1;
		char s;
		std::cin >> s;
	}

	Renderer renderer(w); //This handles all the boring OGL 3.2 initialisation stuff, and sets up our tutorial!
	if(!renderer.HasInitialised()) {
		char s;
		std::cin >> s;
		return -1;
	}

	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);

	while(w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)){
		renderer.UpdateScene(w.GetTimer()->GetTimeDeltaSeconds());
		renderer.RenderScene();
		renderer.SwapBuffers();
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_F5)) {
			Shader::ReloadAllShaders();
		}
	}

	return 0;
}