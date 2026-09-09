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
                LocalGizmoAxis = (RelativeQ).RotateVector(FVector(1, 0, 0));
                break;
            case ETypeAxis::YAxis:
                LocalGizmoAxis = (RelativeQ * RelativeQ.FromEuler(FVector(0, 0, -PI / 2))).RotateVector(FVector(0, 1, 0));
                break;
            case ETypeAxis::ZAxis:
                LocalGizmoAxis = (RelativeQ * RelativeQ.FromEuler(FVector(0, PI / 2, 0))).RotateVector(FVector(0, 0, 1));
                break;
            }


            // 2. 기즈모 원점과 끝점
            FVector Origin = Obj->RelativeLocation;
            FVector End = Origin + Obj->RelativeQ.Inverse().RotateVector(LocalGizmoAxis) * 1.25f;


            // 3. 두 점을 Clip Space로 변환
            FVector OriginClip =
                Origin *
                Obj->GetQuatModelMatrix() *
                Camera->GetViewMatrix() *
                Camera->GetProjectionMatrix();

            FVector EndClip =
                End *
                Obj->GetQuatModelMatrix() *
                Camera->GetViewMatrix() *
                Camera->GetProjectionMatrix();


            // 5. 화면상의 기즈모 축 방향
            FVector2 ScreenAxis = FVector2(EndClip.x - OriginClip.x , EndClip.y - OriginClip.y);

            ScreenAxis.Normalize();


            // 6. 마우스 이동을 화면상의 기즈모 축으로 투영
            FVector2 MouseDelta(CurrentMousePos.x - LastMousePos.x, -CurrentMousePos.y + LastMousePos.y);
            FVector2 MouseDeltaN(CurrentMousePos.x - LastMousePos.x, -CurrentMousePos.y + LastMousePos.y);
            MouseDeltaN.Normalize();
            float theta = acos(MouseDelta.Dot(ScreenAxis) / MouseDelta.Length());
            if (MouseDelta.Length() < 0.0001f || Abs(acos(MouseDeltaN.Dot(ScreenAxis))-(PI/2)) < PI/4)    break;
            
            float Distance = MouseDelta.Length() / cos(theta);

            // 7. 화면 이동량을 월드 이동량으로 변환
            float WorldPerPixel = 0.005f;
            FQuat InverseQ = FQuat(
                -RelativeQ.x,
                -RelativeQ.y,
                -RelativeQ.z,
                RelativeQ.w
            );
            InverseQ.Normalize();
            FVector LocalMove =InverseQ.RotateVector(LocalGizmoAxis * Distance * WorldPerPixel);

            Obj->RelativeLocation += LocalGizmoAxis * Distance * WorldPerPixel;
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
            float reverse = LocalGizmoAxis.Dot(Camera->GetForwardVector_UE());
            
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
            if (reverse < 0) DeltaQuat.Inverse();
            DeltaQuat.Normalize();
            Obj->RelativeQ = Obj->RelativeQ * DeltaQuat;
            Obj->RelativeRotation = Obj->RelativeQ.ToEuler(Obj->RelativeQ);
            break;
        }
        case Scale: {
            Obj->RelativeScale3D += MouseMove;
            Obj->RelativeScale3D.x = max(0, Obj->RelativeScale3D.x);
            Obj->RelativeScale3D.y = max(0, Obj->RelativeScale3D.y);
            Obj->RelativeScale3D.z = max(0, Obj->RelativeScale3D.z);
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
        {
            switch (Axis)
            {
            case ETypeAxis::XAxis:
                LocalGizmoAxis = (RelativeQ).RotateVector(FVector(1, 0, 0));
                break;
            case ETypeAxis::YAxis:
                LocalGizmoAxis = (RelativeQ * RelativeQ.FromEuler(FVector(0, 0, -PI / 2))).RotateVector(FVector(0, 1, 0));
                break;
            case ETypeAxis::ZAxis:
                LocalGizmoAxis = (RelativeQ * RelativeQ.FromEuler(FVector(0, PI / 2, 0))).RotateVector(FVector(0, 0, 1));
                break;
            }
            // 2. 기즈모 원점과 끝점
            FVector Origin = Obj->RelativeLocation;
            FVector End = Origin + Obj->RelativeQ.Inverse().RotateVector(LocalGizmoAxis) * 1.25f;


            // 3. 두 점을 Clip Space로 변환
            FVector OriginClip =
                Origin *
                Obj->GetQuatModelMatrix() *
                Camera->GetViewMatrix() *
                Camera->GetProjectionMatrix();

            FVector EndClip =
                End *
                Obj->GetQuatModelMatrix() *
                Camera->GetViewMatrix() *
                Camera->GetProjectionMatrix();


            // 5. 화면상의 기즈모 축 방향
            FVector2 ScreenAxis = FVector2(EndClip.x / EndClip.z - OriginClip.x / OriginClip.z, EndClip.y / EndClip.z - OriginClip.y / OriginClip.z);

            ScreenAxis.Normalize();


            // 6. 마우스 이동을 화면상의 기즈모 축으로 투영
            FVector2 MouseDelta(CurrentMousePos.x - LastMousePos.x, -CurrentMousePos.y + LastMousePos.y);
            FVector2 MouseDeltaN(CurrentMousePos.x - LastMousePos.x, -CurrentMousePos.y + LastMousePos.y);
            MouseDeltaN.Normalize();
            float theta = acos(MouseDelta.Dot(ScreenAxis) / MouseDelta.Length());
            if (MouseDelta.Length() < 0.0001f || Abs(acos(MouseDeltaN.Dot(ScreenAxis)) - (PI / 2)) < PI / 4)    break;

            float Distance = MouseDelta.Length() / cos(theta);

            // 7. 화면 이동량을 월드 이동량으로 변환
            float WorldPerPixel = 0.005f;
            FQuat InverseQ = FQuat(
                -RelativeQ.x,
                -RelativeQ.y,
                -RelativeQ.z,
                RelativeQ.w
            );
            InverseQ.Normalize();
            FVector LocalMove = InverseQ.RotateVector(LocalGizmoAxis * Distance * WorldPerPixel);

            Obj->RelativeLocation += LocalGizmoAxis * Distance * WorldPerPixel;
        }
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
            Obj->RelativeRotation = Obj->RelativeQ.ToEuler(Obj->RelativeQ);
            break;
        }
        case ETypeTransform::Scale:
            Obj->RelativeScale3D += MouseMove;
            Obj->RelativeScale3D.x = max(0, Obj->RelativeScale3D.x);
            Obj->RelativeScale3D.y = max(0, Obj->RelativeScale3D.y);
            Obj->RelativeScale3D.z = max(0, Obj->RelativeScale3D.z);
            break;
        }
        break;
    default:
        break;
    }
}