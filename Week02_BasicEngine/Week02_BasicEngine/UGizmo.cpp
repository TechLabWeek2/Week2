#include "UGizmo.h"

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
    case ETypeTransform::Rotation:
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
    case ETypeTransform::Scale:
        switch (Axis)
        {
        case XAxis:
            RelativeRotation = Obj->RelativeRotation;
            break;
        case YAxis:
            RelativeRotation = Obj->RelativeRotation + FVector(0, 0, 1.57);
            break;
        case ZAxis:
            RelativeRotation = FVector(0, 1.57, 0) + Obj->RelativeRotation;
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
        Obj->RelativeRotation += MouseMove;
        break;
    case ETypeTransform::Scale:
        Obj->RelativeScale3D += MouseMove;
        break;
    }
}
