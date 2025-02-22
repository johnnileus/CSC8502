#include "../NCLGL/window.h"
#include "Renderer.h"
//switch gpu to nvidia
//extern "C" {
//	_declspec(dllexport)DWORD NvOptimusEnablement = 0x00000001;
//}

float fpsDelay = 0.0f;

int main() {
	Window w("project", 1920, 1080, false); //This is all boring win32 window creation stuff!
	if (!w.HasInitialised()) {
		char a;
		std::cin >> a;
		return -1;
	}

	Renderer renderer(w); //This handles all the boring OGL 3.2 initialisation stuff, and sets up our tutorial!
	if (!renderer.HasInitialised()) {
		char a;
		std::cin >> a;
		return -1;
	}

	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);

	while (w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)) {
		float dt = w.GetTimer()->GetTimeDeltaSeconds();
		fpsDelay += dt;
		if (fpsDelay > .08f) {
			w.SetTitle("fps: " + std::to_string(1 / dt));
			fpsDelay = 0;

		}
		renderer.UpdateScene(dt);
		renderer.RenderScene();
		renderer.SwapBuffers();
		renderer.SetTime(w.GetTimer()->GetTotalTimeMSec());
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_F5)) {
			Shader::ReloadAllShaders();
		}
	}
	return 0;
}