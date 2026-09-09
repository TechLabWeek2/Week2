#include "UGizmo.h"
#include "Core/Core.h"
#include "FGraphicsDevice.h"
UGizmo::UGizmo(ETypeAxis axis)
{
	primitiveType = ETypePrimitive::Gizmo;
	Class = StaticClass();
	bIsActive = false;
	Type = ETypeTransform::Location;
	Axis = axis;
}

void UGizmo::Update(UPrimitiveComponent* Obj, USceneComponent* Camera)
{
    this->RelativeLocation = Obj->RelativeLocation;
    float CameraToGizmo = (RelativeLocation - Camera->RelativeLocation).Size();

    this->RelativeScale3D = FVector(0.1f, 0.1f, 0.1f) * CameraToGizmo;

    RelativeQ = FQuat();
    FQuat Delta;
    switch (Type) {
    case ETypeTransform::Location:
        switch (Axis)
        {
        case XAxis:
            Delta = Delta.FromEuler(FVector(0, 0, 0));
            RelativeQ = RelativeQ * Delta;
            break;
        case YAxis:
            Delta = Delta.FromEuler(FVector(0, 0, PI / 2));
            RelativeQ = RelativeQ * Delta;
            break;
        case ZAxis:
            Delta = Delta.FromEuler(FVector(0, -PI / 2, 0));
            RelativeQ = RelativeQ * Delta;
            break;
        default:
            break;
        }
        break;
    case ETypeTransform::Rotation:
        switch (Axis)
        {
        case XAxis:
            Delta = Delta.FromEuler(FVector(PI / 2, 0, PI / 2));
            RelativeQ = RelativeQ * Delta;
            break;
        case YAxis:
            Delta = Delta.FromEuler(FVector(0, PI / 2, PI / 2));
            RelativeQ = RelativeQ * Delta;
            break;
        case ZAxis:
            Delta = Delta.FromEuler(FVector(0, 0, 0));
            RelativeQ = RelativeQ * Delta;
            break;
        default:
            break;
        }
        break;
    case ETypeTransform::Scale:
        switch (Axis)
        {
        case XAxis:
            RelativeQ = Obj->RelativeQ;
            Delta = Delta.FromEuler(FVector(0, 0, 0));
            RelativeQ = RelativeQ * Delta;
            break;
        case YAxis:
            RelativeQ = Obj->RelativeQ;
            Delta = Delta.FromEuler(FVector(0, 0, PI / 2));
            RelativeQ = RelativeQ * Delta;
            break;
        case ZAxis:
            RelativeQ = Obj->RelativeQ;
            Delta = Delta.FromEuler(FVector(0, -PI / 2, 0));
            RelativeQ = RelativeQ * Delta;
            break;
        default:
            break;
        }
        break;
    }

}

void UGizmo::ObjUpdate(UPrimitiveComponent* Obj, FVector MouseMove, float DeltaX, float DeltaY)
{
    switch (Type) {
    case ETypeTransform::Location:
        Obj->RelativeLocation += MouseMove;
        break;
    case ETypeTransform::Rotation: {
        float Angle = 0.0f;
        FQuat DeltaQuat = FQuat(0, 0, 0, 1);

        switch (Axis)
        {
        case XAxis:
            Angle = DeltaX;
            DeltaQuat = FQuat(sin(Angle * 0.5f), 0.0f, 0.0f, cos(Angle * 0.5f));
            Console.UE_LOG("X");
            break;
        case YAxis:
            Angle = DeltaX;
            DeltaQuat = FQuat(0.0f, sin(Angle * 0.5f), 0.0f, cos(Angle * 0.5f));
            Console.UE_LOG("Y");
            break;
        case ZAxis:
            Angle = DeltaY;
            DeltaQuat = FQuat(0.0f, 0.0f, sin(Angle * 0.5f), cos(Angle * 0.5f));
            Console.UE_LOG("Z");
            break;
        default:
            break;
        }
        Obj->RelativeQ = DeltaQuat * Obj->RelativeQ;
        break;
    }
    case ETypeTransform::Scale:
        Obj->RelativeScale3D += MouseMove;
        break;
    }
}