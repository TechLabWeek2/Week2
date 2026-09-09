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
#include "FSceneLoader.h"
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

    bool sceneChanged = false;

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
        newPrimitive->RelativeQ = newPrimitive->RelativeQ.FromEuler(newPrimitive->RelativeRotation);
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
        if (pickedPrimitivePtr != nullptr)
        {
            GUObjectArray.RemoveObj(pickedPrimitivePtr);
            pickedPrimitivePtr = nullptr;
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
    static std::string sceneMessage;

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
                const std::filesystem::path scenePath =
                    FSceneLoader::MakeScenePath(sceneName);

                if (!scenePath.empty())
                {
					pickedPrimitivePtr = nullptr;
                    sceneChanged = true;
                    FSceneLoader::NewScene(scenePath);
                }

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
                const std::filesystem::path scenePath =
                    FSceneLoader::MakeScenePath(sceneName);

                if (!scenePath.empty())
                {
                    FSceneLoader::SaveScene(scenePath);
                    pickedPrimitivePtr = nullptr;
                    sceneChanged = true;
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

    auto RefreshSceneFiles = [&]()
        {
            sceneFiles.clear();
            selectedScene = -1;
            sceneMessage.clear();

            try
            {
                sceneDirectory = FSceneLoader::GetSceneDirectory();

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
        const fs::path& selectedPath = sceneFiles[selectedScene];

        // 확장자를 제외한 이름을 UTF-8로 얻는다.
        const auto sceneNameUtf8 = selectedPath.stem().u8string();

        // 마지막 널 문자 공간까지 필요하다.
        if (sceneNameUtf8.size() >= sizeof(inputBuffer))
        {
            sceneMessage = "Scene name is too long.";
        }
        else if (FSceneLoader::LoadScene(
            selectedPath,
            MeshRegistry,
            &DefaultShader,
            *Camera))
        {
            std::memcpy(inputBuffer,sceneNameUtf8.data(), sceneNameUtf8.size());

            inputBuffer[sceneNameUtf8.size()] = '\0';
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

void DrawDetailsWindow(UPrimitiveComponent* selectedObject)
{
    ImGui::Begin("Details");
    
    if (selectedObject)
    {
        ETypePrimitive type = selectedObject->primitiveType;
        const char* typeNames[] = { "None", "Plane", "Cube", "Sphere", "Floor", "XLine", "YLine", "ZLine", "Max" };
        ImGui::Text("Primitive Type: %s", typeNames[(int32)type]);

        RasterizerState state = selectedObject->GetRasterizerState();
        const char* stateNames[] = { "Solid", "WireFrame","FrontCulling", "Solid_Culling_None", "WireFrame_FrontCulling" };
        if (ImGui::BeginCombo("Rasterizer State", stateNames[(int32)state]))
        {
            for (int32 i = 0; i <= (int32)RasterizerState::WireFrame_FrontCulling; i++)
            {
                bool selected = ((int32)state == i);
                if (ImGui::Selectable(stateNames[i], selected))
                {
                    state = (RasterizerState)i;
                }
                if (selected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            selectedObject->SetRasterizerState(state);
            ImGui::EndCombo();
        }

        BlendMode mode = selectedObject->GetBlendMode();
        const char* modeNames[] = { "Opaque", "Alpha" };
        if (ImGui::BeginCombo("Blend Mode", modeNames[(int32)mode]))
        {
            for (int32 i = 0; i <= (int32)BlendMode::Alpha; i++)
            {
                bool selected = ((int32)mode == i);
                if (ImGui::Selectable(modeNames[i], selected))
                {
                    mode = (BlendMode)i;
                }
                if (selected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            selectedObject->SetBlendMode(mode);
            ImGui::EndCombo();
        }

        ImGui::Separator();

        float LocationX = selectedObject->GetLocation().x;
        float LocationY = selectedObject->GetLocation().y;
        float LocationZ = selectedObject->GetLocation().z;

        ImGui::Text("Location");
        ImGui::SameLine(130);
        ImGui::SetNextItemWidth(100.0f);
        if (ImGui::DragFloat("##LocationX_input", &LocationX, 0.01f))
        {
            selectedObject->RelativeLocation.x = LocationX;
        }
        ImGui::SameLine();
        ImGui::SetNextItemWidth(100.0f);
        if (ImGui::DragFloat("##LocationY_input", &LocationY, 0.01f))
        {
            selectedObject->RelativeLocation.y = LocationY;
        }
        ImGui::SameLine();
        ImGui::SetNextItemWidth(100.0f);
        if (ImGui::DragFloat("##LocationZ_input", &LocationZ, 0.01f))
        {
            selectedObject->RelativeLocation.z = LocationZ;
        }

        float RotationX = selectedObject->RelativeRotation.x;
        float RotationY = selectedObject->RelativeRotation.y;
        float RotationZ = selectedObject->RelativeRotation.z;

        ImGui::Text("Rotation");
        ImGui::SameLine(130);
        ImGui::SetNextItemWidth(100.0f);
        bool rotationChanged = false;
        rotationChanged |= ImGui::DragFloat("##RotationX_input", &RotationX, 0.01f);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(100.0f);
        rotationChanged |= ImGui::DragFloat("##RotationY_input", &RotationY, 0.01f);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(100.0f);
        rotationChanged |= ImGui::DragFloat("##RotationZ_input", &RotationZ, 0.01f);

        if (rotationChanged)
        {
            selectedObject->RelativeRotation.x = RotationX;
            selectedObject->RelativeRotation.y = RotationY;
            selectedObject->RelativeRotation.z = RotationZ;
            selectedObject->RelativeQ = selectedObject->RelativeQ.FromEuler(selectedObject->RelativeRotation);
        }

        float ScaleX = selectedObject->RelativeScale3D.x;
        float ScaleY = selectedObject->RelativeScale3D.y;
        float ScaleZ = selectedObject->RelativeScale3D.z;

        ImGui::Text("Scale");
        ImGui::SameLine(130);
        ImGui::SetNextItemWidth(100.0f);
        if (ImGui::DragFloat("##ScaleX_input", &ScaleX, 0.01f))
        {
            selectedObject->RelativeScale3D.x = ScaleX;
        }
        ImGui::SameLine();
        ImGui::SetNextItemWidth(100.0f);
        if (ImGui::DragFloat("##ScaleY_input", &ScaleY, 0.01f))
        {
            selectedObject->RelativeScale3D.y = ScaleY;
        }
        ImGui::SameLine();
        ImGui::SetNextItemWidth(100.0f);
        if (ImGui::DragFloat("##ScaleZ_input", &ScaleZ, 0.01f))
        {
            selectedObject->RelativeScale3D.z = ScaleZ;
        }

        //ImGui::Separator();
    }
    else
    {
        ImGui::Text("Object not selected");
    }
    ImGui::End();
}