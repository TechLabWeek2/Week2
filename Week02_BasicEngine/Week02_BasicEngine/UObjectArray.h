#pragma once
#include "Core/Core.h"

class UObject;

class UObjectArray
{
public:
/*	//GUObjectArray에 NewObj를 추가합니다.
	void RegisterObj(UObject* NewObj)*/;
	UObjectArray() = default;
	UObjectArray(const UObjectArray&) = delete;
	UObjectArray(UObjectArray&&) = delete;
	UObjectArray& operator=(UObjectArray&&) = delete;

	~UObjectArray();

	//GUObjectArray에 NewObj를 추가합니다.
	//void RegisterObj(UObject* NewObj);

	//Obj를 delete 합니다.
	void RemoveObj(UObject* Obj);
/*	//GUObjectArray에서 Obj를 제거합니다. delete 시키진 않습니다.
	void UnregisterObject(UObject* Obj);*/

	//모든 Object 배열을 반환합니다.(읽기 전용)
	const TArray<UObject*>& GetAllObjects() const { return Objects; }

	//모든 Object 배열을 반환합니다.
	TArray<UObject*>& GetAllObjects() { return Objects; }

	//UObject의 개수를 반환합니다.
	int32 GetNum()const;

	//소멸
	void Release();

private:
	//모든 UObject를 저장하는 배열
	TArray<UObject*> Objects;
	//인덱싱 가능한 번호를 저장합니다.
	TArray<int32> AvailableIndices;
};

//전역 인스턴스
extern UObjectArray GUObjectArray;