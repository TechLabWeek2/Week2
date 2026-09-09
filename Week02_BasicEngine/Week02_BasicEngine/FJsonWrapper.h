#pragma once
#include "../Week02_BasicEngine/Utils/json.hpp"
#include "FVector.h"

#include <cmath>
#include <stdexcept>
#include <string>
#include <utility>

#include "UPrimitiveComponent.h"
#include "FMeshResource.h"
#include "Core/Core.h"
#include "UGizmo.h"
#include "FMeshResourceRegistry.h"
#include "FPrimitiveFactory.h"
#include "TArray.h"
#include "UCameraComp.h"

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

	static Json SerializeVector4(const FVector4& value)
	{
		if (!std::isfinite(value.x) || !std::isfinite(value.y) || !std::isfinite(value.z))
		{
			throw std::invalid_argument("FVector contains non-finite values.");
		}
		return json::Array(value.x, value.y, value.z, value.w);
	}

	static FVector DeserializeVector3(const Json& jsonValue)
	{
		return FVector(
			GetNumber(jsonValue.at(0)),
			GetNumber(jsonValue.at(1)),
			GetNumber(jsonValue.at(2))
		);
	}

	static FVector4 DeserializeVector4(const Json& jsonValue)
	{
		return FVector4(
			GetNumber(jsonValue.at(0)),
			GetNumber(jsonValue.at(1)),
			GetNumber(jsonValue.at(2)),
			GetNumber(jsonValue.at(3))
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


	static Json SerializeCamera(const UCameraComp& Camera)
	{
		Json data = json::Object();
		data["Location"] = SerializeVector3(Camera.RelativeLocation);
		data["Rotation"] = SerializeVector3(Camera.RelativeRotation);
		data["Scale"] = SerializeVector3(Camera.RelativeScale3D);
		data["Quat"] = SerializeVector4(FVector4(Camera.RelativeQ.x, Camera.RelativeQ.y, Camera.RelativeQ.z, Camera.RelativeQ.w));
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
		data["Quat"] = SerializeVector4(FVector4(value->RelativeQ.x, value->RelativeQ.y, value->RelativeQ.z, value->RelativeQ.w));

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
		const FVector4 quat = DeserializeVector4(data.at("Quat"));

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
		camera.RelativeQ.x = quat.x;
		camera.RelativeQ.y = quat.y;
		camera.RelativeQ.z = quat.z;	
		camera.RelativeQ.w = quat.w;

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
		FQuat quat = FQuat(DeserializeVector4(data.at("Quat")).x, DeserializeVector4(data.at("Quat")).y, DeserializeVector4(data.at("Quat")).z, DeserializeVector4(data.at("Quat")).w);
		
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
		NewPrimitive->RelativeQ = quat;
		NewPrimitive->SetRasterizerState(RasterizerStateType);
		NewPrimitive->SetBlendMode(BlendModeState);
		NewPrimitive->SetModelColor(modelColor);
		NewPrimitive->SetUseColorFlag(useColor);


			
		return NewPrimitive;
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

	
};
