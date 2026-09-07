#pragma once
#include <windows.h>

class URenderer;
class UCameraComp;

class FEngineLoop
{
public:
    void Init(HWND hWindow, UINT screenWidth, UINT screenHeight);
	void Update(float deltaTime);
	void Render();
    void Release();

private:
    URenderer* Renderer;
    //UTextRenderer TextRenderer;
    UCameraComp* MainCamera;
    //UCameraComp* MinimapCamera;
};

