#include "UGizmo.h"
#include "Core/Core.h"
#include "UCameraComp.h"
UGizmo::UGizmo(ETypeAxis axis)
{
	primitiveType = ETypePrimitive::Gizmo;
	Class = StaticClass();
	bIsActive = false;
	Type = ETypeTransform::Location;
	Axis = axis;
    Space = ETypeSpace::Local;
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
            break;
        case YAxis:
            Delta = Delta.FromEuler(FVector(0, 0, PI / 2));
            break;
        case ZAxis:
            Delta = Delta.FromEuler(FVector(0, -PI / 2, 0));
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
            break;
        case YAxis:
            Delta = Delta.FromEuler(FVector(0, PI / 2, PI / 2));
            break;
        case ZAxis:
            Delta = Delta.FromEuler(FVector(0, 0, 0));
            break;
        default:
            break;
        }
        break;
    case ETypeTransform::Scale:
        RelativeQ = Obj->RelativeQ;
        switch (Axis)
        {
        case XAxis:
            Delta = Delta.FromEuler(FVector(0, 0, 0));
            break;
        case YAxis:
            Delta = Delta.FromEuler(FVector(0, 0, PI / 2));
            break;
        case ZAxis:
            Delta = Delta.FromEuler(FVector(0, -PI / 2, 0));
            break;
        default:
            break;
        }
        break;
    }
    switch (Space)
    {
    case World:
        break;
    case Local:
        RelativeQ = Obj->RelativeQ;
        break;
    default:
        break;
    }
    RelativeQ = RelativeQ * Delta;

}

void UGizmo::ObjUpdate(UPrimitiveComponent* Obj, FVector MouseMove, POINT CurrentMousePos, POINT LastMousePos, float ScreenWidth, float ScreenHeight, UCameraComp* Camera)
{
    FVector LocalGizmoAxis;
    switch (Space)
    {
    case Local: {
        switch (Type)
        {
        case Location:{
            switch (Axis)
            {
            case ETypeAxis::XAxis:
                LocalGizmoAxis = RelativeQ.RotateVector(FVector(1, 0, 0));
                break;
            case ETypeAxis::YAxis:
                LocalGizmoAxis = (RelativeQ * RelativeQ.FromEuler(FVector(0, 0, PI / 2))).RotateVector(FVector(0, 1, 0));
                break;
            case ETypeAxis::ZAxis:
                LocalGizmoAxis = (RelativeQ * RelativeQ.FromEuler(FVector(0, -PI / 2, 0))).RotateVector(FVector(0, 0, 1));
                break;
            }
            float Distance = MouseMove.Dot(LocalGizmoAxis);
            Obj->RelativeLocation += LocalGizmoAxis * Distance;
            break;
        }
        case Rotation: {
            switch (Axis)
            {
            case ETypeAxis::XAxis:
                LocalGizmoAxis = RelativeQ.RotateVector(FVector(PI / 2, 0, PI / 2));
                break;
            case ETypeAxis::YAxis:
                LocalGizmoAxis = (RelativeQ * RelativeQ.FromEuler(FVector(0, PI / 2, PI / 2))).RotateVector(FVector(0, 1, 0));
                break;
            case ETypeAxis::ZAxis:
                LocalGizmoAxis = (RelativeQ * RelativeQ.FromEuler(FVector(0, 0, 0))).RotateVector(FVector(0, 0, 1));
                break;
            }
            float Angle = 0.0f;
            FQuat DeltaQuat;
            FVector CurrentObjPoint = Obj->RelativeLocation * Obj->GetQuatModelMatrix() * Camera->GetViewMatrix() * Camera->GetProjectionMatrix();
            float ObjX = CurrentObjPoint.x;
            float ObjY = CurrentObjPoint.y;

            float CurrentMouseX = 2.f * (float)CurrentMousePos.x / ScreenWidth - 1.f;
            float CurrentMouseY = 1.f - 2.f * (float)CurrentMousePos.y / ScreenHeight;

            float LastMouseX = 2.f * (float)LastMousePos.x / ScreenWidth - 1.f;
            float LastMouseY = 1.f - 2.f * (float)LastMousePos.y / ScreenHeight;

            // 중심 -> 마우스
            float CurrentX = CurrentMouseX - ObjX;
            float CurrentY = CurrentMouseY - ObjY;

            float LastX = LastMouseX - ObjX;
            float LastY = LastMouseY - ObjY;



            // 각도
            float Cross = LastX * CurrentY - LastY * CurrentX;
            float Dot = LastX * CurrentX + LastY * CurrentY;

            Angle = atan2(CurrentY, CurrentX) - atan2(LastY, LastX);

            switch (Axis)
            {
            case XAxis:
                DeltaQuat = FQuat(sin(Angle * 0.5f), 0.0f, 0.0f, cos(Angle * 0.5f));
                break;
            case YAxis:
                DeltaQuat = FQuat(0.0f, sin(Angle * 0.5f), 0.0f, cos(Angle * 0.5f));
                break;
            case ZAxis:
                DeltaQuat = FQuat(0.0f, 0.0f, sin(Angle * 0.5f), cos(Angle * 0.5f));
                break;
            default:
                break;
            }
            Obj->RelativeQ = Obj->RelativeQ * DeltaQuat;
            break;
        }
        case Scale: {
            Obj->RelativeScale3D += MouseMove;
            break;
        }
        default:
            break;
        }
    }
        break;
    case World:
        switch (Type) {
        case ETypeTransform::Location:
            Obj->RelativeLocation += MouseMove;
            break;
        case ETypeTransform::Rotation: {
            float Angle = 0.0f;
            FQuat DeltaQuat;
            FVector CurrentObjPoint = Obj->RelativeLocation * Obj->GetQuatModelMatrix() * Camera->GetViewMatrix() * Camera->GetProjectionMatrix();
            float ObjX = CurrentObjPoint.x;
            float ObjY = CurrentObjPoint.y;

            float CurrentMouseX = 2.f * (float)CurrentMousePos.x / ScreenWidth - 1.f;
            float CurrentMouseY = 1.f - 2.f * (float)CurrentMousePos.y / ScreenHeight;

            float LastMouseX = 2.f * (float)LastMousePos.x / ScreenWidth - 1.f;
            float LastMouseY = 1.f - 2.f * (float)LastMousePos.y / ScreenHeight;

            // 중심 -> 마우스
            float CurrentX = CurrentMouseX - ObjX;
            float CurrentY = CurrentMouseY - ObjY;

            float LastX = LastMouseX - ObjX;
            float LastY = LastMouseY - ObjY;



            // 각도
            float Cross = LastX * CurrentY - LastY * CurrentX;
            float Dot = LastX * CurrentX + LastY * CurrentY;

            Angle = atan2(CurrentY, CurrentX) - atan2(LastY, LastX);

            switch (Axis)
            {
            case XAxis:
                //Angle = DeltaX;
                DeltaQuat = FQuat(sin(Angle * 0.5f), 0.0f, 0.0f, cos(Angle * 0.5f));
                break;
            case YAxis:
                //Angle = DeltaX;
                DeltaQuat = FQuat(0.0f, sin(Angle * 0.5f), 0.0f, cos(Angle * 0.5f));
                break;
            case ZAxis:
                //Angle = DeltaY;
                DeltaQuat = FQuat(0.0f, 0.0f, sin(Angle * 0.5f), cos(Angle * 0.5f));
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
        break;
    default:
        break;
    }
}