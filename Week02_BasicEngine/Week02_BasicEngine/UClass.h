#pragma once

class UClass
{
public:
    UClass(const char* InName, UClass* InSuperClass)
        : Name(InName), SuperClass(InSuperClass)
    {
    }

    const char* GetName() const { return Name; }
    UClass* GetSuperClass() const { return SuperClass; }

private:
    const char* Name;
    UClass* SuperClass;
};