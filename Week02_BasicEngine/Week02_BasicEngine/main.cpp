#pragma once 
#include <windows.h>
#include "URenderer.h"
#include "Shapes.h"
#include "UCubeComp.h"
#include "USphereComp.h"
#include "UCameraComp.h"
#include "UAxisGizmo.h"
#include "UPicking.h"
#include "UObject.h"
#include "UObjectArray.h"
#include "FGraphicsDevice.h"
#include "FMeshResource.h"

#define SCREEN_WIDTH 1800
#define SCREEN_HEIGHT 1200

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

UCameraComp* Camera = new UCameraComp();

enum ETypeLine {
    ETL_LB,
    OTHER
};

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// 각종 메시지를 처리할 함수
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
    {
        return true;
    }

    switch (message)
    {
    case WM_DESTROY:
        // Signal that the app should quit
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

void DrawStatWindow()
{
    if (ImGui::Begin("Stat"))
    {
        ImGui::Text(
            "Live Objects: %u",
            static_cast<unsigned int>(GUObjectArray.GetNum())
        );

        ImGui::Text(
            "Heap Allocations: %u",
            static_cast<unsigned int>(TotalAllocationCount)
        );

        ImGui::Text(
            "Heap Memory: %u bytes",
            static_cast<unsigned int>(TotalAllocationBytes)
        );

        ImGui::Text(
            "Heap Memory: %.2f KiB",
            static_cast<double>(TotalAllocationBytes) / 1024.0
        );
    }

    ImGui::End();
}

//Test를 위해서 변수를 넘겨줌
void DrawCreateWindow(FMeshResource* CubeResource, FMeshResource* SphereResource, UPrimitiveComponent*& pickedPrimitivePtr)
{
    ImGui::Begin("Jungle Control Panel");
    ImGui::Text("Hello Jungle World!");
    ImGui::Text("FPS %d (%d ms)", 999, 999);
    const char* typeNames[] = { "None", "Triangle", "Cube", "Sphere", "XLine", "YLine", "ZLine", "Max" };
    static ETypePrimitive current = ETypePrimitive::Cube;
    if (ImGui::BeginCombo("Primitive", typeNames[(int32)current]))
    {
        for (int32 i = 2; i < (int32)ETypePrimitive::XLine; i++)
        {
            bool selected = ((int32)current == i);
            if (ImGui::Selectable(typeNames[i], selected))
            {
                current = (ETypePrimitive)i;
            }
            if (selected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    static float Lx = 0, Ly = 0, Lz = 0;
    static float Rx = 0, Ry = 0, Rz = 0;
    static float Sx = 0.1f, Sy = 0.1f, Sz = 0.1f;
    static bool IsOrthogonal = false;
    static float FOV = 60.f;
    static float CLx = 0, CLy = 0, CLz = 0;
    static float CRx = 0, CRy = 0, CRz = 0;
    static int spawnNum = 1;
    if (ImGui::Button("Spawn", ImVec2(50.0f, 0.0f))) 
    {
        UPrimitiveComponent* newPrimitive = nullptr;
        switch (current)
        {
        case ETypePrimitive::Cube:  //렌더러 정리 끝나면 할것 
            newPrimitive = new UCubeComp();
            newPrimitive->SetMeshResource(CubeResource);
            break;
        case ETypePrimitive::Sphere:
            newPrimitive = new USphereComp();
            newPrimitive->SetMeshResource(SphereResource);
            break;
        }
        newPrimitive->RelativeLocation = FVector(Lx, Ly, Lz);
        newPrimitive->RelativeRotation = FVector(DegreeToRadian(Rx), DegreeToRadian(Ry), DegreeToRadian(Rz));
        newPrimitive->RelativeScale3D = FVector(Sx, Sy, Sz);
    }
    ImGui::SameLine();
    ImGui::InputInt("Number of spawn", &spawnNum);

    //Draw
    ImGui::BeginGroup();

    //Location
    ImGui::Text("Location");
    ImGui::SameLine(70);
    ImGui::SetNextItemWidth(50.0f);
    ImGui::InputFloat("##Lx_input", &Lx);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(50.0f);
    ImGui::InputFloat("##Ly_input", &Ly);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(50.0f);
    ImGui::InputFloat("##Lz_input", &Lz);

    //Rotation
    ImGui::Text("Rotation");
    ImGui::SameLine(70);
    ImGui::SetNextItemWidth(50.0f);
    ImGui::InputFloat("##Rx_input", &Rx);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(50.0f);
    ImGui::InputFloat("##Ry_input", &Ry);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(50.0f);
    ImGui::InputFloat("##Rz_input", &Rz);

    //Scale
    ImGui::Text("Scale");
    ImGui::SameLine(70);
    ImGui::SetNextItemWidth(50.0f);
    ImGui::InputFloat("##Sx_input", &Sx);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(50.0f);
    ImGui::InputFloat("##Sy_input", &Sy);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(50.0f);
    ImGui::InputFloat("##Sz_input", &Sz);

    ImGui::EndGroup();

    ImGui::Separator();

    if (ImGui::Button("Delete", ImVec2(50.0f, 0.0f))) 
    {
        if (pickedPrimitivePtr)
        {
            GUObjectArray.RemoveObj(pickedPrimitivePtr);
        }
    }

    ImGui::Separator();

    //카메라
    ImGui::Checkbox("##Orthogonal", &Camera->IsOrthogonal);
    ImGui::SameLine();
    ImGui::Text("Orthogonal");

    ImGui::Text("FOV");
    ImGui::SameLine(130);
    ImGui::SetNextItemWidth(170.0f);
    ImGui::DragFloat("##FOV", &Camera->FOV, 0.1f, 0.0f, 90.f);
    //Location
    ImGui::Text("Camera Location");
    ImGui::SameLine(130);
    ImGui::SetNextItemWidth(50.0f);
    ImGui::InputFloat("##CLx_input", &Camera->RelativeLocation.x);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(50.0f);
    ImGui::InputFloat("##CLy_input", &Camera->RelativeLocation.y);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(50.0f);
    ImGui::InputFloat("##CLz_input", &Camera->RelativeLocation.z);

    //Rotation
    ImGui::Text("Camera Rotation");
    ImGui::SameLine(130);
    ImGui::SetNextItemWidth(50.0f);
    ImGui::InputFloat("##CRx_input", &Camera->RelativeRotation.x);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(50.0f);
    ImGui::InputFloat("##CRy_input", &Camera->RelativeRotation.y);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(50.0f);
    ImGui::InputFloat("##CRz_input", &Camera->RelativeRotation.z);

    ImGui::End();
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
        CW_USEDEFAULT, CW_USEDEFAULT, SCREEN_WIDTH, SCREEN_HEIGHT,
        nullptr, nullptr, hInstance, nullptr);

    //공통으로 사용할 Graphics Device를 가지게 됩니다.
    GGraphicsDevice.Initialize(hWnd, SCREEN_WIDTH, SCREEN_WIDTH);

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
    ImGui_ImplDX11_Init(renderer.Device, renderer.DeviceContext);

    //Mesh Resource 만들기.
    FMeshResource* SphereResource = new FMeshResource();
    FMeshResource* CubeResource = new FMeshResource();
    FMeshResource* TriangleResource = new FMeshResource();
    FMeshResource* LineResource = new FMeshResource();

    SphereResource->CreateVertexBuffer(sphere_vertices, sizeof(sphere_vertices));
    CubeResource->CreateVertexBuffer(cube_vertices, sizeof(cube_vertices));
    TriangleResource->CreateVertexBuffer(triangle_vertices, sizeof(triangle_vertices));
    LineResource->CreateVertexBuffer(line_vertices, sizeof(line_vertices));

    SphereResource->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    CubeResource->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    TriangleResource->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    LineResource->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

    bool bIsExit = false;

    //FPS 제한을 위한 설정
    const int targetFPS = 30;
    const double targetFrameTime = 1000.0 / targetFPS; //한 프레임의 목표 시간(밀리초 단위)

    //고성능 타이머 초기화
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);

    LARGE_INTEGER startTime, endTime;
    double elapsedTime = 0.0;
    
    //카메라
    UCameraComp* Camera = new UCameraComp();
    Camera->RelativeLocation = { -2.5f, 2.5f,-2.5f };
    Camera->RelativeRotation = { -0.5f,-1.0f, 0 };

    //좌표축
    UAxisGizmo* AxisGizmo = new UAxisGizmo();

    AxisGizmo->RelativeScale3D = FVector(1.0f, 1.0f, 1.0f);

    AxisGizmo->SetMeshResource(LineResource);

    AxisGizmo->RelativeLocation = FVector(0, 0, 0);
    AxisGizmo->RelativeRotation = FVector(0, 0, 0);
    AxisGizmo->RelativeScale3D = FVector(10.f, 10.f, 10.f);


    ExampleAppConsole Console;
    bool is_window_open = true;


    //picking
    bool bIsPicking = false;
    UPrimitiveComponent* pickedObjectPtr = nullptr;

    UCubeComp* Test = new UCubeComp();
    UCubeComp* Test2 = new UCubeComp();
    UCubeComp* Test3 = new UCubeComp();
    UCubeComp* Test4 = new UCubeComp();
    UCubeComp* Test5 = new UCubeComp();
    UCubeComp* Test6 = new UCubeComp();

    Test->SetMeshResource(CubeResource);
    Test2->SetMeshResource(CubeResource);
    Test3->SetMeshResource(CubeResource);
    Test4->SetMeshResource(CubeResource);
    Test5->SetMeshResource(CubeResource);
    Test6->SetMeshResource(CubeResource);

    // Main Loop (Quit Message가 들어오기 전까지 아래 Loop를 무한히 실행하게 됨)
    while (bIsExit == false)
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
        ////////////////////////////////////////////
        // 매번 실행되는 코드를 여기에 추가합니다.
        // 준비 작업
        renderer.Prepare();
        renderer.PrepareShader();


        POINT currentMousePos;
        GetCursorPos(&currentMousePos); // 현재 마우스 스크린 좌표 획득
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

        const float cameraSpeed = 0.03f;
        if (GetAsyncKeyState(VK_LEFT) & 0x8000 || GetAsyncKeyState(0x41) & 0x8000) { //왼쪽 (A)
            Camera->RelativeLocation.x -= XAxis.x * cameraSpeed;
            Camera->RelativeLocation.y -= XAxis.y * cameraSpeed;
            Camera->RelativeLocation.z -= XAxis.z * cameraSpeed;
            //Console.UE_LOG("%s %c %f %d %u %o %x","Hello",'A',3.14f,-100,100,100,255);
        }
        if (GetAsyncKeyState(VK_RIGHT) & 0x8000 || GetAsyncKeyState(0x44) & 0x8000) { //오른쪽 (D)
            Camera->RelativeLocation.x += XAxis.x * cameraSpeed;
            Camera->RelativeLocation.y += XAxis.y * cameraSpeed;
            Camera->RelativeLocation.z += XAxis.z * cameraSpeed;
        }
        if (GetAsyncKeyState(VK_UP) & 0x8000 || GetAsyncKeyState(0x57) & 0x8000) { //앞 (W)
            if (Camera->IsOrthogonal)
            {
                Camera->ZoomLevel -= 0.5f * cameraSpeed;
            }
            else
            {
                Camera->RelativeLocation.x += ZAxis.x * cameraSpeed;
                Camera->RelativeLocation.y += ZAxis.y * cameraSpeed;
                Camera->RelativeLocation.z += ZAxis.z * cameraSpeed;
            }
        }
        if (GetAsyncKeyState(VK_DOWN) & 0x8000 || GetAsyncKeyState(0x53) & 0x8000) { //뒤 (S)
            if (Camera->IsOrthogonal)
            {
                Camera->ZoomLevel += 0.5f * cameraSpeed;
            }
            else
            {
                Camera->RelativeLocation.x -= ZAxis.x * cameraSpeed;
                Camera->RelativeLocation.y -= ZAxis.y * cameraSpeed;
                Camera->RelativeLocation.z -= ZAxis.z * cameraSpeed;
            }
        }
        if (GetAsyncKeyState(0x51) & 0x8000) { //위 (Q)
            Camera->RelativeLocation.y += cameraSpeed;
        }
        if (GetAsyncKeyState(0x45) & 0x8000) { //아래 (E)
            Camera->RelativeLocation.y -= cameraSpeed;
        }
        if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
        {
            if (!isDragging && !io.WantCaptureMouse)
            {
                isDragging = true;
                lastMousePos = currentMousePos;
            }

            float deltaX = (float)(currentMousePos.x - lastMousePos.x);
            float deltaY = (float)(currentMousePos.y - lastMousePos.y);

            float sensitivity = 0.002f;

            float angleX = deltaX * sensitivity;
            float angleY = -deltaY * sensitivity;

            // 좌우
            Camera->RelativeRotation.y += angleX;
            // 상하
            Camera->RelativeRotation.x += angleY;

            lastMousePos = currentMousePos;
        }
        else if (isLeftMousePressed)
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

                if (pickedObjectPtr != nullptr)
                {
                    pickedObjectPtr->bIsSelected = !pickedObjectPtr->bIsSelected;
                }
                pickedObjectPtr = UPicking::GetPickedPrimitive(ndcX, ndcY, Camera, ZAxis, XAxis, YAxis, GUObjectArray.GetAllObjects(), GUObjectArray.GetNum(), &bIsPicking);
                if (pickedObjectPtr != nullptr) 
                { 
                    pickedObjectPtr->bIsSelected = !pickedObjectPtr->bIsSelected;
                }
            }
        }
        else
        {
            isDragging = false;
            lastMousePos = currentMousePos;
        }
             
        Test->RelativeLocation = FVector(1, 0, 0);
        Test2->RelativeLocation = FVector(-1, 0, 0);
        Test3->RelativeLocation = FVector(0, 1, 0);
        Test4->RelativeLocation = FVector(0, -1, 0);
        Test5->RelativeLocation = FVector(0, 0, 1);
        Test6->RelativeLocation = FVector(0, 0, -1);

        
        //리스트 돌면서 렌더
        renderer.RenderScene(GUObjectArray.GetAllObjects(), Camera, 0.f);

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        
        Console.Draw("Console Windows", &is_window_open);
        DrawCreateWindow(CubeResource, SphereResource, pickedObjectPtr);
        DrawStatWindow();

        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        GGraphicsDevice.SwapBuffer();

        do {
            Sleep(0);

            QueryPerformanceCounter(&endTime);

            //프레임이 소요된 시간 계산 (밀리초 단위로 변환)
            elapsedTime = (endTime.QuadPart - startTime.QuadPart) * 1000.0 / frequency.QuadPart;

        } while (elapsedTime < targetFrameTime);
        ////////////////////////////////////////////
    }

    // D3D11 소멸 시키는 함수를 호출합니다.
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    //리소스 제거
    if (SphereResource)
    {
        SphereResource->Release();
        delete SphereResource;
    }

    if (CubeResource)
    {
        CubeResource->Release();
        delete CubeResource;
    }

    if (TriangleResource)
    {
        TriangleResource->Release();
        delete TriangleResource;
    }

    if (LineResource)
    {
        LineResource->Release();
        delete LineResource;
    }
    
    //렌더러들보다 먼저 소멸
    GUObjectArray.Release();

    renderer.ReleaseConstantBuffer();
    renderer.ReleaseShader();
    renderer.Release();

    GGraphicsDevice.Release();
    return 0;
}