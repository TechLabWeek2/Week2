#include "UGizmo.h"
#include "Core/Core.h"

UGizmo::UGizmo(ETypeAxis axis)
{
	primitiveType = ETypePrimitive::Gizmo;
	Class = StaticClass();
	bIsActive = false;
	Type = ETypeTransform::Location;
	Axis = axis;
}

void UGizmo::Update(UPrimitiveComponent* Obj)
{
    this->RelativeLocation = Obj->RelativeLocation;
    switch (Type) {
    case ETypeTransform::Location:
        switch (Axis)
        {
        case XAxis:
            RelativeRotation = FVector(0, 0, 0);
            break;
        case YAxis:
            RelativeRotation = FVector(0, 0, 1.57);
            break;
        case ZAxis:
            RelativeRotation = FVector(0, -1.57, 0);
            break;
        default:
            break;
        }
        break;
    case ETypeTransform::Rotation:
        switch (Axis)
        {
        case XAxis:
            RelativeRotation = FVector(PI/2, PI / 2, 0);
            break;
        case YAxis:
            RelativeRotation = FVector(PI / 2, 0, PI / 2);
            break;
        case ZAxis:
            RelativeRotation = FVector(0, 0, 0);
            break;
        default:
            break;
        }
        break;
    case ETypeTransform::Scale:
        switch (Axis)
        {
        case XAxis:
            RelativeRotation = Obj->RelativeRotation;
            break;
        case YAxis:
            RelativeRotation = Obj->RelativeRotation +FVector(0, 0, 1.57);
            break;
        case ZAxis:
        {
            RelativeRotation = FVector(0, -1.57, 0) - Obj->RelativeRotation;
             FMatrix ObjRot;
            ObjRot = FMatrix::Rotation(FVector(Obj->RelativeRotation));
            FMatrix Rot_90;
            Rot_90 = FMatrix::Rotation(FVector(0, -PI/2.f, 0));
            RelativeRotation = (ObjRot * Rot_90).GetEuler();
            //RelativeRotation = (Rot_90 * ObjRot).GetEuler();
            //RelativeRotation = Obj->RelativeRotation + FVector(0, -0.57, 0);
        };
            break;
        default:
            break;
        }
        break;
    }

}

void UGizmo::ObjUpdate(UPrimitiveComponent* Obj, FVector MouseMove)
{
    switch (Type) {
    case ETypeTransform::Location:
        Obj->RelativeLocation += MouseMove;
        break;
    case ETypeTransform::Rotation:
        Obj->RelativeRotation.x += MouseMove.x;
        Obj->RelativeRotation.y -= MouseMove.y;
        Obj->RelativeRotation.z += MouseMove.z;
        break;
    case ETypeTransform::Scale:
        Obj->RelativeScale3D += MouseMove;
        break;
    }
}
