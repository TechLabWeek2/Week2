#pragma once
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/imgui_impl_dx11.h"
#include "imGui/imgui_impl_win32.h"
#include "UPrimitiveComponent.h"
#include "UCubeComp.h"
#include "USphereComp.h"
#include "UPlaneComp.h"
#include "UCameraComp.h"
#include "FJsonWrapper.h"
#include "FShaderResource.h"
#include "FMeshResourceRegistry.h"
#include "FPrimitiveFactory.h"

void DrawCreateWindow(UCameraComp* Camera, UPrimitiveComponent*& pickedPrimitivePtr, const FMeshResourceRegistry& MeshRegistry, double elapsedTime, double currentFPS)
{
    ImGui::Begin("Jungle Control Panel");
    ImGui::Text("Hello Jungle World!");
    ImGui::Text("FPS %.1f (%.3f ms)", (float)currentFPS, (float)elapsedTime);
    const char* typeNames[] = { "None", "Plane", "Cube", "Sphere", "Floor", "XLine", "YLine", "ZLine", "Max" };
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
    static float FOV = 90.0f;
    static float CLx = 0, CLy = 0, CLz = 0;
    static float CRx = 0, CRy = 0, CRz = 0;
    static int spawnNum = 1;
    if (ImGui::Button("Spawn", ImVec2(50.0f, 0.0f)))
    {
		UPrimitiveComponent* newPrimitive = FPrimitiveFactory::CreatePrimitive(current, &DefaultShader, MeshRegistry);
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
            MeshRegistry,
            &DefaultShader))
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
    ImGui::DragFloat("##FOV", &Camera->FOV, 0.1f, 0.0f, 120.f);
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