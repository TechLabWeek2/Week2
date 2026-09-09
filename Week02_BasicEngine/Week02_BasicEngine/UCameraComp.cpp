#include "UCameraComp.h"
#include "FMatrix.h"
#include "FGraphicsDevice.h"

UCameraComp::UCameraComp()
{
    Class = StaticClass();
	FOV = 90.0f;
}

UCameraComp::~UCameraComp()
{
}

FMatrix UCameraComp::GetViewMatrix() const
{
	//return FMatrix::GetViewMatrix(RelativeLocation, RelativeRotation);
	return FMatrix::GetViewMatrix_UEToDX(RelativeLocation, RelativeRotation);
}

FMatrix UCameraComp::GetProjectionMatrix() const
{
	return FMatrix::GetProjectionMatrix(DegreeToRadian(FOV / 2.f), ZoomLevel, AspectRatio, NearClip, FarClip, IsOrthogonal);
}

//const FVector UCameraComp::GetForwardVector_UE() const
//{
//	FVector Forward(cos(RelativeRotation.y) * cos(RelativeRotation.z), 
//					-cos(RelativeRotation.y) * sin(RelativeRotation.z), 
//					sin(RelativeRotation.y));
//	Forward.Normalize();
//	return Forward;
//}
//
//const FVector UCameraComp::GetRightVector_UE() const
//{
//	FVector Right = FVector(0.f, 0.f, 1.f).Cross(GetForwardVector_UE());
//	Right.Normalize();
//	return Right;
//}
//
//const FVector UCameraComp::GetUpVector_UE() const
//{
//	FVector Forward = GetForwardVector_UE();
//	FVector Right = FVector(0.f, 0.f, 1.f).Cross(Forward);
//	Right.Normalize();
//	FVector Up = Forward.Cross(Right);
//	Up.Normalize();
//	return Up;
//}
//
void UCameraComp::UpdateArguments(bool bFocus, bool bMouseOut, bool bImGuiWantCaptureMouse, bool bImGuiWantCaptureKeyboard,  POINT* currentMousePos)
{
    this->bFocus = bFocus;
    this->bMouseOut = bMouseOut;
    this->bImGuiWantCaptureMouse = bImGuiWantCaptureMouse;
    this->bImGuiWantCaptureKeyboard = bImGuiWantCaptureKeyboard;
    this->currentMousePos = currentMousePos;
}

void UCameraComp::Update(float deltaTime)
{
    if (!bFocus)
    {
        return;
    }

    FVector CameraForward = GetForwardVector_UE();
    FVector CameraRight = GetRightVector_UE();
    FVector CameraUp = GetUpVector_UE();

    const float moveSensitivity = 0.002f;
    const float rotateSensitivity = 0.00015f;
    cameraSpeed = deltaTime * moveSensitivity;
    rotateSpeed = deltaTime * rotateSensitivity;

    float deltaX = float(lastMousePos.x - currentMousePos->x);
    float deltaY = float(lastMousePos.y - currentMousePos->y);
    lastMousePos = *currentMousePos;

    if (!bImGuiWantCaptureKeyboard)
    {
        if (GetAsyncKeyState(VK_LEFT) & 0x8000 || GetAsyncKeyState(0x41) & 0x8000) { //왼쪽 (A)
            RelativeLocation -= (CameraRight * cameraSpeed);
        }
        if (GetAsyncKeyState(VK_RIGHT) & 0x8000 || GetAsyncKeyState(0x44) & 0x8000) { //오른쪽 (D)
            RelativeLocation += CameraRight * cameraSpeed;
        }
        if (GetAsyncKeyState(VK_UP) & 0x8000 || GetAsyncKeyState(0x57) & 0x8000) { //앞 (W)
            if (IsOrthogonal)
            {
                ZoomLevel -= 0.5f * cameraSpeed;
            }
            else
            {
                RelativeLocation.x += CameraForward.x * cameraSpeed;
                RelativeLocation.y += CameraForward.y * cameraSpeed;
                RelativeLocation.z += CameraForward.z * cameraSpeed;
            }
        }
        if (GetAsyncKeyState(VK_DOWN) & 0x8000 || GetAsyncKeyState(0x53) & 0x8000) { //뒤 (S)
            if (IsOrthogonal)
            {
                ZoomLevel += 0.5f * cameraSpeed;
            }
            else
            {
                RelativeLocation.x -= CameraForward.x * cameraSpeed;
                RelativeLocation.y -= CameraForward.y * cameraSpeed;
                RelativeLocation.z -= CameraForward.z * cameraSpeed;
            }
        }
        if (GetAsyncKeyState(0x51) & 0x8000) { //위 (Q)
            RelativeLocation.z -= cameraSpeed;
        }
        if (GetAsyncKeyState(0x45) & 0x8000) { //아래 (E)
            RelativeLocation.z += cameraSpeed;
        }
    }
    
    if (bMouseOut)
    {
        return;
    }
    if (GetAsyncKeyState(VK_RBUTTON) & 0x8000) // 마우스 우클릭으로 시점 변경
    {
        if (!bIsRotating && !bImGuiWantCaptureMouse)
        {
            bIsRotating = true;
            lastMousePos = *currentMousePos;
        }

        float angleX = deltaX * rotateSpeed;
        float angleY = deltaY * rotateSpeed;

        // 좌우
        RelativeRotation.z += angleX;
        // 상하
        RelativeRotation.y += angleY;

        // 상하 각도 제한
        if (RelativeRotation.y <= DegreeToRadian(-89.f))
        {
            RelativeRotation.y = DegreeToRadian(-89.f);
        }
        else if (RelativeRotation.y >= DegreeToRadian(89.f))
        {
            RelativeRotation.y = DegreeToRadian(89.f);
        }

        lastMousePos = *currentMousePos;
    }
    else if (bIsRotating)
    {
        bIsRotating = false;
        lastMousePos = *currentMousePos;
    }
}