#include "UObject.h"
#include "UEngineStatics.h"
#include "UObjectArray.h"
#include <limits>
#include <new>


uint32 TotalAllocationBytes = 0;
uint32 TotalAllocationCount = 0;

namespace
{

	struct alignas(std::max_align_t) FAllocationHeader
	{
		uint32 Size; // 객체의 실제 크기
	};
}


UObject::UObject()
	: UUID(UEngineStatics::GenUUID()), InternalIndex(0)
{
	InternalIndex = 0;
	//GUOjbectArray에 추가되면서 InternalIndex도 재할당됨.
	GUObjectArray.RegisterObj(this);
}

UObject::~UObject()
{
	//GUObjectArray에 자기자신 제거
	GUObjectArray.UnregisterObject(this);
}


void* UObject::operator new(std::size_t Size)
{
	const std::size_t TotalSize = Size + sizeof(FAllocationHeader);

	if (TotalSize > std::numeric_limits<uint32>::max())
	{
		throw std::bad_alloc();
	}

	void* Mem = ::operator new(TotalSize);

	//할당받은 메모리의 맨 앞부분에 Placement new를 이용해 헤더 구조체를 생성하고 객체의 크기를 Header에 기록
	FAllocationHeader* Header = static_cast<FAllocationHeader*>(Mem);
	Header->Size = static_cast<uint32>(Size);

	TotalAllocationBytes += static_cast<uint32>(TotalSize);
	TotalAllocationCount++;

	//사용자가 실제 쓸 메모리 주소를 반환 (헤더 바로 뒤 주소)
	return static_cast<void*>(Header + 1);
}

void UObject::operator delete(void* Ptr) noexcept
{
	if (!Ptr)
	{
		return;
	}

	FAllocationHeader* Header = static_cast<FAllocationHeader*>(Ptr) - 1;
	TotalAllocationBytes -= Header->Size + static_cast<uint32>(sizeof(FAllocationHeader));;
	TotalAllocationCount--;
	Header->~FAllocationHeader();
	::operator delete(Header);	
}
