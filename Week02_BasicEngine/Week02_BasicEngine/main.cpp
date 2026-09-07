#include <windows.h>
#include "URenderer.h"
#include "Shapes.h"
#include "UCubeComp.h"
#include "UCameraComp.h"
#include "UAxisGizmo.h"
#include "UPicking.h"
#include "UObject.h"
#include "UObjectArray.h"

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 1024

#if IMGUI_VERSION_NUM >= 19263
namespace ImGui { extern IMGUI_API void DemoMarker(const char* file, int line, const char* section); }
#define IMGUI_DEMO_MARKER(section)  do { ImGui::DemoMarker("imgui_demo.cpp", __LINE__, section); } while (0)
#endif
struct FVertexSimple;
struct ExampleAppConsole
{
    char                  InputBuf[256];
    ImVector<char*>       Items;
    ImVector<const char*> Commands;
    ImVector<char*>       History;
    int                   HistoryPos;    // -1: new line, 0..History.Size-1 browsing history.
    ImGuiTextFilter       Filter;
    bool                  AutoScroll;
    bool                  ScrollToBottom;

    ExampleAppConsole()
    {
        ClearLog();
        memset(InputBuf, 0, sizeof(InputBuf));
        HistoryPos = -1;

        // "CLASSIFY" is here to provide the test case where "C"+[tab] completes to "CL" and display multiple matches.
        Commands.push_back("HELP");
        Commands.push_back("HISTORY");
        Commands.push_back("CLEAR");
        Commands.push_back("CLASSIFY");
        AutoScroll = true;
        ScrollToBottom = false;
        AddLog("Welcome to Dear ImGui!");
    }
    ~ExampleAppConsole()
    {
        ClearLog();
        for (int i = 0; i < History.Size; i++)
            ImGui::MemFree(History[i]);
    }

    // Portable helpers
    static int   Stricmp(const char* s1, const char* s2) { int d; while ((d = toupper(*s2) - toupper(*s1)) == 0 && *s1) { s1++; s2++; } return d; }
    static int   Strnicmp(const char* s1, const char* s2, int n) { int d = 0; while (n > 0 && (d = toupper(*s2) - toupper(*s1)) == 0 && *s1) { s1++; s2++; n--; } return d; }
    static char* Strdup(const char* s) { IM_ASSERT(s); size_t len = strlen(s) + 1; void* buf = ImGui::MemAlloc(len); IM_ASSERT(buf); return (char*)memcpy(buf, (const void*)s, len); }
    static void  Strtrim(char* s) { char* str_end = s + strlen(s); while (str_end > s && str_end[-1] == ' ') str_end--; *str_end = 0; }

    void    ClearLog()
    {
        for (int i = 0; i < Items.Size; i++)
            ImGui::MemFree(Items[i]);
        Items.clear();
    }

    void    AddLog(const char* fmt, ...) IM_FMTARGS(2)
    {
        // FIXME-OPT
        char buf[1024];
        va_list args;
        va_start(args, fmt);
        vsnprintf(buf, IM_COUNTOF(buf), fmt, args);
        buf[IM_COUNTOF(buf) - 1] = 0;
        va_end(args);
        Items.push_back(Strdup(buf));
    }

    void    Draw(const char* title, bool* p_open)
    {
        ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
        if (!ImGui::Begin(title, p_open))
        {
            ImGui::End();
            return;
        }
        IMGUI_DEMO_MARKER("Examples/Console");

        // As a specific feature guaranteed by the library, after calling Begin() the last Item represent the title bar.
        // So e.g. IsItemHovered() will return true when hovering the title bar.
        // Here we create a context menu only available from the title bar.
        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::MenuItem("Close Console"))
                *p_open = false;
            ImGui::EndPopup();
        }

        ImGui::TextWrapped(
            "This example implements a console with basic coloring, completion (TAB key) and history (Up/Down keys). A more elaborate "
            "implementation may want to store entries along with extra data such as timestamp, emitter, etc.");
        ImGui::TextWrapped("Enter 'HELP' for help.");

        // TODO: display items starting from the bottom

        if (ImGui::SmallButton("Add Debug Text")) { AddLog("%d some text", Items.Size); AddLog("some more text"); AddLog("display very important message here!"); }
        ImGui::SameLine();
        if (ImGui::SmallButton("Add Debug Error")) { AddLog("[error] something went wrong"); }
        ImGui::SameLine();
        if (ImGui::SmallButton("Clear")) { ClearLog(); }
        ImGui::SameLine();
        bool copy_to_clipboard = ImGui::SmallButton("Copy");
        //static float t = 0.0f; if (ImGui::GetTime() - t > 0.02f) { t = ImGui::GetTime(); AddLog("Spam %f", t); }

        ImGui::Separator();

        // Options menu
        if (ImGui::BeginPopup("Options"))
        {
            ImGui::Checkbox("Auto-scroll", &AutoScroll);
            ImGui::EndPopup();
        }

        // Options, Filter
        ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_O, ImGuiInputFlags_Tooltip);
        if (ImGui::Button("Options"))
            ImGui::OpenPopup("Options");
        ImGui::SameLine();
        Filter.Draw("Filter (\"incl,-excl\") (\"error\")", 180);
        ImGui::Separator();

        // Reserve enough left-over height for 1 separator + 1 input text
        ImGuiStyle& style = ImGui::GetStyle();
        const float footer_height_to_reserve = style.SeparatorSize + style.ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
        if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), ImGuiChildFlags_NavFlattened, ImGuiWindowFlags_HorizontalScrollbar))
        {
            if (ImGui::BeginPopupContextWindow())
            {
                if (ImGui::Selectable("Clear")) ClearLog();
                ImGui::EndPopup();
            }

            // Display every line as a separate entry so we can change their color or add custom widgets.
            // If you only want raw text you can use ImGui::TextUnformatted(log.begin(), log.end());
            // NB- if you have thousands of entries this approach may be too inefficient and may require user-side clipping
            // to only process visible items. The clipper will automatically measure the height of your first item and then
            // "seek" to display only items in the visible area.
            // To use the clipper we can replace your standard loop:
            //      for (int i = 0; i < Items.Size; i++)
            //   With:
            //      ImGuiListClipper clipper;
            //      clipper.Begin(Items.Size);
            //      while (clipper.Step())
            //         for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
            // - That your items are evenly spaced (same height)
            // - That you have cheap random access to your elements (you can access them given their index,
            //   without processing all the ones before)
            // You cannot this code as-is if a filter is active because it breaks the 'cheap random-access' property.
            // We would need random-access on the post-filtered list.
            // A typical application wanting coarse clipping and filtering may want to pre-compute an array of indices
            // or offsets of items that passed the filtering test, recomputing this array when user changes the filter,
            // and appending newly elements as they are inserted. This is left as a task to the user until we can manage
            // to improve this example code!
            // If your items are of variable height:
            // - Split them into same height items would be simpler and facilitate random-seeking into your list.
            // - Consider using manual call to IsRectVisible() and skipping extraneous decoration from your items.
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
            if (copy_to_clipboard)
                ImGui::LogToClipboard();
            for (const char* item : Items)
            {
                if (!Filter.PassFilter(item))
                    continue;

                // Normally you would store more information in your item than just a string.
                // (e.g. make Items[] an array of structure, store color/type etc.)
                ImVec4 color;
                bool has_color = false;
                if (strstr(item, "[error]")) { color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f); has_color = true; }
                else if (strncmp(item, "# ", 2) == 0) { color = ImVec4(1.0f, 0.8f, 0.6f, 1.0f); has_color = true; }
                if (has_color)
                    ImGui::PushStyleColor(ImGuiCol_Text, color);
                ImGui::TextUnformatted(item);
                if (has_color)
                    ImGui::PopStyleColor();
            }
            if (copy_to_clipboard)
                ImGui::LogFinish();

            // Keep up at the bottom of the scroll region if we were already at the bottom at the beginning of the frame.
            // Using a scrollbar or mouse-wheel will take away from the bottom edge.
            if (ScrollToBottom || (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
                ImGui::SetScrollHereY(1.0f);
            ScrollToBottom = false;

            ImGui::PopStyleVar();
        }
        ImGui::EndChild();
        ImGui::Separator();

        // Command-line
        bool reclaim_focus = false;
        ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;
        if (ImGui::InputText("Input", InputBuf, IM_COUNTOF(InputBuf), input_text_flags, &TextEditCallbackStub, (void*)this))
        {
            char* s = InputBuf;
            Strtrim(s);
            if (s[0])
                ExecCommand(s);
            strcpy_s(s, sizeof(s), "");
            reclaim_focus = true;
        }

        // Auto-focus on window apparition
        ImGui::SetItemDefaultFocus();
        if (reclaim_focus)
            ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget

        ImGui::End();
    }

    void    ExecCommand(const char* command_line)
    {
        AddLog("# %s\n", command_line);

        // Insert into history. First find match and delete it so it can be pushed to the back.
        // This isn't trying to be smart or optimal.
        HistoryPos = -1;
        for (int i = History.Size - 1; i >= 0; i--)
            if (Stricmp(History[i], command_line) == 0)
            {
                ImGui::MemFree(History[i]);
                History.erase(History.begin() + i);
                break;
            }
        History.push_back(Strdup(command_line));

        // Process command
        if (Stricmp(command_line, "CLEAR") == 0)
        {
            ClearLog();
        }
        else if (Stricmp(command_line, "HELP") == 0)
        {
            AddLog("Commands:");
            for (int i = 0; i < Commands.Size; i++)
                AddLog("- %s", Commands[i]);
        }
        else if (Stricmp(command_line, "HISTORY") == 0)
        {
            int first = History.Size - 10;
            for (int i = first > 0 ? first : 0; i < History.Size; i++)
                AddLog("%3d: %s\n", i, History[i]);
        }
        else
        {
            AddLog("Unknown command: '%s'\n", command_line);
        }

        // On command input, we scroll to bottom even if AutoScroll==false
        ScrollToBottom = true;
    }

    // In C++11 you'd be better off using lambdas for this sort of forwarding callbacks
    static int TextEditCallbackStub(ImGuiInputTextCallbackData* data)
    {
        ExampleAppConsole* console = (ExampleAppConsole*)data->UserData;
        return console->TextEditCallback(data);
    }

    int     TextEditCallback(ImGuiInputTextCallbackData* data)
    {
        //AddLog("cursor: %d, selection: %d-%d", data->CursorPos, data->SelectionStart, data->SelectionEnd);
        switch (data->EventFlag)
        {
        case ImGuiInputTextFlags_CallbackCompletion:
        {
            // Example of TEXT COMPLETION

            // Locate beginning of current word
            const char* word_end = data->Buf + data->CursorPos;
            const char* word_start = word_end;
            while (word_start > data->Buf)
            {
                const char c = word_start[-1];
                if (c == ' ' || c == '\t' || c == ',' || c == ';')
                    break;
                word_start--;
            }

            // Build a list of candidates
            ImVector<const char*> candidates;
            for (int i = 0; i < Commands.Size; i++)
                if (Strnicmp(Commands[i], word_start, (int)(word_end - word_start)) == 0)
                    candidates.push_back(Commands[i]);

            if (candidates.Size == 0)
            {
                // No match
                AddLog("No match for \"%.*s\"!\n", (int)(word_end - word_start), word_start);
            }
            else if (candidates.Size == 1)
            {
                // Single match. Delete the beginning of the word and replace it entirely so we've got nice casing.
                data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
                data->InsertChars(data->CursorPos, candidates[0]);
                data->InsertChars(data->CursorPos, " ");
            }
            else
            {
                // Multiple matches. Complete as much as we can..
                // So inputting "C"+Tab will complete to "CL" then display "CLEAR" and "CLASSIFY" as matches.
                int match_len = (int)(word_end - word_start);
                for (;;)
                {
                    int c = 0;
                    bool all_candidates_matches = true;
                    for (int i = 0; i < candidates.Size && all_candidates_matches; i++)
                        if (i == 0)
                            c = toupper(candidates[i][match_len]);
                        else if (c == 0 || c != toupper(candidates[i][match_len]))
                            all_candidates_matches = false;
                    if (!all_candidates_matches)
                        break;
                    match_len++;
                }

                if (match_len > 0)
                {
                    data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
                    data->InsertChars(data->CursorPos, candidates[0], candidates[0] + match_len);
                }

                // List matches
                AddLog("Possible matches:\n");
                for (int i = 0; i < candidates.Size; i++)
                    AddLog("- %s\n", candidates[i]);
            }

            break;
        }
        case ImGuiInputTextFlags_CallbackHistory:
        {
            // Example of HISTORY
            const int prev_history_pos = HistoryPos;
            if (data->EventKey == ImGuiKey_UpArrow)
            {
                if (HistoryPos == -1)
                    HistoryPos = History.Size - 1;
                else if (HistoryPos > 0)
                    HistoryPos--;
            }
            else if (data->EventKey == ImGuiKey_DownArrow)
            {
                if (HistoryPos != -1)
                    if (++HistoryPos >= History.Size)
                        HistoryPos = -1;
            }

            // A better implementation would preserve the data on the current input line along with cursor position.
            if (prev_history_pos != HistoryPos)
            {
                const char* history_str = (HistoryPos >= 0) ? History[HistoryPos] : "";
                data->DeleteChars(0, data->BufTextLen);
                data->InsertChars(0, history_str);
            }
        }
        }
        return 0;
    }
    void UE_LOG(const char* Format, ...) {
        int print_len = 0;
        char* str = new char[1024];
        va_list Args;
        va_start(Args, Format);

        while (*Format) {
            if (*Format == '%') {
                if (*(++Format)) {
                    
                    switch (*Format) {
                    case 'd':{
                        int value = va_arg(Args, int32);
                        char temp[20];
                        int temp_len = 0;
                        if (value == 0) {
                            temp[temp_len++] = '0';
                        }
                        else {
                            if (value < 0) {
                                str[print_len++] = '-';
                                value = -value;
                            }

                            while (value > 0) {
                                temp[temp_len++] = '0' + (value % 10);
                                value /= 10;
                            }

                            for (int i = temp_len - 1; i >= 0; i--) {
                                str[print_len++] = temp[i];
                            }
                        }
                        break;
                    }
                    case 'f':{
                        double value = va_arg(Args, double);
                        if (value < 0) {
                            str[print_len++] = '-';
                            value = -value;
                        }
                        int integerPart = (int)value;
                        double fractionalPart = value - integerPart;
                        char temp[20];
                        int temp_len = 0;

                        if (integerPart == 0) {
                            temp[temp_len++] = '0';
                        }
                        else {
                            while (integerPart > 0) {
                                temp[temp_len++] = '0' + (integerPart % 10);
                                integerPart /= 10;
                            }

                            for (int i = temp_len - 1; i >= 0; i--) {
                                str[print_len++] = temp[i];
                            }
                        }

                        str[print_len++] = '.';

                        for (int i = 0; i < 6; i++) {
                            fractionalPart *= 10.0f;

                            int digit = (int)fractionalPart;
                            str[print_len++] = '0' + digit;

                            fractionalPart -= digit;
                        }

                        break;
                    }
                    case 'c':{
                        char value = (char)va_arg(Args, int);
                        str[print_len++] = value;
                        break;
                    }
                    case 's':{
                        const char* value = va_arg(Args, const char*);
                        while (*value) {
                            str[print_len++] = *value++;
                        }
                        break;
                    }
                    case 'u':{
                        unsigned int value = va_arg(Args, unsigned int);
                        char temp[20];
                        int temp_len = 0;
                        if (value == 0) {
                            temp[temp_len++] = '0';
                        }
                        else {
                            while (value > 0) {
                                temp[temp_len++] = '0' + (value % 10);
                                value /= 10;
                            }

                            for (int i = temp_len - 1; i >= 0; i--) {
                                str[print_len++] = temp[i];
                            }
                        }
                        break;
                    }
                    case 'o':{
                        unsigned int value = va_arg(Args, unsigned int);

                        char temp[20];
                        int temp_len = 0;

                        if (value == 0) {
                            str[print_len++] = '0';
                        }
                        else {
                            while (value > 0) {
                                temp[temp_len++] = '0' + (value % 8);
                                value /= 8;
                            }
                            for (int i = temp_len - 1; i >= 0; i--) {
                                str[print_len++] = temp[i];
                            }
                        }
                        break;
                    }
                    case 'x':
                    case 'X':
                    {
                        unsigned int value = va_arg(Args, unsigned int);

                        char temp[20];
                        int temp_len = 0;

                        if (value == 0) {
                            str[print_len++] = '0';
                        }
                        else {
                            while (value > 0) {
                                int digit = value % 16;

                                if (digit < 10) {
                                    temp[temp_len++] = '0' + digit;
                                }
                                else {
                                    if(*Format == 'x')
                                        temp[temp_len++] = 'a' + digit - 10;
                                    else
                                        temp[temp_len++] = 'A' + digit - 10;
                                }

                                value /= 16;
                            }
                        }
                        for (int i = temp_len - 1; i >= 0; --i)
                        {
                            str[print_len++] = temp[i];
                        }
                        break;
                    }
                    default:
                    {
                        str[print_len++] = '%';
                        str[print_len++] = *Format;

                        break;
                    }
                    }
                }
                    
            }
            else   str[print_len++] = *Format;

            Format++;
        }
        str[print_len] = '\0';
        va_end(Args);
        AddLog(str);
        delete[] str;
        return;
    }
};

enum ETypeLine {
    ETL_LB,
    OTHER
};

class UPrimitive {
public:
    FVector Location;
    FVector Rotation;
    float Radius;
    float Mass;
    ETypePrimitive Type = ETypePrimitive::EPT_Cube;
    ETypeLine LineType = ETypeLine::OTHER;
    UPrimitive(FVector location, FVector rotation, float radius, ETypePrimitive type, ETypeLine etl = OTHER) : Location(location), Rotation(rotation), Radius(radius), LineType(etl) {
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
void DrawCreateWindow(ID3D11Buffer* vertexBufferCube, uint32 numVerticesCube)
{
    if (ImGui::Begin("Create"))
    {
        static float Lx = 0, Ly = 0, Lz = 0;
        static float Rx = 0, Ry = 0, Rz = 0;
        static float Sx = 0.1f, Sy = 0.1f, Sz = 0.1f;
        if (ImGui::Button("Spawn", ImVec2(50.0f, 0.0f))) {
            UCubeComp* obj = new UCubeComp();
            obj->Vertices = vertexBufferCube;
            obj->NumVertices = numVerticesCube;
            obj->RelativeLocation = FVector(Lx, Ly, Lz);
            obj->RelativeRotation = FVector(DegreeToRadian(Rx), DegreeToRadian(Ry), DegreeToRadian(Rz));
            obj->RelativeScale3D = FVector(Sx, Sy, Sz);
        }
        ImGui::SameLine();


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

        if (ImGui::Button("Delete", ImVec2(50.0f, 0.0f))) {
            // 버튼이 클릭되었을 때 실행할 코드
        }
    }

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

    // Renderer Class를 생성합니다.
    URenderer	renderer;

    // D3D11 생성하는 함수를 호출합니다.
    renderer.Create(hWnd, SCREEN_WIDTH, SCREEN_HEIGHT);
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
    uint32 numVerticesLine = sizeof(line_vertices) / sizeof(FVertexSimple);
    uint32 numVerticesTriangle = sizeof(triangle_vertices) / sizeof(FVertexSimple);
    uint32 numVerticesCube = sizeof(cube_vertices) / sizeof(FVertexSimple);
    uint32 numVerticesSphere = sizeof(sphere_vertices) / sizeof(FVertexSimple);

    ID3D11Buffer* vertexBufferLine = renderer.CreateVertexBuffer(line_vertices, sizeof(line_vertices));
    ID3D11Buffer* vertexBufferTriangle = renderer.CreateVertexBuffer(triangle_vertices, sizeof(triangle_vertices));
    ID3D11Buffer* vertexBufferCube = renderer.CreateVertexBuffer(cube_vertices, sizeof(cube_vertices));
    ID3D11Buffer* vertexBufferSphere = renderer.CreateVertexBuffer(sphere_vertices, sizeof(sphere_vertices));

    bool bIsExit = false;

    //FPS 제한을 위한 설정
    const int targetFPS = 30;
    const double targetFrameTime = 1000.0 / targetFPS; //한 프레임의 목표 시간(밀리초 단위)

    //고성능 타이머 초기화
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);

    LARGE_INTEGER startTime, endTime;
    double elapsedTime = 0.0;
    float degree = 100;
    
    //카메라
    UCameraComp* Camera = renderer.MainCamera;
    Camera->RelativeLocation = { 0,2,-2 };
    Camera->RelativeRotation = { DegreeToRadian(-45),DegreeToRadian(-135), 0 };
    Camera->RelativeRotation = { DegreeToRadian(0),DegreeToRadian(0), 0 };

    //좌표축
    UAxisGizmo* AxisGizmo = new UAxisGizmo();

    AxisGizmo->Vertices = vertexBufferLine;
    AxisGizmo->NumVertices = numVerticesLine;

    ExampleAppConsole Console;
    bool is_window_open = true;

    UCubeComp* Test1 = new UCubeComp();
    Test1->Vertices = vertexBufferCube;
    Test1->NumVertices = numVerticesCube;
    Test1->RelativeLocation = FVector(1, 0, 0);
    UCubeComp* Test2 = new UCubeComp();
    Test2->Vertices = vertexBufferCube;
    Test2->NumVertices = numVerticesCube;
    Test2->RelativeLocation = FVector(0, 1, 0);
    UCubeComp* Test3 = new UCubeComp();
    Test3->Vertices = vertexBufferCube;
    Test3->NumVertices = numVerticesCube;
    Test3->RelativeLocation = FVector(0, 0, 1);

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

        //picking
        bool bIsPicking = false;
        USceneComponent* pickedObjectPtr = nullptr;

        // camera forward
        FVector ZAxis(cos(Camera->RelativeRotation.y) * cos(Camera->RelativeRotation.x), sin(Camera->RelativeRotation.x), -sin(Camera->RelativeRotation.y) * cos(Camera->RelativeRotation.x));
        ZAxis.Normalize();
        // camera right
        FVector XAxis = FVector(0, 1, 0).Cross(ZAxis);
        XAxis.Normalize();
        // camera up
        FVector YAxis = ZAxis.Cross(XAxis);
        YAxis.Normalize();

        if (GetAsyncKeyState(VK_LEFT) & 0x8000 || GetAsyncKeyState(0x41) & 0x8000) { //왼쪽 (A)
            Camera->RelativeLocation.x -= XAxis.x * 0.01f;
            Camera->RelativeLocation.y -= XAxis.y * 0.01f;
            Camera->RelativeLocation.z -= XAxis.z * 0.01f;
            //Console.UE_LOG("%s %c %f %d %u %o %x","Hello",'A',3.14f,-100,100,100,255);
        }
        if (GetAsyncKeyState(VK_RIGHT) & 0x8000 || GetAsyncKeyState(0x44) & 0x8000) { //오른쪽 (D)
            Camera->RelativeLocation.x += XAxis.x * 0.01f;
            Camera->RelativeLocation.y += XAxis.y * 0.01f;
            Camera->RelativeLocation.z += XAxis.z * 0.01f;
        }
        if (GetAsyncKeyState(VK_UP) & 0x8000 || GetAsyncKeyState(0x57) & 0x8000) { //앞 (W)
            Camera->RelativeLocation.x += ZAxis.x * 0.01f;
            Camera->RelativeLocation.y += ZAxis.y * 0.01f;
            Camera->RelativeLocation.z += ZAxis.z * 0.01f;
        }
        if (GetAsyncKeyState(VK_DOWN) & 0x8000 || GetAsyncKeyState(0x53) & 0x8000) { //뒤 (S)
            Camera->RelativeLocation.x -= ZAxis.x * 0.01f;
            Camera->RelativeLocation.y -= ZAxis.y * 0.01f;
            Camera->RelativeLocation.z -= ZAxis.z * 0.01f;
        }
        if (GetAsyncKeyState(0x51) & 0x8000) { //위 (Q)
            Camera->RelativeLocation.y += 0.01f;
        }
        if (GetAsyncKeyState(0x45) & 0x8000) { //아래 (E)
            Camera->RelativeLocation.y -= 0.01f;
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

            // 좌우
            Camera->RelativeRotation.y += angleX;
            // 상하
            Camera->RelativeRotation.x += angleY;

            lastMousePos = currentMousePos;
        }
        else if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
        {
            // picking
            ScreenToClient(hWnd, &currentMousePos);
            float ndcX = 2 * currentMousePos.x / SCREEN_WIDTH - 1;  // screen xy to NDC xy
            float ndcY = 1 - 2 * currentMousePos.y / SCREEN_HEIGHT;

            pickedObjectPtr = UPicking::GetPickedPrimitive(ndcX, ndcY, Camera, ZAxis, XAxis, YAxis, GUObjectArray.GetAllObjects(), GUObjectArray.GetNum(), &bIsPicking);
        }
        else
        {
            isDragging = false;
            lastMousePos = currentMousePos;
        }

        const float RAD_TO_DEG = 180.0f / 3.14159265359f;

/*        // Pitch
        Camera->RelativeRotation.x = -atan2(
            CameraForward.y,
            sqrt(
                CameraForward.x * CameraForward.x +
                CameraForward.z * CameraForward.z
            )
        ) * RAD_TO_DEG;

        // Yaw
        Camera->RelativeRotation.y = atan2(
            CameraForward.x,
            CameraForward.z
        ) * RAD_TO_DEG;

        // Roll
        Camera->RelativeRotation.z = 0.0f; */

        //카메라 오류
        for (int i = 1; i < GUObjectArray.GetNum(); i++) {
            static_cast<UPrimitiveComponent*>(GUObjectArray.GetAllObjects()[i])->Render(&renderer);
        }

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        
        Console.Draw("Console Windows", &is_window_open);
        DrawCreateWindow(vertexBufferCube, numVerticesCube);
        DrawStatWindow();

        ImGui::Begin("picking test");
        if (pickedObjectPtr)
        {
            ImGui::Text("%f", pickedObjectPtr->RelativeLocation.x);
            ImGui::Text("%f", pickedObjectPtr->RelativeLocation.y);
            ImGui::Text("%f", pickedObjectPtr->RelativeLocation.z);
        }
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
    renderer.ReleaseConstantBuffer();
    renderer.ReleaseShader();
    renderer.Release();
    return 0;
}