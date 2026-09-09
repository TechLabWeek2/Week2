#pragma once
#include "../Week02_BasicEngine/Utils/json.hpp"
#include "FVector.h"

#include <cmath>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <utility>

#include "UPrimitiveComponent.h"
#include "UEngineStatics.h"
#include "FMeshResource.h"
#include "UObjectArray.h"
#include "Core/Core.h"
#include "UAxisGizmo.h"
#include "UFloorComp.h"
#include "UGizmo.h"
#include "FMeshResourceRegistry.h"
#include "FPrimitiveFactory.h"
#include "TArray.h"
#include "UCameraComp.h"
#include "UHighlightComp.h"

class FJsonWrapper
{
public:
	using Json = json::JSON;


	static Json MakeObject()
	{
		return json::Object();
	}

	//Fvector를 Json으로 변환하는 함수
	static Json SerializeVector3(const FVector& value)
	{
		if (!std::isfinite(value.x) || !std::isfinite(value.y) || !std::isfinite(value.z))
		{
			throw std::invalid_argument("FVector contains non-finite values.");
		}
		return json::Array(value.x, value.y, value.z);
	}

	static FVector DeserializeVector3(const Json& jsonValue)
	{
		return FVector(
			GetNumber(jsonValue.at(0)),
			GetNumber(jsonValue.at(1)),
			GetNumber(jsonValue.at(2))
		);
	}

	static float GetNumber(const Json& jsonValue)
	{
		if (jsonValue.JSONType() == Json::Class::Floating)
		{
			return static_cast<float>(jsonValue.ToFloat());
		}
		else if (jsonValue.JSONType() == Json::Class::Integral)
		{
			return static_cast<float>(jsonValue.ToInt());
		}
		else
		{
			throw std::invalid_argument("JSON value is not a number.");
		}
	}

	static bool IsEditorHelper(UObject* object)
	{
		return dynamic_cast<UAxisGizmo*>(object) != nullptr
			|| dynamic_cast<UGizmo*>(object) != nullptr
			|| dynamic_cast<UFloorComp*>(object) != nullptr 
			|| dynamic_cast<UHighlightComp*>(object) != nullptr;
	}

	static Json SerializeCamera(const UCameraComp& Camera)
	{
		Json data = json::Object();
		data["Location"] = SerializeVector3(Camera.RelativeLocation);
		data["Rotation"] = SerializeVector3(Camera.RelativeRotation);
		data["Scale"] = SerializeVector3(Camera.RelativeScale3D);
		data["Type"] = "Camera";
		data["FOV"] = Camera.FOV;
		data["AspectRatio"] = Camera.AspectRatio;
		data["NearClip"] = Camera.NearClip;
		data["FarClip"] = Camera.FarClip;
		data["ZoomLevel"] = Camera.ZoomLevel;
		data["IsOrthogonal"] = Camera.IsOrthogonal;
	
		return data;
	}

	static Json SerializePrimitive(UPrimitiveComponent* value = nullptr)
	{
		

		Json data = json::Object();
		data["Location"] = SerializeVector3(value->RelativeLocation);
		data["Rotation"] = SerializeVector3(value->RelativeRotation);
		data["Scale"] = SerializeVector3(value->RelativeScale3D);

		if( value->primitiveType == ETypePrimitive::None ||
			value->primitiveType == ETypePrimitive::Plane ||
			value->primitiveType == ETypePrimitive::Cube ||
			value->primitiveType == ETypePrimitive::Sphere ||
			value->primitiveType == ETypePrimitive::Floor ||
			value->primitiveType == ETypePrimitive::XLine ||
			value->primitiveType == ETypePrimitive::YLine ||
			value->primitiveType == ETypePrimitive::ZLine ||
			value->primitiveType == ETypePrimitive::Gizmo ||
			value->primitiveType == ETypePrimitive::Max )
		{
			
			data["Type"] = ToString(value->primitiveType);
		}
		else
		{
			throw std::invalid_argument("Invalid primitive type.");
		}

		if(value->GetRasterizerState() == RasterizerState::Solid)
		{
			data["RasterizerState"] = "RasterizerState::Solid";
		}
		else if(value->GetRasterizerState() == RasterizerState::WireFrame)
		{
			data["RasterizerState"] = "RasterizerState::WireFrame";
		}
		else if(value->GetRasterizerState() == RasterizerState::FrontCulling)
		{
			data["RasterizerState"] = "RasterizerState::FrontCulling";
		}
		else if (value->GetRasterizerState() == RasterizerState::Solid_Culling_None)
		{
			data["RasterizerState"] = "RasterizerState::Solid_Culling_None";
		}
		else if(value->GetRasterizerState() == RasterizerState::WireFrame_FrontCulling)
		{
			data["RasterizerState"] = "RasterizerState::WireFrame_FrontCulling";
		}
		else
		{
			throw std::invalid_argument("Invalid RasterizerState type.");
		}


		if (value->GetBlendMode() == BlendMode::Opaque)
		{
			data["BlendMode"] = "BlendMode::Opaque";
		}
		else if (value->GetBlendMode() == BlendMode::Alpha)
		{
			data["BlendMode"] = "BlendMode::Alpha";
		}
		else
		{
			throw std::invalid_argument("Invalid BlendMode type.");
		}

		const float* modelColor = value->GetModelColor();
		data["ModelColor"] = json::Array(modelColor[0], modelColor[1], modelColor[2], modelColor[3]);
		data["UseColor"] = value->GetUseColorFlag();

		return data;
	}


	static void DeserializeCamera(const Json& data, UCameraComp& camera)
	{
		// 먼저 모두 읽고 검사한 뒤 기존 카메라에 적용한다.
		const FVector location = DeserializeVector3(data.at("Location"));
		const FVector rotation = DeserializeVector3(data.at("Rotation"));
		const FVector scale = DeserializeVector3(data.at("Scale"));

		const float fov = GetNumber(data.at("FOV"));
		const float aspectRatio = GetNumber(data.at("AspectRatio"));
		const float nearClip = GetNumber(data.at("NearClip"));
		const float farClip = GetNumber(data.at("FarClip"));
		const float zoomLevel = GetNumber(data.at("ZoomLevel"));

		const Json& orthogonal = data.at("IsOrthogonal");

		if (orthogonal.JSONType() != Json::Class::Boolean)
		{
			throw std::invalid_argument("IsOrthogonal must be boolean.");
		}

		if (fov <= 0.f || fov >= 180.f ||
			aspectRatio <= 0.f ||
			nearClip <= 0.f ||
			farClip <= nearClip ||
			zoomLevel <= 0.f)
		{
			throw std::invalid_argument("Invalid camera projection values.");
		}

		camera.RelativeLocation = location;
		camera.RelativeRotation = rotation;
		camera.RelativeScale3D = scale;

		camera.FOV = fov;
		camera.AspectRatio = aspectRatio;
		camera.NearClip = nearClip;
		camera.FarClip = farClip;
		camera.ZoomLevel = zoomLevel;
		camera.IsOrthogonal = orthogonal.ToBool();
	}

	static UPrimitiveComponent* DeserializePrimitive(const Json& data,FShaderResource* shaderResource, const FMeshResourceRegistry& MeshRegistry)
	{

		FVector location = DeserializeVector3(data.at("Location"));
		FVector rotation = DeserializeVector3(data.at("Rotation"));
		FVector scale = DeserializeVector3(data.at("Scale"));
		
		//나중에 FString으로 바꿀수 있으면
		std::string typeString = data.at("Type").ToString();
		std::string typeRasterizer = data.at("RasterizerState").ToString();
		std::string typeBlend = data.at("BlendMode").ToString();
		ETypePrimitive type = FromString(typeString);
		RasterizerState RasterizerStateType = RasterizerStateFromString(typeRasterizer);
		BlendMode BlendModeState = BlendModeFromString(typeBlend);


		TArray<float> modelColor = { 1.f, 1.f, 1.f, 1.f };
		bool useColor = false;

		const Json& color = data.at("ModelColor");
		for (int i = 0; i < 4; ++i)
		{
			modelColor[i] = GetNumber(color.at(i));
		}

		const Json& flag = data.at("UseColor");
		useColor = flag.ToBool();

		FMeshResource* MeshResource = MeshRegistry.GetMeshResource(type);

		if (MeshResource == nullptr)
			return nullptr;

		UPrimitiveComponent* NewPrimitive = FPrimitiveFactory::CreatePrimitive(type, shaderResource, MeshRegistry);

		// 생성 후 한 번에 적용
		NewPrimitive->primitiveType = type;
		NewPrimitive->RelativeLocation = location;
		NewPrimitive->RelativeRotation = rotation;
		NewPrimitive->RelativeScale3D = scale;
		NewPrimitive->SetRasterizerState(RasterizerStateType);
		NewPrimitive->SetBlendMode(BlendModeState);
		NewPrimitive->SetModelColor(modelColor);
		NewPrimitive->SetUseColorFlag(useColor);


			
		return NewPrimitive;
	}

	static bool NewScene(const std::filesystem::path& filename )
	{

		for (int i = GUObjectArray.GetNum() - 1; i >= 0; --i)
		{
			UObject* object = GUObjectArray.GetAllObjects()[i];
			// 카메라와 축만 보존
			if (dynamic_cast<UCameraComp*>(object) != nullptr ||
				IsEditorHelper(object))
			{
				continue;
			}

			if (dynamic_cast<UPrimitiveComponent*>(object))
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
				primitiveJson[uuid] = SerializeCamera(*camera);
			}
			else if (IsEditorHelper(object))
			{
				continue;
			}
			else if (object->IsA(UPrimitiveComponent::StaticClass()))
			{
				auto* primitive = static_cast<UPrimitiveComponent*>(object);
				primitiveJson[uuid] = SerializePrimitive(primitive);
			}
		}

		root["Primitives"] = primitiveJson;

		std::ofstream file(filename);

		if (!file.is_open())
			return false;

		file << root.dump();

		return true;
	}

	static bool LoadScene(const std::filesystem::path& filename, const FMeshResourceRegistry& MeshRegistry,FShaderResource* shaderResource, UCameraComp& camera)
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

				const ETypePrimitive type = FromString(typeName);

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
					DeserializePrimitive(
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
				DeserializeCamera(*cameraData, camera);
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

	static std::string ToString(ETypePrimitive type) 
	{
			switch (type) 
			{
				case ETypePrimitive::None:     return "ETypePrimitive::None";
				case ETypePrimitive::Plane: return "ETypePrimitive::Plane";
				case ETypePrimitive::Cube:     return "ETypePrimitive::Cube";
				case ETypePrimitive::Sphere:   return "ETypePrimitive::Sphere";
				case ETypePrimitive::Floor:   return "ETypePrimitive::Floor";
				case ETypePrimitive::XLine:    return "ETypePrimitive::XLine";
				case ETypePrimitive::YLine:    return "ETypePrimitive::YLine";
				case ETypePrimitive::ZLine:    return "ETypePrimitive::ZLine";
				case ETypePrimitive::Gizmo:    return "ETypePrimitive::Gizmo";
				case ETypePrimitive::Max:      return "ETypePrimitive::Max";
				default:           return "Unknown";
			}
	}

	static ETypePrimitive FromString(const std::string typeString)
	{
		if (typeString == "ETypePrimitive::None")     return ETypePrimitive::None;
		if (typeString == "ETypePrimitive::Plane") return ETypePrimitive::Plane;
		if (typeString == "ETypePrimitive::Cube")     return ETypePrimitive::Cube;
		if (typeString == "ETypePrimitive::Sphere")   return ETypePrimitive::Sphere;
		if (typeString == "ETypePrimitive::Floor")   return ETypePrimitive::Floor;
		if (typeString == "ETypePrimitive::XLine")    return ETypePrimitive::XLine;
		if (typeString == "ETypePrimitive::YLine")    return ETypePrimitive::YLine;
		if (typeString == "ETypePrimitive::ZLine")    return ETypePrimitive::ZLine;
		if (typeString == "ETypePrimitive::Gizmo")    return ETypePrimitive::Gizmo;
		if (typeString == "ETypePrimitive::Max")      return ETypePrimitive::Max;
		throw std::invalid_argument("Invalid primitive type string: " + typeString);
	}
	
	static RasterizerState RasterizerStateFromString(const std::string& stateString)
	{
		if (stateString == "RasterizerState::Solid")      return RasterizerState::Solid;
		if (stateString == "RasterizerState::WireFrame")      return RasterizerState::WireFrame;
		if (stateString == "RasterizerState::FrontCulling")      return RasterizerState::FrontCulling;
		if (stateString == "RasterizerState::Solid_Culling_None")      return RasterizerState::Solid_Culling_None;
		if (stateString == "RasterizerState::WireFrame_FrontCulling") return RasterizerState::WireFrame_FrontCulling;
		
		throw std::invalid_argument("Invalid rasterizer state string: " + stateString);
	}

	static BlendMode BlendModeFromString(const std::string& stateString)
	{
		if (stateString == "BlendMode::Opaque")      return BlendMode::Opaque;
		if (stateString == "BlendMode::Alpha")      return BlendMode::Alpha;
		throw std::invalid_argument("Invalid blend mode string: " + stateString);
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
};
