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


	static Json SerializePrimitive(UPrimitiveComponent* value)
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

	static UPrimitiveComponent* DeserializePrimitive(const Json& data, FMeshResource* cubeResource, FMeshResource* sphereResource,FMeshResource* PlaneResource,FShaderResource* shaderResource)
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

		FMeshResource* MeshResource = nullptr;

		switch (type)
		{
		case ETypePrimitive::Cube:
			MeshResource = cubeResource;
			break;

		case ETypePrimitive::Sphere:
			MeshResource = sphereResource;
			//newPrimitive = new USphereComp();
			break;

		case ETypePrimitive::Plane:
			MeshResource = PlaneResource;
			break;
		default:
			return nullptr; // 
		}
		if (MeshResource == nullptr)
			return nullptr;

		UPrimitiveComponent* NewPrimitive = new UPrimitiveComponent();

		// 생성 후 한 번에 적용
		NewPrimitive->primitiveType = type;
		NewPrimitive->RelativeLocation = location;
		NewPrimitive->RelativeRotation = rotation;
		NewPrimitive->RelativeScale3D = scale;
		NewPrimitive->SetRasterizerState(RasterizerStateType);
		NewPrimitive->SetBlendMode(BlendModeState);
		NewPrimitive->SetMeshResource(MeshResource);
		NewPrimitive->SetShaderResource(shaderResource); 
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
			if (dynamic_cast<UCameraComp*>(object) ||
				dynamic_cast<UAxisGizmo*>(object) ||
				dynamic_cast<UGizmo*>(object) || 
				dynamic_cast<UFloorComp*>(object))
			{
				continue;
			}

			if (dynamic_cast<UPrimitiveComponent*>(object))
			{
				GUObjectArray.RemoveObj(object);
			}
		}

		// 디버그용
		/*for (int i = 0; i < GUObjectArray.GetNum(); ++i)
		{
			UObject* object = GUObjectArray.GetAllObjects()[i];

			std::string message =
				"Remaining [" + std::to_string(i) + "] " +
				typeid(*object).name() + "\n";

			OutputDebugStringA(message.c_str());
		}*/

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

		for (int i = 1; i < GUObjectArray.GetNum(); i++) {
			
			std::string uuid =
				std::to_string(GUObjectArray.GetAllObjects()[i]->UUID);

			primitiveJson[uuid] =
				SerializePrimitive(static_cast<UPrimitiveComponent*>(GUObjectArray.GetAllObjects()[i]));
		}

		root["Primitives"] = primitiveJson;

		std::ofstream file(filename);

		if (!file.is_open())
			return false;

		file << root.dump();

		return true;
	}

	static bool LoadScene(const std::filesystem::path& filename, FMeshResource* cubeResource, FMeshResource* sphereResource, FMeshResource* PlaneResource,FShaderResource* shaderResource)
	{
		std::ifstream file(filename);

		if (!file.is_open())
			return false;

		std::stringstream buffer;
		buffer << file.rdbuf();

		json::JSON root =
			json::JSON::Load(buffer.str());

		int version =
			static_cast<int>(root.at("Version").ToInt());

		if (version != 1)
			return false;

		int nextUUID =
			static_cast<int>(root.at("NextUUID").ToInt());

		//현재 화면 Clear()
		for (int i = GUObjectArray.GetNum() - 1; i >=0; --i)
		{
			UObject* object = GUObjectArray.GetAllObjects()[i];
			if (dynamic_cast<UCameraComp*>(object) ||
				dynamic_cast<UAxisGizmo*>(object) ||
				dynamic_cast<UGizmo*>(object) ||
				dynamic_cast<UFloorComp*>(object))
			{
				continue;
			}

			if (dynamic_cast<UPrimitiveComponent*>(object))
			{
				GUObjectArray.RemoveObj(object);
			}
		}

		//scene.SetNextUUID(nextUUID);

		// TODO : buffer를 어떻게 땡겨올지, 다음 설계 pull 넘어온거 보고 생각하기
		// resource로 부터 넘어온 buffer를 받아서 등록
		// 클래스 구조 변화에 따른 타입도 신경
		// 팩토리 패턴 사용하기 - 후순위
		const auto& primitives =
			root.at("Primitives");

		for (const auto& [uuidString, primitiveData]
			: primitives.ObjectRange())
		{
			DeserializePrimitive(primitiveData, cubeResource, sphereResource, PlaneResource, shaderResource);

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
