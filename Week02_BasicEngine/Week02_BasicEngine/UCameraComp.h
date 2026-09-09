#pragma once
#include "USceneComponent.h"
#include "ImGui/imgui.h"

class UCameraComp :
    public USceneComponent
{
public:
    float FOV = 90.0f;
    float AspectRatio = 1024.f / 1024.f;
    float NearClip = 0.1f;
    float FarClip = 5000.0f;
    float ZoomLevel = 1.f;
    bool IsOrthogonal = false;
    float cameraSpeed = 0.04f;
    float rotateSpeed = 0.04f;

    FVector ZAxis;
    FVector XAxis;
    FVector YAxis;

    bool bIsRotating = false;
    bool bFocus = false;
    bool bMouseOut = false;
    bool bImGuiWantCaptureMouse = false;
    POINT lastMousePos = {};
    POINT* currentMousePos = {};

public:
    UCameraComp();
    ~UCameraComp();

    FMatrix GetViewMatrix() const;
    FMatrix GetProjectionMatrix() const;
    FMatrix GetProjectionMatrixOrthogonal() const;

    /*const FVector GetForwardVector_UE()const;
    const FVector GetRightVector_UE()const;
    const FVector GetUpVector_UE()const;*/

    void UpdateArguments(bool bFocus, bool bMouseOut, bool bImGuiWantCaptureMouse, POINT* currentMousePos);
    void Update(float deltaTime) override;

    static UClass* StaticClass()
    {
        static UClass Class("UCameraComp", USceneComponent::StaticClass());

        return &Class;
    }
};