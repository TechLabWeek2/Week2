#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

#include <windows.h>

#include "FJsonWrapper.h"
#include "UEngineStatics.h"
#include "UObjectArray.h"
#include "TArray.h"

#include "UCameraComp.h"
#include "UPrimitiveComponent.h"
#include "UAxisGizmo.h"
#include "UGizmo.h"
#include "UFloorComp.h"
#include "UHighlightComp.h"

class FSceneLoader
{
	public:
		using Json = FJsonWrapper::Json;
	public:

		static bool NewScene(const std::filesystem::path& filename)
		{

			for (int i = GUObjectArray.GetNum() - 1; i >= 0; --i)
			{
				UObject* object = GUObjectArray.GetAllObjects()[i];
				
				if (object == nullptr)
				{
					continue;
				}

				if (object->IsA(UCameraComp::StaticClass()) ||
					IsEditorHelper(object))
				{
					continue;
				}

				if (object->IsA(UPrimitiveComponent::StaticClass()))
				{
					GUObjectArray.RemoveObj(object);
				}
			}



			json::JSON root = json::Object();
			root["Version"] = 1;
			root["NextUUID"] = UEngineStatics::GetNextUUID();
			json::JSON primitiveJson = json::Object();
			root["Primitives"] = primitiveJson;
			std::ofstream file(filename);
			if (!file.is_open())
				return false;
			file << root.dump();
			return true;
		}

		static bool SaveScene(const std::filesystem::path& filename, const Json& sceneData = nullptr)
		{
			json::JSON root = json::Object();

			root["Version"] = 1;
			root["NextUUID"] = UEngineStatics::GetNextUUID();

			json::JSON primitiveJson = json::Object();

			for (int i = 0; i < GUObjectArray.GetNum(); i++) {


				UObject* object = GUObjectArray.GetAllObjects()[i];

				std::string uuid =
					std::to_string(object->UUID);


				if (object->IsA(UCameraComp::StaticClass()))
				{
					auto* camera = static_cast<UCameraComp*>(object);
					primitiveJson[uuid] = FJsonWrapper::SerializeCamera(*camera);
				}
				else if (IsEditorHelper(object))
				{
					continue;
				}
				else if (object->IsA(UPrimitiveComponent::StaticClass()))
				{
					auto* primitive = static_cast<UPrimitiveComponent*>(object);
					primitiveJson[uuid] = FJsonWrapper::SerializePrimitive(primitive);
				}
			}

			root["Primitives"] = primitiveJson;

			std::ofstream file(filename);

			if (!file.is_open())
				return false;

			file << root.dump();

			return true;
		}

		static bool LoadScene(const std::filesystem::path& filename, const FMeshResourceRegistry& MeshRegistry, FShaderResource* shaderResource, UCameraComp& camera)
		{

			TArray<UPrimitiveComponent*> oldPrimitives;
			TArray<UPrimitiveComponent*> newPrimitives;

			try
			{
				std::ifstream file(filename);

				if (!file.is_open())
				{
					return false;
				}

				std::stringstream buffer;
				buffer << file.rdbuf();

				const Json root = Json::Load(buffer.str());
				const Json& version = root.at("Version");

				const Json& primitives = root.at("Primitives");



				// 새 객체 생성 전에 기존 프리미티브 목록을 보관한다.
				for (int32 i = 0; i < GUObjectArray.GetNum(); ++i)
				{
					UObject* object = GUObjectArray.GetAllObjects()[i];

					if (object == nullptr || IsEditorHelper(object))
					{
						continue;
					}

					if (object->IsA(UPrimitiveComponent::StaticClass()))
					{
						oldPrimitives.Add(
							static_cast<UPrimitiveComponent*>(object));
					}
				}

				// 생성한 객체를 기록할 슬롯을 미리 확보한다.
				// TArray::Resize()가 포인터 원소를 nullptr로 초기화한다.
				int32 entryCount = 0;

				for (const auto& entry : primitives.ObjectRange())
				{
					++entryCount;
				}

				newPrimitives.Resize(entryCount);

				int32 createdCount = 0;
				const Json* cameraData = nullptr;

				for (const auto& [uuid, data] : primitives.ObjectRange())
				{
					if (data.JSONType() != Json::Class::Object)
					{
						throw std::invalid_argument(
							"Scene entry must be an object.");
					}

					const Json& typeValue = data.at("Type");

					if (typeValue.JSONType() != Json::Class::String)
					{
						throw std::invalid_argument(
							"Type must be a string.");
					}

					const std::string typeName = typeValue.ToString();

					// 카메라는 생성하지 않고 데이터를 보관한다.
					if (typeName == "Camera")
					{
						if (cameraData != nullptr)
						{
							throw std::invalid_argument(
								"Only one editor camera is supported.");
						}

						cameraData = &data;
						continue;
					}

					const ETypePrimitive type = FJsonWrapper::FromString(typeName);

					// 이전 파일에 포함된 에디터 보조 객체는 유지한다.
					if (type == ETypePrimitive::Floor ||
						type == ETypePrimitive::XLine ||
						type == ETypePrimitive::YLine ||
						type == ETypePrimitive::ZLine ||
						type == ETypePrimitive::Gizmo)
					{
						continue;
					}

					UPrimitiveComponent* primitive =
						FJsonWrapper::DeserializePrimitive(
							data,
							shaderResource,
							MeshRegistry);

					if (primitive == nullptr)
					{
						throw std::runtime_error(
							"Failed to create primitive: " + typeName);
					}

					// Resize로 확보했으므로 Add 대신 인덱스로 기록한다.
					newPrimitives[createdCount++] = primitive;
				}

				// 모든 프리미티브 생성 성공 후 기존 카메라에 적용한다.
				// 카메라 데이터가 없는 파일은 현재 카메라를 유지한다.
				if (cameraData != nullptr)
				{
					FJsonWrapper::DeserializeCamera(*cameraData, camera);
				}
			}
			catch (const std::exception& error)
			{
				// 실패하면 이번 로드에서 생성한 객체만 제거한다.
				for (int32 i = 0; i < newPrimitives.Num(); ++i)
				{
					UPrimitiveComponent* primitive = newPrimitives[i];

					if (primitive != nullptr)
					{
						GUObjectArray.RemoveObj(primitive);
					}
				}

				OutputDebugStringA(error.what());
				OutputDebugStringA("\n");

				return false;
			}

			// 복원 준비가 끝난 후 기존 프리미티브를 제거한다.
			for (int32 i = 0; i < oldPrimitives.Num(); ++i)
			{
				GUObjectArray.RemoveObj(oldPrimitives[i]);
			}



			return true;
		}

		

		static std::filesystem::path FindDirectory(const wchar_t* dir)
		{
			namespace fs = std::filesystem;

			fs::path directory = fs::current_path();

			while (!directory.empty())
			{
				for (const auto& entry : fs::directory_iterator(directory))
				{
					if (entry.is_regular_file() &&
						entry.path().extension() == dir)
					{
						return directory;
					}
				}

				fs::path parent = directory.parent_path();

				if (parent == directory)
					break;

				directory = parent;
			}

			return {};
		}


	private:
		static bool IsEditorHelper(UObject* object)
		{
			return dynamic_cast<UAxisGizmo*>(object) != nullptr
				|| dynamic_cast<UGizmo*>(object) != nullptr
				|| dynamic_cast<UFloorComp*>(object) != nullptr
				|| dynamic_cast<UHighlightComp*>(object) != nullptr;
		}
};