#include "UObjectArray.h"
#include "UObject.h"

//전역 인스턴스
UObjectArray GUObjectArray;

/*void UObjectArray::RegisterObj(UObject* NewObj)
UObjectArray::~UObjectArray()
{
	Release();
}

void UObjectArray::RegisterObj(UObject* NewObj)
{
	if (NewObj == nullptr) return;

	//지금은 그냥 추가하지만 나중에 오브젝트 개수가 늘어난다면 TArray의 용량 초과시
	// 배열 재할당이 아니라 옆에 붙여서 넣어주는 식으로 추가 개발해야 한다.
	// 오브젝트 배열을 새로 만들게 되면 곤란할 거 같아서.

	int32 Index = 0;
	//사용 가능 인덱스가 비어있으면 그냥 추가
	if (AvailableIndices.IsEmpty())
	{
		Objects.Add(NewObj);
	}
	//사용 가능 인덱스가 있다면 그 인덱스 사용
	else
	{
		//사용 가능 인덱스 하나를 반환
		Index = AvailableIndices.Pop();
		Objects[Index] = NewObj;
	}

	NewObj->InternalIndex = Index;
}*/

UObjectArray::~UObjectArray()
{
}

void UObjectArray::RemoveObj(UObject* Obj)
{
	if (Obj == nullptr) return;
	Objects.Remove(Obj);
	delete Obj;
}

/*void UObjectArray::UnregisterObject(UObject* Obj)
{
	if (Obj == nullptr) return;

	Objects[Obj->InternalIndex] = nullptr;
	AvailableIndices.Add(Obj->InternalIndex);
}*/

int32 UObjectArray::GetNum() const
{
	return Objects.Num();
}

void UObjectArray::Release()
{
	for (int i = 0; i < Objects.Num(); i++)
	{
		if (Objects[i] == nullptr) continue;
		GUObjectArray.RemoveObj(Objects[i]);
		//delete Objects[i];
	}
}
