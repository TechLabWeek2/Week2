#pragma once 
#include <windows.h>
#include "URenderer.h"
#include "Shapes.h"
#include "UCubeComp.h"
#include "USphereComp.h"
#include "UPlaneComp.h"
#include "UCameraComp.h"
#include "UAxisGizmo.h"
#include "UPicking.h"
#include "UObject.h"
#include "UObjectArray.h"
#include "FGraphicsDevice.h"
#include "FMeshResource.h"
#include "UGizmo.h"
#include "UFloorComp.h"
#include "FShaderResource.h"
#include "ImGuiFunction.h"

#define SCREEN_WIDTH_INIT 1800
#define SCREEN_HEIGHT_INIT 1200

//#if IMGUI_VERSION_NUM >= 19263
//namespace ImGui { extern IMGUI_API void DemoMarker(const char* file, int line, const char* section); }
//#define IMGUI_DEMO_MARKER(section)  do { ImGui::DemoMarker("imgui_demo.cpp", __LINE__, section); } while (0)
//#endif
////struct FVertexSimple;
//struct ExampleAppConsole
//{
//    char                  InputBuf[256];
//    ImVector<char*>       Items;
//    ImVector<const char*> Commands;
//    ImVector<char*>       History;
//    int                   HistoryPos;    // -1: new line, 0..History.Size-1 browsing history.
//    ImGuiTextFilter       Filter;
//    bool                  AutoScroll;
//    bool                  ScrollToBottom;

#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/imgui_impl_dx11.h"
#include "imGui/imgui_impl_win32.h"
#include "ExampleAppConsole.h"
#include <FJsonWrapper.h>

UINT screenWidth = SCREEN_WIDTH_INIT;
UINT screenHeight = SCREEN_HEIGHT_INIT;
bool bStopRender = false;
bool bResizeWindow = false;
bool bFocus = true;

UCameraComp* Camera = new UCameraComp();

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// 각종 메시지를 처리할 함수
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
    {
        return true;
    }
    RECT rect;
    GetClientRect(hWnd, &rect);
    long width = rect.right - rect.left;
    long height = rect.bottom - rect.top;
    switch (message)
    {
    case WM_DESTROY:
        // Signal that the app should quit
        PostQuitMessage(0);
        break;
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
        {
            //bStopRender = true; // 최소화시 구현 필요
            break;
        }
        else if (wParam == SIZE_MAXIMIZED || (wParam == SIZE_RESTORED && !bStopRender))
        {
            screenWidth = width;
            screenHeight = height;
            bResizeWindow = true;
            break;
        }
        break;
    case WM_ENTERSIZEMOVE:
        bStopRender = true;
        break;
    case WM_EXITSIZEMOVE:
        if (bStopRender)
        {
            screenWidth = width;
            screenHeight = height;
            bStopRender = false;
            bResizeWindow = true;
        }
        break;
    case WM_SETFOCUS:
        bFocus = true;
        break;
    case WM_KILLFOCUS:
        bFocus = false;
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    // 윈도우 클래스 이름
    WCHAR WindowClass[] = L"JungleWindowClass";

    // 윈도우 타이틀바에 표시될 이름
    WCHAR Title[] = L"Game Tech Lab";

    // 각종 메시지를 처리할 함수인 WndProc의 함수 포인터를 WindowClass 구조체에 넣는다.
    WNDCLASSW wndclass = { 0, WndProc, 0, 0, 0, 0, 0, 0, 0, WindowClass };

    // 윈도우 클래스 등록
    RegisterClassW(&wndclass);

    // 1024 x 1024 크기에 윈도우 생성
    HWND hWnd = CreateWindowExW(0, WindowClass, Title, WS_POPUP | WS_VISIBLE | WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, screenWidth, screenHeight,
        nullptr, nullptr, hInstance, nullptr);

    //공통으로 사용할 Graphics Device를 가지게 됩니다.
    GGraphicsDevice.Initialize(hWnd, screenWidth, screenHeight);

    // Renderer Class를 생성합니다.
    URenderer renderer;

    //렌더러 초기화
    renderer.Init();

    // 렌더러 생성 직후에 쉐이더를 생성하는 함수를 호출합니다.
    renderer.CreateShader();
    //생성 함수 추가
    renderer.CreateConstantBuffer();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplWin32_Init((void*)hWnd);
    ImGui_ImplDX11_Init(GGraphicsDevice.GetDevice(), GGraphicsDevice.GetDeviceContext());

    ////Mesh Resource 만들기.

    //생성
    FMeshResource CubeResourceData;
    FMeshResource SphereResourceData;
    FMeshResource LineResourceData;
    FMeshResource TriangleResourceData;
    FMeshResource PlaneResourceData;

    FMeshResource LocationGizmoResourceData;
    FMeshResource RotationGizmoResourceData;
    FMeshResource ScaleGizmoResourceData;

    FMeshResource FloorResourceData;

    //데이터 할당
    CubeResourceData.SetMeshResourceData(CubeResource);
    SphereResourceData.SetMeshResourceData(SphereResource);
    LineResourceData.SetMeshResourceData(LineResource);
    TriangleResourceData.SetMeshResourceData(TriangleResource);
    PlaneResourceData.SetMeshResourceData(PlaneResource);

    InitializeVertexArray(LocationGizmoResource.Vertices, 144);
    InitializeVertexArray(RotationGizmoResource.Vertices, 1536);

    CreateLocationGizmo(LocationGizmoResource.Vertices.Data(), 0.0f, 0.0f, 1.0f);
    CreateRotationGizmo(RotationGizmoResource.Vertices.Data(), 1.0f, 0.0f, 0.0f);

    LocationGizmoResourceData.SetMeshResourceData(LocationGizmoResource);
    RotationGizmoResourceData.SetMeshResourceData(RotationGizmoResource);
    ScaleGizmoResourceData.SetMeshResourceData(ScaleGizmoResource);

    FloorResourceData.SetMeshResourceData(FloorResource);

    //초기화
    CubeResourceData.Initialize();
    SphereResourceData.Initialize();
    LineResourceData.Initialize();
    TriangleResourceData.Initialize();
    PlaneResourceData.Initialize();

    LocationGizmoResourceData.Initialize();
    RotationGizmoResourceData.Initialize();
    ScaleGizmoResourceData.Initialize();

    FloorResourceData.Initialize();


    FMeshResourceRegistry MeshRegistry;
    MeshRegistry.Registry(ETypePrimitive::Cube, &CubeResourceData);
    MeshRegistry.Registry(ETypePrimitive::Sphere, &SphereResourceData);
    MeshRegistry.Registry(ETypePrimitive::Plane, &PlaneResourceData);


    //Shader Resource
    DefaultShader.SetVertexShaderName(L"ShaderW0.hlsl");
    DefaultShader.SetPixelShaderName(L"ShaderW0.hlsl");

    TArray <D3D11_INPUT_ELEMENT_DESC> DefaultLayout =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    DefaultShader.SetInputLayoutDesc(DefaultLayout);

    //Shader 데이터 생성
    DefaultShader.CreateShaderResource();

    //체크무늬 셰이더
    CheckerShader.SetVertexShaderName(L"ShaderW0.hlsl");
    CheckerShader.SetPixelShaderName(L"CheckPattern.hlsl");

    TArray <D3D11_INPUT_ELEMENT_DESC> CheckerLayout =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    CheckerShader.SetInputLayoutDesc(CheckerLayout);

    //Shader 데이터 생성
    CheckerShader.CreateShaderResource();


    bool bIsExit = false;

    //FPS 제한을 위한 설정
    const int targetFPS = 60;
    const double targetFrameTime = 1000.0 / targetFPS; //한 프레임의 목표 시간(밀리초 단위)
    double currentFPS = 0.0;

    //고성능 타이머 초기화
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);

    LARGE_INTEGER startTime, endTime;
    double elapsedTime = 0.0;
    
    //카메라
    Camera->RelativeLocation = { -4.7f, 4.0f, 2.6f };
    Camera->RelativeRotation = { 0.f, -0.3f, 0.6f };

    //좌표축
    UAxisGizmo* AxisGizmo = new UAxisGizmo();

    AxisGizmo->RelativeScale3D = FVector(1.0f, 1.0f, 1.0f);

    AxisGizmo->SetMeshResource(&LineResourceData);
    AxisGizmo->SetShaderResource(&DefaultShader);

    AxisGizmo->RelativeLocation = FVector(0, 0, 0);
    AxisGizmo->RelativeRotation = FVector(0, 0, 0);
    AxisGizmo->RelativeScale3D = FVector(10.f, 10.f, 10.f);


    //ExampleAppConsole Console;
    bool is_window_open = true;


    //picking
    bool bIsPicking = false;
    UPrimitiveComponent* prevObjectPtr = nullptr;
    UPrimitiveComponent* hoveringtObjectPtr = nullptr;
    UPrimitiveComponent* pickedObjectPtr = nullptr;
    UPrimitiveComponent* pickedGizmoPtr = nullptr;

    UCubeComp* Test = new UCubeComp();
    UCubeComp* Test2 = new UCubeComp();
    USphereComp* Test3 = new USphereComp();
    UCubeComp* Test4 = new UCubeComp();
    UCubeComp* Test5 = new UCubeComp();
    USphereComp* Test6 = new USphereComp(); 
    UPlaneComp* Test7 = new UPlaneComp();
    UGizmo* XGizmo = new UGizmo(ETypeAxis::XAxis);
    UGizmo* YGizmo = new UGizmo(ETypeAxis::YAxis);
    UGizmo* ZGizmo = new UGizmo(ETypeAxis::ZAxis);

    Test->SetMeshResource(&CubeResourceData);
    Test2->SetMeshResource(&CubeResourceData);
    Test3->SetMeshResource(&SphereResourceData);
    Test4->SetMeshResource(&CubeResourceData);
    Test5->SetMeshResource(&CubeResourceData);
    Test6->SetMeshResource(&SphereResourceData);
    Test7->SetMeshResource(&PlaneResourceData);    

    Test->SetShaderResource(&DefaultShader);
    Test2->SetShaderResource(&DefaultShader);
    Test3->SetShaderResource(&DefaultShader);
    Test4->SetShaderResource(&DefaultShader);
    Test5->SetShaderResource(&DefaultShader);
    Test6->SetShaderResource(&DefaultShader);
    Test7->SetShaderResource(&DefaultShader);

    Test->SetRasterizerState(RasterizerState::WireFrame);
    Test3->SetRasterizerState(RasterizerState::WireFrame);
    Test5->SetRasterizerState(RasterizerState::FrontCulling);

    XGizmo->SetMeshResource(&LocationGizmoResourceData);
    YGizmo->SetMeshResource(&LocationGizmoResourceData);
    ZGizmo->SetMeshResource(&LocationGizmoResourceData);

    XGizmo->SetShaderResource(&DefaultShader);
    YGizmo->SetShaderResource(&DefaultShader);
    ZGizmo->SetShaderResource(&DefaultShader);

    XGizmo->RelativeScale3D = FVector(1, 1, 1);
    YGizmo->RelativeScale3D = FVector(1, 1, 1);
    ZGizmo->RelativeScale3D = FVector(1, 1, 1);
    XGizmo->RelativeRotation = FVector(0, 0, 0);
    YGizmo->RelativeRotation = FVector(0, 0, 1.57);
    ZGizmo->RelativeRotation = FVector(0, -1.57, 0);


    /*Test->RelativeLocation = FVector(1, 0, 0);
    Test2->RelativeLocation = FVector(-1, 0, 0);
    Test3->RelativeLocation = FVector(0, 1, 0);
    Test3->RelativeRotation = FVector(0, 0, 1.57);
    Test4->RelativeLocation = FVector(0, -1, 0);
    Test5->RelativeLocation = FVector(0, 0, 1);
    Test5->RelativeRotation = FVector(0, -1.57, 0);
    Test6->RelativeLocation = FVector(0, 0, -1);
    Test7->RelativeRotation = FVector(DegreeToRadian(90), 0, 0);*/

    //UE -> DX 테스트
    Test->RelativeLocation = FVector(0, 0, 1);
    Test2->RelativeLocation = FVector(0, 0, 2);
    Test3->RelativeLocation = FVector(0, 0, 3);
    //Test3->RelativeRotation = FVector(0, 0, 1.57);
    Test4->RelativeLocation = FVector(0, 0, 4);
    Test5->RelativeLocation = FVector(0, 0, 5);
    //Test5->RelativeRotation = FVector(0, -1.57, 0);
    Test6->RelativeLocation = FVector(0, 0, 6);
    Test7->RelativeLocation = FVector(0, 0, 0.5f);
    //Test7->RelativeRotation = FVector(DegreeToRadian(90), 0, 0);

    TArray<float> Red = { 1.f, 0.f, 0.f, 1.0f };
    TArray<float> Green = { 0.f, 1.f, 0.f, 1.f };
    TArray<float> Blue = { 0.f, 0.f, 1.f, 1.f };
    XGizmo->SetModelColor(Red);
    YGizmo->SetModelColor(Green);
    ZGizmo->SetModelColor(Blue);
    XGizmo->SetUseColorFlag(true);
    YGizmo->SetUseColorFlag(true);
    ZGizmo->SetUseColorFlag(true);

    UFloorComp* Floor = new UFloorComp();

    Floor->primitiveType = ETypePrimitive::Floor;
    Floor->SetMeshResource(&FloorResourceData);
    Floor->SetShaderResource(&CheckerShader);

    Floor->SetRasterizerState(RasterizerState::Solid_Culling_None);

    Floor->RelativeScale3D = FVector(5.f, 5.f, 5.f);
    //Floor->RelativeRotation = FVector(DegreeToRadian(90), 0.f, 0.f); 
    Floor->RelativeLocation = FVector(0.f, 0.f, 0.f);

    Floor->SetBlendMode(BlendMode::Alpha); 
    Floor->SetUseColorFlag(true);
    TArray<float> Black = { 0.f, 0.f, 0.f, 1.f };
    Floor->SetModelColor(Black);

    /*GUObjectArray.RemoveObj(Test);
    GUObjectArray.RemoveObj(Test3);
    GUObjectArray.RemoveObj(Test4);
    GUObjectArray.RemoveObj(Test5);
    GUObjectArray.RemoveObj(Test6);
    GUObjectArray.RemoveObj(Test7);*/

    // Main Loop (Quit Message가 들어오기 전까지 아래 Loop를 무한히 실행하게 됨)
    while (bIsExit == false && !bStopRender)
    {
        QueryPerformanceCounter(&startTime);

        MSG msg;

        // 처리할 메시지가 더 이상 없을때 까지 수행
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            // 키 입력 메시지를 번역
            TranslateMessage(&msg);

            // 메시지를 적절한 윈도우 프로시저에 전달, 메시지가 위에서 등록한 WndProc 으로 전달됨
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT)
            {
                bIsExit = true;
                break;
            }
        }

        if (bResizeWindow)
        {
            renderer.ResizeWindow(screenWidth, screenHeight, &bStopRender);
            bResizeWindow = false;
        }

        ////////////////////////////////////////////
        // 매번 실행되는 코드를 여기에 추가합니다.
        // 준비 작업
        renderer.Prepare();
        //renderer.PrepareShader();

        POINT currentMousePos;
        GetCursorPos(&currentMousePos); // 현재 마우스 스크린 좌표 획득
        ScreenToClient(hWnd, &currentMousePos);
        RECT Rect;
        GetClientRect(hWnd, &Rect);
        float Width = static_cast<float>(Rect.right - Rect.left);
        float Height = static_cast<float>(Rect.bottom - Rect.top);
        Camera->AspectRatio = Width / Height;
        static POINT lastMousePos = currentMousePos;
        static bool isDragging = false;
        static bool wasLeftMouseDown = false;
        const bool isLeftMouseDown = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
        const bool isLeftMousePressed = isLeftMouseDown && !wasLeftMouseDown;
        wasLeftMouseDown = isLeftMouseDown;
        
        // camera forward
        FVector ZAxis(cos(Camera->RelativeRotation.y) * cos(Camera->RelativeRotation.x), sin(Camera->RelativeRotation.x), -sin(Camera->RelativeRotation.y) * cos(Camera->RelativeRotation.x));
        ZAxis.Normalize();
        // camera right
        FVector XAxis = FVector(0, 1, 0).Cross(ZAxis);
        XAxis.Normalize();
        // camera up
        FVector YAxis = ZAxis.Cross(XAxis);
        YAxis.Normalize();

        //Mouse Hovering
        float ndcX = 2.f * (float)currentMousePos.x / Width - 1.f;  // screen xy to NDC xy
        float ndcY = 1.f - 2.f * (float)currentMousePos.y / Height;
        UPicking::Hovering(ndcX, ndcY, Camera, ZAxis, XAxis, YAxis, GUObjectArray.GetAllObjects(), GUObjectArray.GetNum(), &bIsPicking, hoveringtObjectPtr, prevObjectPtr, pickedObjectPtr, pickedGizmoPtr);

        //UE 기준 3축 벡터
        FVector CameraForward = Camera->GetForwardVector_UE();
        FVector CameraRight = Camera->GetRightVector_UE();
        FVector CameraUp = Camera->GetUpVector_UE();

        const float cameraSpeed = 0.04f;
        if (bFocus)
        {
            if (GetAsyncKeyState(VK_LEFT) & 0x8000 || GetAsyncKeyState(0x41) & 0x8000) { //왼쪽 (A)
                /*Camera->RelativeLocation.x -= XAxis.x * cameraSpeed;
                Camera->RelativeLocation.y -= XAxis.y * cameraSpeed;
                Camera->RelativeLocation.z -= XAxis.z * cameraSpeed;*/
                Camera->RelativeLocation -= (CameraRight * cameraSpeed);
                //Console.UE_LOG("%s %c %f %d %u %o %x","Hello",'A',3.14f,-100,100,100,255);
            }
            if (GetAsyncKeyState(VK_RIGHT) & 0x8000 || GetAsyncKeyState(0x44) & 0x8000) { //오른쪽 (D)
                /*Camera->RelativeLocation.x += XAxis.x * cameraSpeed;
                Camera->RelativeLocation.y += XAxis.y * cameraSpeed;
                Camera->RelativeLocation.z += XAxis.z * cameraSpeed;*/
                Camera->RelativeLocation += CameraRight * cameraSpeed;
            }
            if (GetAsyncKeyState(VK_UP) & 0x8000 || GetAsyncKeyState(0x57) & 0x8000) { //앞 (W)
                if (Camera->IsOrthogonal)
                {
                    Camera->ZoomLevel -= 0.5f * cameraSpeed;
                }
                else
                {
                    Camera->RelativeLocation.x += CameraForward.x * cameraSpeed;
                    Camera->RelativeLocation.y += CameraForward.y * cameraSpeed;
                    Camera->RelativeLocation.z += CameraForward.z * cameraSpeed;
                }
            }
            if (GetAsyncKeyState(VK_DOWN) & 0x8000 || GetAsyncKeyState(0x53) & 0x8000) { //뒤 (S)
                if (Camera->IsOrthogonal)
                {
                    Camera->ZoomLevel += 0.5f * cameraSpeed;
                }
                else
                {
                    Camera->RelativeLocation.x -= CameraForward.x * cameraSpeed;
                    Camera->RelativeLocation.y -= CameraForward.y * cameraSpeed;
                    Camera->RelativeLocation.z -= CameraForward.z * cameraSpeed;
                }
            }
            if (GetAsyncKeyState(0x51) & 0x8000) { //위 (Q)
                Camera->RelativeLocation.z -= cameraSpeed;
            }
            if (GetAsyncKeyState(0x45) & 0x8000) { //아래 (E)
                Camera->RelativeLocation.z += cameraSpeed;
            }
            if (GetAsyncKeyState(VK_SPACE) & 0x0001 && pickedObjectPtr) {
                switch (XGizmo->Type) {
                case ETypeTransform::Location:
                    XGizmo->SetMeshResource(&RotationGizmoResourceData);
                    YGizmo->SetMeshResource(&RotationGizmoResourceData);
                    ZGizmo->SetMeshResource(&RotationGizmoResourceData);
                    XGizmo->Type = ETypeTransform::Rotation;
                    YGizmo->Type = ETypeTransform::Rotation;
                    ZGizmo->Type = ETypeTransform::Rotation;
                    break;
                case ETypeTransform::Rotation:
                    XGizmo->SetMeshResource(&ScaleGizmoResourceData);
                    YGizmo->SetMeshResource(&ScaleGizmoResourceData);
                    ZGizmo->SetMeshResource(&ScaleGizmoResourceData);
                    XGizmo->Type = ETypeTransform::Scale;
                    YGizmo->Type = ETypeTransform::Scale;
                    ZGizmo->Type = ETypeTransform::Scale;
                    break;
                case ETypeTransform::Scale:
                    XGizmo->SetMeshResource(&LocationGizmoResourceData);
                    YGizmo->SetMeshResource(&LocationGizmoResourceData);
                    ZGizmo->SetMeshResource(&LocationGizmoResourceData);
                    XGizmo->Type = ETypeTransform::Location;
                    YGizmo->Type = ETypeTransform::Location;
                    ZGizmo->Type = ETypeTransform::Location;
                    break;
                }
                XGizmo->Update(pickedObjectPtr, Camera);
                YGizmo->Update(pickedObjectPtr, Camera);
                ZGizmo->Update(pickedObjectPtr, Camera);
            }
            if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
            {
                if (!isDragging && !io.WantCaptureMouse)
                {
                    isDragging = true;
                    lastMousePos = currentMousePos;
                }

            /*float deltaX = (float)(currentMousePos.x - lastMousePos.x);
            float deltaY = (float)(currentMousePos.y - lastMousePos.y);*/

            float deltaX = (float)(lastMousePos.x - currentMousePos.x);
            float deltaY = (float)(lastMousePos.y - currentMousePos.y);

                const float sensitivity = 0.002f;

            float angleX = deltaX * sensitivity;
            float angleY = deltaY * sensitivity;

            // 좌우
            Camera->RelativeRotation.z += angleX;
            // 상하
            Camera->RelativeRotation.y += angleY;

            // 상하 각도 제한
            if (Camera->RelativeRotation.y <= DegreeToRadian(-89.f))
            {
                Camera->RelativeRotation.y = DegreeToRadian(-89.f);
            }
            else if (Camera->RelativeRotation.y >= DegreeToRadian(89.f))
            {
                Camera->RelativeRotation.y = DegreeToRadian(89.f);
            }

                lastMousePos = currentMousePos;
            }
            else if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000))
            {
                if (!io.WantCaptureMouse)
                {
                    // picking
                    RECT rect;
                    GetClientRect(hWnd, &rect);
                    float screenWidth = (float)(rect.right - rect.left);
                    float screenHeight = (float)(rect.bottom - rect.top);

                    GetCursorPos(&currentMousePos);
                    ScreenToClient(hWnd, &currentMousePos);
                    float ndcX = 2.f * (float)currentMousePos.x / screenWidth - 1.f;  // screen xy to NDC xy
                    float ndcY = 1.f - 2.f * (float)currentMousePos.y / screenHeight;

                    if (pickedGizmoPtr) {
                        if (!isDragging && !io.WantCaptureMouse)
                        {
                            isDragging = true;
                            lastMousePos = currentMousePos;
                        }

                        float sensitivity = 0.002f;
                        float deltaX = (float)(currentMousePos.x - lastMousePos.x);
                        float deltaY = (float)(currentMousePos.y - lastMousePos.y);

                        FVector GizmoAxis;

                        switch (static_cast<UGizmo*>(pickedGizmoPtr)->Axis)
                        {
                        case ETypeAxis::XAxis:
                            GizmoAxis = FVector(1, 0, 0);
                            break;

                        case ETypeAxis::YAxis:
                            GizmoAxis = FVector(0, 1, 0);
                            break;

                        case ETypeAxis::ZAxis:
                            GizmoAxis = FVector(0, 0, 1);
                            break;
                        }

                        float GizmoLength = 1.f; //기즈모를 같은 크기로 했을 때 이 변수를 기즈모 길이로 수정

                        FVector ObjectLocation = pickedObjectPtr->RelativeLocation;
                        FMatrix VP = Camera->GetViewMatrix() * Camera->GetProjectionMatrix();
                        FVector StartNDC = VP.WorldToNDC(ObjectLocation, VP);
                        FVector EndNDC = VP.WorldToNDC(ObjectLocation + GizmoAxis * GizmoLength, VP);

                        FVector GizmoDirection = EndNDC - StartNDC;
                        GizmoDirection.z = 0;

                        GizmoDirection.Normalize();

                        float AxisScreenLength =
                            GizmoDirection.Length();

                        if (AxisScreenLength > 0.00001f)
                        {
                            GizmoDirection.Normalize();

                            // 마우스 이동량을 NDC로 변환
                            FVector MouseDeltaNDC(deltaX * 2.0f / Width, -deltaY * 2.0f / Height);

                            // 마우스 이동을 Gizmo 화면 방향으로 투영
                            float MouseAxisMovement = MouseDeltaNDC.Dot(GizmoDirection);

                            // NDC 이동량 → 월드 이동량
                            float WorldMoveAmount = MouseAxisMovement / AxisScreenLength;

                            // 선택된 월드 축으로만 이동
                            FVector WorldMove = GizmoAxis * WorldMoveAmount;

                            static_cast<UGizmo*>(pickedGizmoPtr)->ObjUpdate(pickedObjectPtr, WorldMove);
                        }
                        XGizmo->Update(pickedObjectPtr, Camera);
                        YGizmo->Update(pickedObjectPtr, Camera);
                        ZGizmo->Update(pickedObjectPtr, Camera);

                        lastMousePos = currentMousePos;
                    }

                    if (!isDragging) {
                        if (pickedObjectPtr != nullptr)
                        {
                            //pickedGizmoPtr = UPicking::GetPickedPrimitive(ndcX, ndcY, Camera, ZAxis, XAxis, YAxis, GUObjectArray.GetAllObjects(), GUObjectArray.GetNum(), &bIsPicking);
                            pickedGizmoPtr = UPicking::GetPickedPrimitive(
                                ndcX, ndcY, Camera, CameraForward, CameraRight, CameraUp, GUObjectArray.GetAllObjects(), GUObjectArray.GetNum(), &bIsPicking);

                            if (pickedGizmoPtr == nullptr) {
                                pickedObjectPtr->bIsSelected = !pickedObjectPtr->bIsSelected;
                                pickedObjectPtr = nullptr;
                                XGizmo->bIsActive = false;
                                YGizmo->bIsActive = false;
                                ZGizmo->bIsActive = false;
                            }
                            else if (pickedGizmoPtr->IsA(UGizmo::StaticClass())) {
                                pickedGizmoPtr->bIsSelected = !pickedGizmoPtr->bIsSelected;
                            }
                            else {
                                pickedObjectPtr->bIsSelected = !pickedObjectPtr->bIsSelected;

                                pickedObjectPtr = pickedGizmoPtr;
                                pickedGizmoPtr = nullptr;
                                pickedObjectPtr->bIsSelected = !pickedObjectPtr->bIsSelected;
                                XGizmo->bIsActive = true;
                                YGizmo->bIsActive = true;
                                ZGizmo->bIsActive = true;
                                XGizmo->RelativeLocation = pickedObjectPtr->RelativeLocation;
                                YGizmo->RelativeLocation = pickedObjectPtr->RelativeLocation;
                                ZGizmo->RelativeLocation = pickedObjectPtr->RelativeLocation;
                            }
                        }
                        else {
                            //pickedObjectPtr = UPicking::GetPickedPrimitive(ndcX, ndcY, Camera, ZAxis, XAxis, YAxis, GUObjectArray.GetAllObjects(), GUObjectArray.GetNum(), &bIsPicking);
                            pickedObjectPtr = UPicking::GetPickedPrimitive(
                                ndcX, ndcY, Camera, CameraForward, CameraRight, CameraUp, GUObjectArray.GetAllObjects(), GUObjectArray.GetNum(), &bIsPicking);

                            if (pickedObjectPtr != nullptr)
                            {
                                pickedObjectPtr->bIsSelected = !pickedObjectPtr->bIsSelected;
                                XGizmo->bIsActive = true;
                                YGizmo->bIsActive = true;
                                ZGizmo->bIsActive = true;
                                XGizmo->RelativeLocation = pickedObjectPtr->RelativeLocation;
                                YGizmo->RelativeLocation = pickedObjectPtr->RelativeLocation;
                                ZGizmo->RelativeLocation = pickedObjectPtr->RelativeLocation;
                            }
                        }
                    }
                }
            }
            else
            {
                isDragging = false;
                lastMousePos = currentMousePos;
                pickedGizmoPtr = nullptr;
            }
        }
        if (pickedObjectPtr) {
            XGizmo->Update(pickedObjectPtr, Camera);
            YGizmo->Update(pickedObjectPtr, Camera);
            ZGizmo->Update(pickedObjectPtr, Camera);
        }
        //오브젝트 순회하면서 Update 호출
        TArray<UObject*>& AllObj = GUObjectArray.GetAllObjects();
        for (int i = 0; i < AllObj.Num(); i++)
        {
            if (AllObj[i] == nullptr) continue;

            //UPrimitiveComponent만 Render하도록
            UPrimitiveComponent* PrimitiveComponent = dynamic_cast<UPrimitiveComponent*>(AllObj[i]);
            if (PrimitiveComponent && PrimitiveComponent->bIsActive)
            {
                PrimitiveComponent->Update(elapsedTime);
            }
        }
                   
        //리스트 돌면서 렌더
        renderer.RenderScene(GUObjectArray.GetAllObjects(), Camera);

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        
        Console.Draw("Console Windows", &is_window_open);
        DrawCreateWindow(Camera, pickedObjectPtr, MeshRegistry, elapsedTime, currentFPS);
        DrawStatWindow();

        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        GGraphicsDevice.SwapBuffer();

        do {
            Sleep(0);

            QueryPerformanceCounter(&endTime);

            //프레임이 소요된 시간 계산 (밀리초 단위로 변환)
            elapsedTime = (endTime.QuadPart - startTime.QuadPart) * 1000.0 / frequency.QuadPart;

            currentFPS = 1000.0 / elapsedTime;

        } while (elapsedTime < targetFrameTime);
        ////////////////////////////////////////////
    }

    // D3D11 소멸 시키는 함수를 호출합니다.
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
        
    //렌더러들보다 먼저 소멸
    GUObjectArray.Release();

    renderer.ReleaseConstantBuffer();
    renderer.ReleaseShader();
    renderer.Release();

    GGraphicsDevice.Release();
    return 0;
}