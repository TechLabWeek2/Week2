#pragma once
#include <Core/Core.h>

class UObject
{
public:
    uint32 UUID;
	//전역 배열 인덱스
    uint32 InternalIndex;

    UObject();
    virtual ~UObject();

    UObject(const UObject&) = delete;
    UObject& operator=(const UObject&) = delete;

    static void* operator new(std::size_t Size);
    static void operator delete(void* Ptr) noexcept;

};

extern uint32 TotalAllocationBytes;
extern uint32 TotalAllocationCount;