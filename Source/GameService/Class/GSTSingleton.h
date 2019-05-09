#pragma once

#include "CoreMinimal.h"
#include <iostream>

// �������� �̱����� ���� �ʴ´�.
template<typename T>
class GSTSingleton
{
public:
	static T* Instance;
	static void InitInstance(T* inThis)
	{
		Instance = inThis;
	}
	static void RemoveInstance()
	{
		Instance = NULL;
	}
};

