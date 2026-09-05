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
    UINT numVerticesLine = sizeof(line_vertices) / sizeof(FVertexSimple);
    UINT numVerticesTriangle = sizeof(triangle_vertices) / sizeof(FVertexSimple);
    UINT numVerticesCube = sizeof(cube_vertices) / sizeof(FVertexSimple);
    UINT numVerticesSphere = sizeof(sphere_vertices) / sizeof(FVertexSimple);

    ID3D11Buffer* vertexBufferLine = renderer.CreateVertexBuffer(line_vertices, sizeof(line_vertices));
    ID3D11Buffer* vertexBufferTriangle = renderer.CreateVertexBuffer(triangle_vertices, sizeof(triangle_vertices));
    ID3D11Buffer* vertexBufferCube = renderer.CreateVertexBuffer(cube_vertices, sizeof(cube_vertices));
    ID3D11Buffer* vertexBufferSphere = renderer.CreateVertexBuffer(sphere_vertices, sizeof(sphere_vertices));

    bool bIsExit = false;

    UPrimitive** PrimitiveList = new UPrimitive* [30];

    int UPrimitiveCnt = 6;

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
    float degree = 100;

    FVector CameraLocation = { 0,0,0 };
    FVector CameraForward = { 0,0,1 };
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

        static POINT lastMousePos = currentMousePos;
        static bool isDragging = false;

        if (GetAsyncKeyState(VK_LEFT) & 0x8000 || GetAsyncKeyState(0x41) & 0x8000) { //왼쪽 (A)
            FVector XAxis;
            XAxis.x = CameraForward.z;
            XAxis.y = 0;
            XAxis.z = -CameraForward.x;
            XAxis.Normalize();

            CameraLocation.x -= XAxis.x * 0.01f;
            CameraLocation.y -= XAxis.y * 0.01f;
            CameraLocation.z -= XAxis.z * 0.01f;
        }
        if (GetAsyncKeyState(VK_RIGHT) & 0x8000 || GetAsyncKeyState(0x44) & 0x8000) { //오른쪽 (D)
            FVector XAxis;
            XAxis.x = CameraForward.z;
            XAxis.y = 0;
            XAxis.z = -CameraForward.x;
            XAxis.Normalize();

            CameraLocation.x += XAxis.x * 0.01f;
            CameraLocation.y += XAxis.y * 0.01f;
            CameraLocation.z += XAxis.z * 0.01f;
        }
        if (GetAsyncKeyState(VK_UP) & 0x8000 || GetAsyncKeyState(0x57) & 0x8000) { //앞 (W)
            CameraLocation.x += CameraForward.x * 0.01f;
            CameraLocation.y += CameraForward.y * 0.01f;
            CameraLocation.z += CameraForward.z * 0.01f;
        }
        if (GetAsyncKeyState(VK_DOWN) & 0x8000 || GetAsyncKeyState(0x53) & 0x8000) { //뒤 (S)
            CameraLocation.x -= CameraForward.x * 0.01f;
            CameraLocation.y -= CameraForward.y * 0.01f;
            CameraLocation.z -= CameraForward.z * 0.01f;
        }
        if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
        {
            if (!isDragging)
            {
                isDragging = true;
                lastMousePos = currentMousePos;
            }

            float deltaX = (float)(currentMousePos.x - lastMousePos.x);
            float deltaY = (float)(currentMousePos.y - lastMousePos.y);

            float sensitivity = 0.002f;

            float angleX = deltaX * sensitivity;
            float angleY = -deltaY * sensitivity;

            // 좌우 회전
            FVector rotation;

            rotation.x = CameraForward.x * cos(angleX)
                + CameraForward.z * sin(angleX);

            rotation.y = CameraForward.y;

            rotation.z = -CameraForward.x * sin(angleX)
                + CameraForward.z * cos(angleX);

            rotation.Normalize();

            FVector XAxis;
            XAxis.x = rotation.z;
            XAxis.y = 0;
            XAxis.z = -rotation.x;
            XAxis.Normalize();

            FVector YAxis;
            YAxis.x = rotation.y * XAxis.z - rotation.z * XAxis.y;
            YAxis.y = rotation.z * XAxis.x - rotation.x * XAxis.z;
            YAxis.z = rotation.x * XAxis.y - rotation.y * XAxis.x;
            YAxis.Normalize();

            // 상하 회전
            FVector rotation2;

            rotation2.x = rotation.x * cos(angleY) + YAxis.x * sin(angleY) + XAxis.x * YAxis.Dot(XAxis, rotation) * (1.0f - cos(angleY));

            rotation2.y = rotation.y * cos(angleY) + YAxis.y * sin(angleY) + XAxis.y * YAxis.Dot(XAxis, rotation) * (1.0f - cos(angleY));

            rotation2.z = rotation.z * cos(angleY) + YAxis.z * sin(angleY) + XAxis.z * YAxis.Dot(XAxis, rotation) * (1.0f - cos(angleY));

            rotation2.Normalize();

            CameraForward = rotation2;

            lastMousePos = currentMousePos;
        }
        else
        {
            isDragging = false;
            lastMousePos = currentMousePos;
        }
        PrimitiveList[0] = new UPrimitive({ 0,0,1 }, { 0,0,0 }, 0.1f, ETypePrimitive::EPT_Cube);
        PrimitiveList[1] = new UPrimitive({ 0,0,-1 }, { 0,0,0 }, 0.1f, ETypePrimitive::EPT_Cube);
        PrimitiveList[2] = new UPrimitive({ 0,1,0 }, { 0,0,0 }, 0.1f, ETypePrimitive::EPT_Cube);
        PrimitiveList[3] = new UPrimitive({ 0,-1,0 }, { 0,0,0 }, 0.1f, ETypePrimitive::EPT_Cube);
        PrimitiveList[4] = new UPrimitive({ 1,0,0 }, { 0,0,0 }, 0.1f, ETypePrimitive::EPT_Cube);
        PrimitiveList[5] = new UPrimitive({ -1,0,0 }, { 0,0,0 }, 0.1f, ETypePrimitive::EPT_Cube);
        UPrimitiveCnt = 6;
        degree -= 1.f;
        if (degree >= 360)   degree = 0;
        for (int i = 0; i < UPrimitiveCnt; i++) {
            renderer.UpdateConstant(PrimitiveList[i]->Location, PrimitiveList[i]->Radius, PrimitiveList[i]->Rotation, CameraLocation, CameraForward);
            switch (PrimitiveList[i]->Type)
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
        }
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // 이후 ImGui UI 컨트롤 추가는 ImGui::NewFrame()과 ImGui::Render() 사이인 여기에 위치합니다.
        ImGui::Begin("Jungle Property Window");
        ImGui::Text("Hello Jungle World!");
        ImGui::Checkbox("Bound Ball To Screen", &bBoundBallToScreen);
        ImGui::Checkbox("Pinball Movement", &bPinballMovement);
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