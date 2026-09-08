#pragma once
#include "USceneComponent.h"
#include "URenderer.h"

enum class ETypePrimitive : uint8
{
	None,
	Plane,
	Cube,
	Sphere,
	Floor,
	XLine,
	YLine,
	ZLine,
	Gizmo,
	Max
};


class FMeshResource;
class URenderer;

class UPrimitiveComponent :
    public USceneComponent
{
    using Super = USceneComponent;

public:
    ETypePrimitive primitiveType;

	ID3D11Buffer* Vertices;
	int32 NumVertices;

    UPrimitiveComponent();

    void Update(float deltaTime) override;

    FMeshResource* GetMeshResource() const;
    void SetMeshResource(FMeshResource* pMeshResource);

	static UClass* StaticClass() {
		static UClass Class("UPrimitiveComponent", USceneComponent::StaticClass());
		return &Class;
	}

	RasterizerState GetRasterizerState()const;
	void SetRasterizerState(RasterizerState StateType);

	BlendMode GetBlendMode()const;
	void SetBlendMode(BlendMode NewBlendMode);

	const float* GetModelColor() const;
	void SetModelColor(const float NewColor[4]);
	void SetModelColor(const TArray<float>& NewColor);

	bool GetUseColorFlag()const;
	void SetUseColorFlag(bool pUseColorFlag);

private:
    //메시 데이터
    FMeshResource* MeshResource = nullptr;

	RasterizerState RasterizerStateType = RasterizerState::Solid;

	BlendMode BlendModeState = BlendMode::Opaque;

	float ModelColor[4] = { 1.f, 1.f, 1.f, 1.f };

	bool UseColor = false;
};