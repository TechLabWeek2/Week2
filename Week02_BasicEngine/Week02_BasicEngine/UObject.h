#pragma once
#include <Core/Core.h>
#include "UClass.h"
class UObject
{
public:
    uint32 UUID;
	//전역 배열 인덱스
    uint32 InternalIndex;

    bool bIsActive = true;

    UObject();
    virtual ~UObject();

    UObject(const UObject&) = delete;
    UObject& operator=(const UObject&) = delete;

    static void* operator new(std::size_t Size);
    static void operator delete(void* Ptr) noexcept;

    //virtual void Update(float deltaTime);

    UClass* GetClass() const { return Class; }

    bool IsA(UClass* InClass) const {
        UClass* Current = Class;

        while (Current) {
            if (Current == InClass)  return true;

            Current = Current->GetSuperClass();
        }
        return false;
    }

    static UClass* StaticClass() {
        static UClass Class("UObject", nullptr);
        return &Class;
    }

protected:
    UClass* Class = StaticClass();
};

extern uint32 TotalAllocationBytes;
extern uint32 TotalAllocationCount;