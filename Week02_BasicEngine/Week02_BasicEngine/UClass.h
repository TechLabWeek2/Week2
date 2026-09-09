#pragma once

class UObject;

class UClass
{
public:
    using CreateFunc = UObject * (*)();

    UClass(const char* InName, UClass* InSuperClass, CreateFunc InCreator = nullptr)
        : Name(InName), SuperClass(InSuperClass), Creator(InCreator)
    {
    }

    const char* GetName() const { return Name; }
    UClass* GetSuperClass() const { return SuperClass; }

    UObject* CreateInstance() const
    {
        return Creator ? Creator() : nullptr;
    }

private:
    const char* Name;
    UClass* SuperClass;
	CreateFunc Creator;
};