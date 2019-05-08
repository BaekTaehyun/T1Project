#pragma once
#include <string>


class UTextUtil
{
	// Example usage
	//GetEnumValueAsString<EVictoryEnum>("EVictoryEnum", VictoryEnum)));
public:
	template<typename TEnum>
	static FORCEINLINE FString GetEnumValueAsString(const FString& Name, TEnum Value)
	{
		const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, *Name, true);
		if (!enumPtr)
		{
			return FString("Invalid");
		}
		return enumPtr->GetNameByValue((int64)Value).ToString();
	}

	static FORCEINLINE std::string FString_TO_string(FString& fstring)
	{
		return std::string(TCHAR_TO_UTF8(*fstring));
	}
	
	static FORCEINLINE FString string_TO_FString(std::string& stdstring)
	{
		return FString(stdstring.c_str());
	}
};

#define EnumToString(EnumClassName, ValueOfEnum) UTextUtil::GetEnumValueAsString<EnumClassName>(FString(TEXT(#EnumClassName)), (ValueOfEnum))
#define TOstring(Fstring) UTextUtil::FString_TO_string(Fstring);
#define TOFString(STDstring) UTextUtil::string_TO_FString(STDstring);