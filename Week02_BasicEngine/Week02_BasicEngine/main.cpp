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
    long width;
    long height;
    switch (message)
    {
    case WM_DESTROY:
        // Signal that the app should quit
        PostQuitMessage(0);
        break;
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
        {
            bStopRender = true;
            break;
        }
        else if (wParam == SIZE_MAXIMIZED || (wParam == SIZE_RESTORED && !bStopRender))
        {
            GetClientRect(hWnd, &rect);
            width = rect.right - rect.left;
            height = rect.bottom - rect.top;
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
            bStopRender = false;
            GetClientRect(hWnd, &rect);
            width = rect.right - rect.left;
            height = rect.bottom - rect.top;
            screenWidth = width;
            screenHeight = height;
            bResizeWindow = true;
        }
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
void DrawCreateWindow(FMeshResource* CubeResource, FMeshResource* SphereResource, FMeshResource* PlaneResource, UPrimitiveComponent*& pickedPrimitivePtr)
{
    ImGui::Begin("Jungle Control Panel");
    ImGui::Text("Hello Jungle World!");
    ImGui::Text("FPS %d (%d ms)", 999, 999);
    const char* typeNames[] = { "None", "Plane", "Cube", "Sphere", "XLine", "YLine", "ZLine", "Max" };
    static ETypePrimitive current = ETypePrimitive::Sphere;
    if (ImGui::BeginCombo("Primitive", typeNames[(int32)current]))
    {
        for (int32 i = 1; i < (int32)ETypePrimitive::Floor; i++)
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
    static float FOV = 80.f;
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
        case ETypePrimitive::Plane:
            newPrimitive = new UPlaneComp();
            newPrimitive->SetMeshResource(PlaneResource);
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

        ImGui::Separator();

        //카메라
        ImGui::Checkbox("##Orthogonal", &IsOrthogonal);
        ImGui::SameLine();
        ImGui::Text("Orthogonal");

        ImGui::Text("FOV");
        ImGui::SameLine(130);
        ImGui::SetNextItemWidth(170.0f);
        ImGui::InputFloat("##FOV", &FOV);
        //Location
        ImGui::Text("Camera Location");
        ImGui::SameLine(130);
        ImGui::SetNextItemWidth(50.0f);
        ImGui::InputFloat("##CLx_input", &CLx);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(50.0f);
        ImGui::InputFloat("##CLy_input", &CLy);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(50.0f);
        ImGui::InputFloat("##CLz_input", &CLz);

        //Rotation
        ImGui::Text("Camera Rotation");
        ImGui::SameLine(130);
        ImGui::SetNextItemWidth(50.0f);
        ImGui::InputFloat("##CRx_input", &CRx);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(50.0f);
        ImGui::InputFloat("##CRy_input", &CRy);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(50.0f);
        ImGui::InputFloat("##CRz_input", &CRz);
    }


    ImGui::Separator();
    ImGui::Separator();

    //Scene 이름을 담는 버퍼
    static char inputBuffer[256] = "Default";
    ImGui::InputText("Scene Name", inputBuffer, IM_ARRAYSIZE(inputBuffer));

    //새로운 Scene 생성
    if (ImGui::Button("New Scene", ImVec2(100.0f, 0.0f)))
    {
        if (inputBuffer[0] != '\0')
        {
            // ImGui의 UTF-8 입력 → wchar_t
            wchar_t sceneName[256] = {};
            int count = MultiByteToWideChar(
                CP_UTF8, MB_ERR_INVALID_CHARS,
                inputBuffer, -1,
                sceneName, 256
            );

            if (count > 0)
            {
                FString path(L"..\\");
                path.Append(FString(sceneName));
                path.Append(L".Scene");

                FJsonWrapper::NewScene(std::filesystem::path(*path));

            }
        }
    }

    //현재 씬 저장
    if (ImGui::Button("Save Scene", ImVec2(100.0f, 0.0f)))
    {
        if (inputBuffer[0] != '\0')
        {
            // ImGui의 UTF-8 입력 → wchar_t
            wchar_t sceneName[256] = {};
            int count = MultiByteToWideChar(
                CP_UTF8, MB_ERR_INVALID_CHARS,
                inputBuffer, -1,
                sceneName, 256
            );

            if (count > 0)
            {
                FString path(L"..\\");
                path.Append(FString(sceneName));
                path.Append(L".Scene");

                if (FJsonWrapper::SaveScene(std::filesystem::path(*path)))
                {
                    inputBuffer[0] = '\0';
                }
            }
        }
    }

    //여러 씬 중 원하는 씬 로드
    namespace fs = std::filesystem;

    static fs::path sceneDirectory;
    static std::vector<fs::path> sceneFiles;
    static int selectedScene = -1;
    static bool initialized = false;
    static std::string sceneMessage;

    auto RefreshSceneFiles = [&]()
        {
            sceneFiles.clear();
            selectedScene = -1;
            sceneMessage.clear();

            try
            {
                sceneDirectory = FJsonWrapper::FindDirectory(L".slnx");

                if (sceneDirectory.empty())
                {
                    sceneMessage = "Solution directory not found.";
                    return;
                }

                // slnx와 같은 폴더의 Scene 파일만 검색
                for (const auto& entry : fs::directory_iterator(sceneDirectory))
                {
                    if (!entry.is_regular_file())
                        continue;

                    std::wstring extension = entry.path().extension().wstring();

                    if (_wcsicmp(extension.c_str(), L".Scene") == 0)
                    {
                        sceneFiles.push_back(entry.path());
                    }
                }

                std::sort(sceneFiles.begin(), sceneFiles.end());

                if (!sceneFiles.empty())
                    selectedScene = 0;
            }
            catch (const std::exception&)
            {
                sceneMessage = "Failed to list scene files.";
            }
        };

    if (!initialized)
    {
        RefreshSceneFiles();
        initialized = true;
    }

    if (ImGui::Button("Refresh Scenes"))
    {
        RefreshSceneFiles();
    }

    // ImGui에는 UTF-8 문자열 전달
    auto GetFileLabel = [](const fs::path& path) -> std::string
        {
            const auto utf8 = path.filename().u8string();

            return std::string(
                reinterpret_cast<const char*>(utf8.data()),
                utf8.size()
            );
        };

    std::string preview = selectedScene >= 0
        ? GetFileLabel(sceneFiles[selectedScene])
        : "No scene selected";

    if (ImGui::BeginCombo("Scene Files", preview.c_str()))
    {
        for (int i = 0; i < static_cast<int>(sceneFiles.size()); ++i)
        {
            const std::string label = GetFileLabel(sceneFiles[i]);
            const bool selected = (selectedScene == i);

            if (ImGui::Selectable(label.c_str(), selected))
            {
                selectedScene = i;
            }

            if (selected)
                ImGui::SetItemDefaultFocus();
        }

        ImGui::EndCombo();
    }

    ImGui::BeginDisabled(selectedScene < 0);

    if (ImGui::Button("Load Selected Scene"))
    {
        // 경로에 이미 .Scene이 포함되어 있으므로 다시 붙이지 않음
        if (FJsonWrapper::LoadScene(
            sceneFiles[selectedScene],
            CubeResource,
            SphereResource,
            PlaneResource))
        {
            pickedPrimitivePtr = nullptr;
            sceneMessage = "Scene loaded.";
        }
        else
        {
            sceneMessage = "Failed to load scene.";
        }
    }

    ImGui::EndDisabled();

    ImGui::Separator();



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

    FMeshResource Floor1ResourceData;
    FMeshResource Floor2ResourceData;

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

    Floor1ResourceData.SetMeshResourceData(Floor1Resource);
    Floor2ResourceData.SetMeshResourceData(Floor2Resource);


    //초기화
    CubeResourceData.Initialize();
    SphereResourceData.Initialize();
    LineResourceData.Initialize();
    TriangleResourceData.Initialize();
    PlaneResourceData.Initialize();

    LocationGizmoResourceData.Initialize();
    RotationGizmoResourceData.Initialize();
    ScaleGizmoResourceData.Initialize();

    Floor1ResourceData.Initialize();
    Floor2ResourceData.Initialize();

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
    Camera->RelativeLocation = { -2.5f, 2.5f,-2.5f };
    Camera->RelativeRotation = { -0.5f,-1.0f, 0 };

    //좌표축
    UAxisGizmo* AxisGizmo = new UAxisGizmo();

    AxisGizmo->RelativeScale3D = FVector(1.0f, 1.0f, 1.0f);

    AxisGizmo->SetMeshResource(&LineResourceData);

    AxisGizmo->RelativeLocation = FVector(0, 0, 0);
    AxisGizmo->RelativeRotation = FVector(0, 0, 0);
    AxisGizmo->RelativeScale3D = FVector(10.f, 10.f, 10.f);


    //ExampleAppConsole Console;
    bool is_window_open = true;


    //picking
    bool bIsPicking = false;
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
    Test3->SetMeshResource(&CubeResourceData);
    Test4->SetMeshResource(&CubeResourceData);
    Test5->SetMeshResource(&CubeResourceData);
    Test6->SetMeshResource(&CubeResourceData);
    Test7->SetMeshResource(&PlaneResourceData);

    Test->SetRasterizerState(RasterizerState::WireFrame);
    Test3->SetRasterizerState(RasterizerState::WireFrame);
    Test5->SetRasterizerState(RasterizerState::FrontCulling);

    XGizmo->SetMeshResource(&LocationGizmoResourceData);
    YGizmo->SetMeshResource(&LocationGizmoResourceData);
    ZGizmo->SetMeshResource(&LocationGizmoResourceData);

    XGizmo->RelativeScale3D = FVector(1, 1, 1);
    YGizmo->RelativeScale3D = FVector(1, 1, 1);
    ZGizmo->RelativeScale3D = FVector(1, 1, 1);
    XGizmo->RelativeRotation = FVector(0, 0, 0);
    YGizmo->RelativeRotation = FVector(0, 0, 1.57);
    ZGizmo->RelativeRotation = FVector(0, -1.57, 0);


    Test->RelativeLocation = FVector(1, 0, 0);
    Test2->RelativeLocation = FVector(-1, 0, 0);
    Test3->RelativeLocation = FVector(0, 1, 0);
    Test3->RelativeRotation = FVector(0, 0, 1.57);
    Test4->RelativeLocation = FVector(0, -1, 0);
    Test5->RelativeLocation = FVector(0, 0, 1);
    Test5->RelativeRotation = FVector(0, -1.57, 0);
    Test6->RelativeLocation = FVector(0, 0, -1);
    Test7->RelativeRotation = FVector(DegreeToRadian(90), 0, 0);

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
    Floor->ConstructFloor(&Floor1ResourceData, &Floor2ResourceData); 
    GUObjectArray.RemoveObj(Floor);

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
            Camera->RelativeLocation.y -= cameraSpeed;
        }
        if (GetAsyncKeyState(0x45) & 0x8000) { //아래 (E)
            Camera->RelativeLocation.y += cameraSpeed;
        }
        if (GetAsyncKeyState(VK_SPACE) & 0x0001) {
            switch (XGizmo->Type) {
            case ETypeTransform::Location:
                XGizmo->SetMeshResource(&RotationGizmoResourceData);
                YGizmo->SetMeshResource(&RotationGizmoResourceData);
                ZGizmo->SetMeshResource(&RotationGizmoResourceData);
                XGizmo->Type = ETypeTransform::Rotation;
                YGizmo->Type = ETypeTransform::Rotation;
                ZGizmo->Type = ETypeTransform::Rotation;
                XGizmo->RelativeRotation = FVector(0, 1.57, 0);
                YGizmo->RelativeRotation = FVector(1.57, 0, 0);
                ZGizmo->RelativeRotation = FVector(0, 0, 0);
                break;
            case ETypeTransform::Rotation:
                XGizmo->SetMeshResource(&ScaleGizmoResourceData);
                YGizmo->SetMeshResource(&ScaleGizmoResourceData);
                ZGizmo->SetMeshResource(&ScaleGizmoResourceData);
                XGizmo->Type = ETypeTransform::Scale;
                YGizmo->Type = ETypeTransform::Scale;
                ZGizmo->Type = ETypeTransform::Scale;
                XGizmo->RelativeRotation = FVector(0, 0, 0);
                YGizmo->RelativeRotation = FVector(0, 0, 1.57);
                ZGizmo->RelativeRotation = FVector(0, -1.57, 0);
                break;
            case ETypeTransform::Scale:
                XGizmo->SetMeshResource(&LocationGizmoResourceData);
                YGizmo->SetMeshResource(&LocationGizmoResourceData);
                ZGizmo->SetMeshResource(&LocationGizmoResourceData);
                XGizmo->Type = ETypeTransform::Location;
                YGizmo->Type = ETypeTransform::Location;
                ZGizmo->Type = ETypeTransform::Location;
                XGizmo->RelativeRotation = FVector(0, 0, 0);
                YGizmo->RelativeRotation = FVector(0, 0, 1.57);
                ZGizmo->RelativeRotation = FVector(0, -1.57, 0);
                break;
            }
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
                    float deltaX = (float)(currentMousePos.x - lastMousePos.x) * sensitivity;
                    float deltaY = (float)(currentMousePos.y - lastMousePos.y) * sensitivity;

                    FVector MouseMove = XAxis * deltaX + YAxis * deltaY;
                    static_cast<UGizmo*>(pickedGizmoPtr)->Update(pickedObjectPtr, MouseMove);
                    XGizmo->RelativeLocation = pickedObjectPtr->RelativeLocation;
                    YGizmo->RelativeLocation = pickedObjectPtr->RelativeLocation;
                    ZGizmo->RelativeLocation = pickedObjectPtr->RelativeLocation;

                    lastMousePos = currentMousePos;
                }

                if (!isDragging) {
                    if (pickedObjectPtr != nullptr)
                    {
                        pickedGizmoPtr = UPicking::GetPickedPrimitive(ndcX, ndcY, Camera, ZAxis, XAxis, YAxis, GUObjectArray.GetAllObjects(), GUObjectArray.GetNum(), &bIsPicking);
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
                        pickedObjectPtr = UPicking::GetPickedPrimitive(ndcX, ndcY, Camera, ZAxis, XAxis, YAxis, GUObjectArray.GetAllObjects(), GUObjectArray.GetNum(), &bIsPicking);
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
        DrawCreateWindow(&CubeResourceData, &SphereResourceData, &PlaneResourceData, pickedObjectPtr);
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
        
    //렌더러들보다 먼저 소멸
    GUObjectArray.Release();

    renderer.ReleaseConstantBuffer();
    renderer.ReleaseShader();
    renderer.Release();

    GGraphicsDevice.Release();
    return 0;
}