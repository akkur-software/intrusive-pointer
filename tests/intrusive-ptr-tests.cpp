#include <algorithm>
#include "CppUnitTest.h"
#include "include/intrusive_ptr.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

struct Object : public RefCountObject<Object>
{
	Object() : Value(0) { }
	Object(int value) : Value(value) { }
	virtual ~Object() = default;
	
	int Value;
};


TEST_CLASS(IntrusivePtrTests)
{
public:
		
	TEST_METHOD(CreateEmptyPtr_Success)
	{
		// Arrange && Act
		auto ptr = intrusive_ptr<Object>();
		auto data = ptr.get();
		auto use_count = ptr.use_count();

		// Assert
		Assert::IsNull(data);
		Assert::AreEqual(0u, use_count);
	}

	TEST_METHOD(CreatePtr_Success)
	{
		// Arrange
		auto raw_value = 10;
		auto raw_ptr = new Object(raw_value);
		
		// Act		
		auto ptr = intrusive_ptr<Object>(raw_ptr);
		auto data = ptr.get();
		auto value = data ? data->Value : 0;
		auto use_count = ptr.use_count();

		// Assert
		Assert::IsNotNull(data);
		Assert::AreEqual(1u, use_count);
		Assert::AreEqual(raw_value, value);
	}

	TEST_METHOD(MakePtr_Success)
	{
		// Arrange
		auto raw_value = 2;

		// Act
		auto ptr = make_intrusive<Object>(raw_value);
		auto data = ptr.get();
		auto value = data ? data->Value : 0;
		auto use_count = ptr.use_count();

		// Assert
		Assert::IsNotNull(data);
		Assert::AreEqual(1u, use_count);
		Assert::AreEqual(raw_value, value);
	}

	TEST_METHOD(MakeEmptyPtr_Fail)
	{
		// Arrange && Act
		auto ptr = make_intrusive<Object>();
		auto data = ptr.get();
		auto use_count = ptr.use_count();

		// Assert
		Assert::IsNotNull(data);
		Assert::AreNotEqual(0u, use_count);
	}

	TEST_METHOD(CopyPtrFromSource_Success)
	{
		// Arrange
		auto raw_value = 20;
		auto src_ptr = make_intrusive<Object>(raw_value);

		// Act
		auto copy_ptr = src_ptr;
		auto data = copy_ptr.get();
		auto value = data ? data->Value : 0;
		auto use_count = copy_ptr.use_count();

		// Assert
		Assert::IsNotNull(data);
		Assert::AreEqual(2u, use_count);
		Assert::AreEqual(raw_value, value);
	}

	TEST_METHOD(MovePtrFromSource_Success)
	{
		// Arrange
		auto raw_value = 99;
		auto src_ptr = make_intrusive<Object>(raw_value);

		// Act
		auto mv_ptr = std::move(src_ptr);
		auto mv_data = mv_ptr.get();
		auto mv_value = mv_data ? mv_data->Value : 0;
		auto mv_use_count = mv_ptr.use_count();

		// Assert
		Assert::IsNull(src_ptr.get());
		Assert::IsNotNull(mv_data);
		Assert::AreEqual(1u, mv_use_count);
		Assert::AreEqual(raw_value, mv_value);
	}

	TEST_METHOD(ComparePtr_Success)
	{
		// Arrange
		auto raw_value = 40;
		auto ptr1 = make_intrusive<Object>(raw_value);
		auto ptr2 = ptr1;
		auto ptr3 = make_intrusive<Object>(raw_value);

		// Act
		auto areEquals = ptr1 == ptr2;
		auto areNotEquals = ptr1 != ptr3;

		// Assert
		Assert::IsTrue(areEquals);
		Assert::IsTrue(areNotEquals);
	}

	TEST_METHOD(DetachPtr_Success)
	{
		// Arrange
		auto raw_value = 60;
		auto ptr = make_intrusive<Object>(raw_value);		

		// Act
		auto raw_ptr = ptr.detach();

		// Assert
		Assert::IsNotNull(raw_ptr);
		Assert::IsNull(ptr.get());
		Assert::AreEqual(1u, raw_ptr->ReferenceCount());
	}

	TEST_METHOD(ResetPtr_Success)
	{
		// Arrange
		auto raw_value = 60;
		auto raw_ptr = new Object();
		auto ptr = make_intrusive<Object>(raw_value);

		// Act
		ptr.reset(raw_ptr);
		auto data = ptr.get();
		auto use_count = ptr.use_count();

		// Assert
		Assert::IsNotNull(ptr.get());
		Assert::AreEqual(0, ptr.get()->Value);
		Assert::AreEqual(1u, use_count);
	}

	TEST_METHOD(SwapPtr_Success)
	{
		// Arrange
		auto raw_value1 = 18;
		auto raw_value2 = 81;

		auto ptr1 = make_intrusive<Object>(raw_value1);
		auto ptr2 = make_intrusive<Object>(raw_value2);

		// Act
		ptr1.swap(ptr2);

		// Assert
		Assert::IsNotNull(ptr1.get());
		Assert::IsNotNull(ptr2.get());
		Assert::AreEqual(raw_value1, ptr2.get()->Value);
		Assert::AreEqual(raw_value2, ptr1.get()->Value);
		Assert::AreEqual(1u, ptr1.use_count());
		Assert::AreEqual(1u, ptr2.use_count());
	}
};
