#pragma once

#include <Core/Core.h>

class UObject
{
public:
    uint32 UUID;
    uint32 InternalIndex;

    UObject();
    virtual ~UObject();

    UObject(const UObject&) = delete;
    UObject& operator=(const UObject&) = delete;

    static void* operator new(std::size_t Size);
    static void operator delete(void* Ptr) noexcept;
};

extern TArray<UObject*> GUObjectArray;

extern uint32 TotalAllocationBytes;
extern uint32 TotalAllocationCount;