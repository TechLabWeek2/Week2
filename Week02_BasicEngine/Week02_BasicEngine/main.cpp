#include <windows.h>
#include "URenderer.h"
#include "Shapes.h"

class UPrimitive {
public:
    FVector Location;
    FVector Rotation;
    float Radius;
    float Mass;
    ETypePrimitive Type = ETypePrimitive::EPT_Cube;
    UPrimitive(FVector location, FVector rotation, float radius, ETypePrimitive type) : Location(location), Rotation(rotation), Radius(radius) {
        Mass = Radius;
        Type = type;
    }
    virtual ~UPrimitive() {}
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
        CW_USEDEFAULT, CW_USEDEFAULT, 1024, 1024,
        nullptr, nullptr, hInstance, nullptr);

    // Renderer Class를 생성합니다.
    URenderer	renderer;

    // D3D11 생성하는 함수를 호출합니다.
    renderer.Create(hWnd);
    // 렌더러 생성 직후에 쉐이더를 생성하는 함수를 호출합니다.
    renderer.CreateShader();
    //생성 함수 추가
    renderer.CreateConstantBuffer();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplWin32_Init((void*)hWnd);
    ImGui_ImplDX11_Init(renderer.Device, renderer.DeviceContext);


    // Renderer와 Shader 생성 이후에 버텍스 버퍼를 생성합니다.
    UINT numVerticesTriangle = sizeof(triangle_vertices) / sizeof(FVertexSimple);
    UINT numVerticesCube = sizeof(cube_vertices) / sizeof(FVertexSimple);
    UINT numVerticesSphere = sizeof(sphere_vertices) / sizeof(FVertexSimple);

    ID3D11Buffer* vertexBufferTriangle = renderer.CreateVertexBuffer(triangle_vertices, sizeof(triangle_vertices));
    ID3D11Buffer* vertexBufferCube = renderer.CreateVertexBuffer(cube_vertices, sizeof(cube_vertices));
    ID3D11Buffer* vertexBufferSphere = renderer.CreateVertexBuffer(sphere_vertices, sizeof(sphere_vertices));

    bool bIsExit = false;

    UPrimitive** PrimitiveList = nullptr;

    int TargetBallCount = 1;

    ETypePrimitive typePrimitive = EPT_Cube;

    FVector offset(0.0f);
    FVector velocity(0.0f);

    //화면의 경계 위치
    const float leftBorder = -1.0f;
    const float rightBorder = 1.0f;
    const float topBorder = -1.0f;
    const float bottomBorder = 1.0f;
    //화면을 넘어가는 것을 막을지 여부
    bool bBoundBallToScreen = true;
    bool bPinballMovement = true;

    velocity.x = ((float)(rand() % 100 - 50)) * 0.001f;
    velocity.y = ((float)(rand() % 100 - 50)) * 0.001f;

    //FPS 제한을 위한 설정
    const int targetFPS = 30;
    const double targetFrameTime = 1000.0 / targetFPS; //한 프레임의 목표 시간(밀리초 단위)

    //고성능 타이머 초기화
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);

    LARGE_INTEGER startTime, endTime;
    double elapsedTime = 0.0;
    float degree = 0;
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

        UPrimitive* Primitive = new UPrimitive({ 0,0,0 }, { degree,degree,degree }, 0.1f, ETypePrimitive::EPT_Sphere);
        degree += 1.f;
        if (degree >= 360)   degree = 0;
        renderer.UpdateConstant(Primitive->Location, Primitive->Radius, Primitive->Rotation);
        switch (Primitive->Type)
        {
        case EPT_Cube:
            renderer.RenderPrimitive(vertexBufferCube, numVerticesCube);
            break;
        case EPT_Sphere:
            renderer.RenderPrimitive(vertexBufferSphere, numVerticesSphere);
            break;
        case EPT_Triangle:
            renderer.RenderPrimitive(vertexBufferTriangle, numVerticesTriangle);
            break;
        default:
            break;
        }
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // 이후 ImGui UI 컨트롤 추가는 ImGui::NewFrame()과 ImGui::Render() 사이인 여기에 위치합니다.
        ImGui::Begin("Jungle Property Window");
        ImGui::Text("Hello Jungle World!");
        ImGui::Checkbox("Bound Ball To Screen", &bBoundBallToScreen);
        ImGui::Checkbox("Pinball Movement", &bPinballMovement);
        ImGui::InputInt("Target Ball Count", &TargetBallCount);
        ImGui::End();

        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        // 다 그렸으면 버퍼를 교환
        renderer.SwapBuffer();

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
    renderer.ReleaseVertexBuffer(vertexBufferTriangle);
    renderer.ReleaseVertexBuffer(vertexBufferCube);
    renderer.ReleaseVertexBuffer(vertexBufferSphere);
    renderer.ReleaseConstantBuffer();
    renderer.ReleaseShader();
    renderer.Release();
    return 0;
}