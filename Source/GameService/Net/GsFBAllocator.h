#pragma once
#include  "./Network/AllocateHelper.h"
#include "LeanPacket_generated.h"
#include "GameService.h"

class FBAllocator : public flatbuffers::Allocator
{
public:

	uint8_t* allocate(size_t size) override
	{
		if (size == 0)
			size = 1;
		return SimpleAllocator::Alloc<uint8_t>(size);
	}
	void deallocate(uint8_t* p, size_t size) override
	{
		//(size);
		SimpleAllocator::Free(p);
	}

};

class FBBuilder	
{
	enum
	{
		INIT_FB_BUFFER_SIZE = 2 * 1024,
	};

	static FBAllocator*	GetAllocator()
	{
		static FBAllocator allocator;
		return &allocator;
	}

	flatbuffers::FlatBufferBuilder	fbBuilder_;

public:

	FBBuilder()
		: fbBuilder_(INIT_FB_BUFFER_SIZE, GetAllocator())
	{
	}

	flatbuffers::FlatBufferBuilder&	get() { return fbBuilder_; }

	const void* getBuffer() const { return fbBuilder_.GetCurrentBufferPointer(); }
	uint32_t size() const { return fbBuilder_.GetSize(); }

	using StringOffset = flatbuffers::Offset<flatbuffers::String>;
	
	//http://www.open-std.org/jtc1/sc22/wg14/www/docs/n2231.htm
	/*The types declared are mbstate_t(described in 7.29.1) and size_t(described in 7.19);
	char8_t
	which is an unsigned integer type used for 8 - bit characters and is the same type as unsigned char; and*/
	//StringOffset createString(const char8_t* str)
	/*StringOffset createString(const unsigned char* str)
	{
		return fbBuilder_.CreateString(str);
	}*/
	// https://en.cppreference.com/w/cpp/string/basic_string
	// c++ 20에 기본으로 추가됨.. 현재는 지원하지 않음
	// std::u8string (C++20)	std::basic_string<char8_t>
	/*StringOffset createString(const std::basic_string<unsigned char>& str)
	{
		return fbBuilder_.CreateString(str);
	}*/
#pragma	todo("bak1210 : serverside string check!!")
	StringOffset createString(const FString& str)
	{
		return fbBuilder_.CreateString(TCHAR_TO_UTF8(*str));
	}

	template<typename T>
	flatbuffers::Offset<flatbuffers::Vector<T>> createVector(const std::vector<T>& v)
	{
		return fbBuilder_.CreateVector(v);
	}
	template<typename T, size_t SIZE>
	flatbuffers::Offset<flatbuffers::Vector<T>> createVector(const std::array<T, SIZE>& v)
	{
		return fbBuilder_.CreateVector(v.data(), v.size());
	}
	template<typename T, size_t SIZE>
	flatbuffers::Offset<flatbuffers::Vector<T>> createVector(const std::array<T, SIZE>& v, size_t size)
	{
		//ASSERT_CRASH(size <= SIZE);
		return fbBuilder_.CreateVector(v.data(), size);
	}

	template<typename T>
	void finish(flatbuffers::Offset<T> root)
	{
		fbBuilder_.Finish(root);
	}

	void clear()
	{
		fbBuilder_.Clear();
	}
};
