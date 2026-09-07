#include "FEngineLoop.h"
#include "UObject.h"
#include "UObjectArray.h"
#include "FGraphicsDevice.h"
#include "USceneComponent.h"
#include "URenderer.h"
#include "UCameraComp.h"

void FEngineLoop::Init(HWND hWindow, UINT screenWidth, UINT screenHeight)
{
	//Renderer = new URenderer();
	//Renderer->Create(hWindow, screenWidth, screenHeight);

	//MainCamera = new UCameraComp();


}

void FEngineLoop::Update(float deltaTime)
{
	//TArray<UObject*> AllObj = GUObjectArray.GetAllObjects();

	///*for (UObject* Obj : GUObjectArray.GetAllObjects())
	//{

	//}*/

	//for (int i = 0; i < AllObj.Num(); i++)
	//{
	//	if (AllObj[i] == nullptr) continue;

	//	//SceneComponent만 Update하도록
	//	USceneComponent* SceneComponent = dynamic_cast<USceneComponent*>(AllObj[i]);
	//	if (SceneComponent)
	//	{
	//		SceneComponent->Update(deltaTime);
	//	}
	//}
}

void FEngineLoop::Render()
{
	//Renderer->RenderScene(GUObjectArray.GetAllObjects(), MainCamera, )
}

void FEngineLoop::Release()
{
	if (Renderer)
	{
		Renderer->Release();
		delete Renderer;
		Renderer = nullptr;
	}

	if (MainCamera)
	{
		delete MainCamera;
		MainCamera = nullptr;
	}
}
