#pragma once
#include "PMP.hpp"
#include <functional>

#define $lf(item) Types::FunctorType<decltype(item)>::Result

namespace Types{
	template<typename...>struct TypeArray;
	template<size_t...>struct NumArray;
	template<char...>struct TMPString;
	template<typename Type, Type...>struct ScalarArray;
		//result: <bool> 类型表层上是否有const标记
		//Result: <type> 表层上去除const标记的类型
		template<typename _1>struct ConstType{
			static const bool result = false;
			typedef _1 Result;
		};
		template<typename _1>struct ConstType<_1 const>{
			static const bool result = true;
			typedef _1 Result;
		};
		//result: <bool> 类型表层上是否有volatile标记
		//Result: <type> 表层上去除volatile标记的类型
		template<typename _1>struct VolatileType{
			static const bool result = false;
			typedef _1 Result;
		};
		template<typename _1>struct VolatileType<_1 volatile>{
			static const bool result = true;
			typedef _1 Result;
		};
		//Result: <type> 去除类型表层上的const和volatile
		template<typename _1>struct RemoveCV{
			typedef typename VolatileType
				<typename ConstType<_1>::Result>::Result Result;
		};
		//result: <bool> 类型是否相等
		//Select: <template<_,_>> 相等则选前者，否则选后者
		template<typename _1, typename _2>struct SameType{
			static const bool result = false;
			template<typename _3, typename _4>struct Select{
				typedef _4 Result;
			};
		};
		template<typename _1>struct SameType<_1,_1>{
			static const bool result = true;
			template<typename _3, typename _4>struct Select{
				typedef _3 Result;
			};
		};
		//result: <bool> 其余类型中是否有一个与_1相等
		template<typename _1, typename _2, typename... _n>struct AnySameType{
			static const bool result = SameType<_1, _2>::result ||
				AnySameType<_1, _n...>::result;
		};
		template<typename _1, typename _2>struct AnySameType<_1,_2>{
			static const bool result = SameType<_1, _2>::result;
		};
		//result: <bool> 其余类型中是否都与_1相等
		template<typename _1, typename _2, typename... _n>struct AllSameType{
			static const bool result = SameType<_1, _2>::result &&
				AllSameType<_1, _n...>::result;
		};
		template<typename _1, typename _2>struct AllSameType<_1, _2>{
			static const bool result = SameType<_1, _2>::result;
		};
		//result: <bool> 是否为有符号整数
		template<typename _1>struct SignedIntegralType{
			typedef typename RemoveCV<_1>::Result once;
			static const bool result = AnySameType
				<once, signed char, short, int, long, long long>::result;
		};
		//result: <bool> 是否为无符号整数
		template<typename _1>struct UnsignedIntegralType{
			typedef typename RemoveCV<_1>::Result once;
			static const bool result = AnySameType
				<once, unsigned char, unsigned short, unsigned,
				unsigned long, unsigned long long>::result;
		};
		//result: <bool> 是否为bool，char，wchar_t
		template<typename _1>struct OtherIntegralType{
			typedef typename RemoveCV<_1>::Result once;
			static const bool result = AnySameType<once, bool, char, wchar_t>::result;
		};
		//result: <bool> 是否为整数
		template<typename _1>struct IntegralType{
			static const bool result = SignedIntegralType<_1>::result ||
				UnsignedIntegralType<_1>::result || OtherIntegralType<_1>::result;
		};
		//result: <bool> 是否为浮点数
		template<typename _1>struct FloatType{
			typedef typename RemoveCV<_1>::Result once;
			static const bool result = AnySameType
				<once, float, double, long double>::result;
		};
		//result: <bool> 是否为void
		template<typename _1>struct VoidType{
			typedef typename RemoveCV<_1>::Result once;
			static const bool result = SameType<once, void>::result;
		};
		//result: <size_t> 数组大小，0表示非数组
		//Result: <type> 数组元素类型
		template<typename _1>struct ArrayType{
			static const size_t result = 0;
			typedef _1 Result;
		};
		template<typename _1, size_t size>struct ArrayType<_1[size]>{
			static const size_t result = size;
			typedef _1 Result;
		};
		//result: <bool> 是否为指针
		//Result: <type> 被指向类型
		template<typename _1>struct PointerType{
			template<typename _2>struct Helper{
				static const bool result = false;
				typedef _2 Result;
			};
			template<typename _2>struct Helper<_2*>{
				static const bool result = true;
				typedef _2 Result;
			};
			typedef typename RemoveCV<_1>::Result once;
			static const bool result = Helper<once>::result;
			typedef typename Helper<once>::Result Result;
		};
		//result: <size_t> 主线指针层数
		template<typename _1>struct PointerDepth{
			template<size_t depth, typename _2>struct Helper{
				static const size_t result = depth;
				typedef _2 Result;
			};
			template<size_t depth, typename _2>struct Helper<depth, _2*>{
				typedef typename RemoveCV<_2>::Result once;
				static const size_t result = Helper<depth + 1, once>::result;
				typedef typename Helper<depth + 1, once>::Result Result;
			};
			typedef typename RemoveCV<_1>::Result once;
			static const size_t result = Helper<0, once>::result;
			typedef typename Helper<0, once>::Result Result;
		};
		//result: <size_t> 1为左值引用，2为右值引用，0为非引用类型
		//Result: <type> 被引用类型
		template<typename _1>struct ReferenceType{
			static const size_t result = 0;
			typedef _1 Result;
		};
		template<typename _1>struct ReferenceType<_1&>{
			static const size_t result = 1;
			typedef _1 Result;
		};
		template<typename _1>struct ReferenceType<_1&&>{
			static const size_t result = 2;
			typedef _1 Result;
		};
		//result: <bool> 是否为成员类型
		//Result: <type> 成员形态
		template<typename _1>struct MemberType{
			static const bool result = false;
			typedef void Result;
			typedef void Class;
		};
		template<typename _1, typename C>struct MemberType<_1 C::*>{
			static const bool result = true;
			typedef _1 Result;
			typedef C Class;
		};
		template<typename Ret, typename C, typename... Args>
		struct MemberType<Ret(C::*)(Args...)>{
			static const bool result = true;
			typedef Ret Result(Args...);
			typedef C Class;
		};
		template<typename Ret, typename C, typename... Args>
		struct MemberType<Ret(C::*)(Args...)const>{
			static const bool result = true;
			typedef Ret Result(Args...);
			typedef C Class;
		};
		template<typename Ret, typename C, typename... Args>
		struct MemberType<Ret(C::*)(Args...,...)>{
			static const bool result = true;
			typedef Ret Result(Args...,...);
			typedef C Class;
		};
		template<typename Ret, typename C, typename... Args>
		struct MemberType<Ret(C::*)(Args...,...)const>{
			static const bool result = true;
			typedef Ret Result(Args...,...);
			typedef C Class;
		};
		//result: <bool> 是否为数值
		template<typename _1>struct NumberType{
			static const bool result = IntegralType<_1>::result ||
				FloatType<_1>::result;
		};
		//result: <bool> 是否为内建类型
		template<typename _1>struct IntrinsicType{
			static const bool result = NumberType<_1>::result || VoidType<_1>::result;
		};
		//result: <bool> 非内建类型
		template<typename _1>struct NonIntrinsicType{
			static const bool result = !IntrinsicType<_1>::result;
		};
		//result: <size_t> 主线数组深度
		//Result: <type> 数组根元素类型
		//Select: <template<N>> 第N层数组大小【由里到外】
		//SizeArray: <NumArray> 数组大小的整数序列
		template<typename _1>struct ArrayDepth{
			template<typename _A, size_t w>struct Array{
				static const size_t result = w;
				typedef _A Result;
			};

			template<size_t depth, typename _arr, typename _A>struct Helper{
				typedef _arr RawType;
				typedef _A SizeArray;
				static const size_t result = depth;
			};
			template<size_t depth, typename _arr, size_t _size, typename _A>
			struct Helper<depth, _arr[_size], _A>
				:Helper<depth + 1, _arr, Array<_A, _size> >{};

			typedef Helper<0, _1, int > Next;
			typedef typename Next::RawType Result;
			typedef typename Next::SizeArray _SizeArray;
			static const size_t result = Next::result;

			template<size_t index>struct Select{
				template<size_t index, typename _A>
				struct Helper:Helper<index - 1, typename _A::Result >{};
				template<typename _A>struct Helper<0, _A>{
					static const size_t result = _A::result;
				};
				
				static const size_t result = Helper<index, SizeArray>::result;
			};

			template<typename NA1, typename NA2>struct MakeArray;
			template<size_t... nums, typename NA2>
			struct MakeArray<NumArray<nums...>, NA2>
				:MakeArray<NumArray<nums..., NA2::result>, typename NA2::Result>{};
			template<size_t... nums>struct MakeArray<NumArray<nums...>, int>{
				typedef NumArray<nums...> Result;
			};

			typedef typename MakeArray<NumArray<>, _SizeArray>::Result SizeArray;

		};
		//result: <size_t> 字节对齐所需要的个数
		template<typename _1>struct AlignmentType{
			static const size_t result = std::alignment_of<_1>::value;
		};
		//result: <bool> 是否为Enum类型
		template<typename _1>struct EnumType{
			static const bool result = std::is_enum<_1>::value;
		};
		//result: <bool> 是否为Union类型
		template<typename _1>struct UnionType{
			static const bool result = std::is_union<_1>::value;
		};
		//result: <bool> 是否为class类型，而不是union类型
		template<typename _1>struct ClassType{
			template<typename _2>static char TypeTester(void (_2::*)()){};
			template<typename _2>static long TypeTester(...){};
			static const bool result = sizeof(TypeTester<_1>(nullptr)) == sizeof(char)
				&& !UnionType<_1>::result;
		};
		//result: <bool> 是否是对象类型（数值或类）
		template<typename _1>struct ObjectType{
			static const bool result = !PointerType<_1>::result &&
				!ReferenceType<_1>::result && !VoidType<_1>::result;
		};
		//result: <bool> 是否为标量类型
		template<typename _1>struct ScalarType{
			static const bool result = NumberType<_1>::result || VoidType<_1>::result
				|| EnumType<_1>::result || MemberType<_1>::result;
		};
		//Result: <type> 是则为前者，否则为后者
		template<bool check, typename _1, typename _2>struct SelectType{
			typedef _2 Result;
		};
		template<typename _1, typename _2>struct SelectType<true, _1, _2>{
			typedef _1 Result;
		};
		//result: <bool> 是否为函数或仿函数类型
		//Result: <type> 函数形态
		template<typename _1>struct FunctorType{
			template<typename _2>struct Helper{
				static const bool result = false;
				typedef void Result;
			};
			template<typename Ret, typename... Args>struct Helper<Ret(Args...)>{
				static const bool result = true;
				typedef Ret Result(Args...);
			};
			template<typename Ret, typename... Args>struct Helper<Ret(Args...,...)>{
				static const bool result = true;
				typedef Ret Result(Args...,...);
			};
			template<typename Ret, typename... Args>struct Helper<Ret(*)(Args...)>{
				static const bool result = true;
				typedef Ret Result(Args...);
			};
			template<typename Ret, typename... Args>struct Helper<Ret(&)(Args...)>{
				static const bool result = true;
				typedef Ret Result(Args...);
			};
			template<typename Ret, typename... Args>struct Helper<Ret(*)(Args...,...)>{
				static const bool result = true;
				typedef Ret Result(Args...,...);
			};
			template<typename Ret, typename... Args>struct Helper<Ret(&)(Args...,...)>{
				static const bool result = true;
				typedef Ret Result(Args...,...);
			};
			template<typename Class, typename Ret, typename... Args>
			struct Helper<Ret(Class::*)(Args...)>{
				static const bool result = true;
				typedef Ret Result(Args...);
			};
			template<typename Class, typename Ret, typename... Args>
			struct Helper<Ret(Class::*)(Args...)const>{
				static const bool result = true;
				typedef Ret Result(Args...);
			};
			template<typename Class, typename Ret, typename... Args>
			struct Helper<Ret(Class::*)(Args...,...)>{
				static const bool result = true;
				typedef Ret Result(Args...,...);
			};
			template<typename Class, typename Ret, typename... Args>
			struct Helper<Ret(Class::*)(Args...,...)const>{
				static const bool result = true;
				typedef Ret Result(Args...,...);
			};
			template<typename Class, typename Ret, typename... Args>
			struct Helper<Ret(Class::*)(Args...)volatile>{
				static const bool result = true;
				typedef Ret Result(Args...);
			};
			template<typename Class, typename Ret, typename... Args>
			struct Helper<Ret(Class::*)(Args...)const volatile>{
				static const bool result = true;
				typedef Ret Result(Args...);
			};
			template<typename Class, typename Ret, typename... Args>
			struct Helper<Ret(Class::*)(Args..., ...)volatile>{
				static const bool result = true;
				typedef Ret Result(Args..., ...);
			};
			template<typename Class, typename Ret, typename... Args>
			struct Helper<Ret(Class::*)(Args..., ...)const volatile>{
				static const bool result = true;
				typedef Ret Result(Args..., ...);
			};
			typedef typename RemoveCV<_1>::Result once;
			template<bool check, typename _2>struct FunctorHelper{
				static const bool result = false;
				typedef void Result;
			};
			template<typename _2>struct FunctorHelper<true, _2>{
				typedef Helper<decltype(&_2::operator())> Next;
				static const bool result = Next::result;
				typedef typename Next::Result Result;
			};
			template<typename _2>static 
				char operatorhelper(decltype(&_2::operator()) w){};
			template<typename _2>static long operatorhelper(...){};
			typedef Helper<once> Xonce;
			typedef FunctorHelper
				<sizeof(operatorhelper<once>(nullptr)) == sizeof(char), _1> Xtwice;
			static const bool result = Xonce::result || Xtwice::result;
			typedef typename SelectType<Xonce::result, typename Xonce::Result,
				typename Xtwice::Result>::Result Result;
		};
		//获取函数的信息
		template<typename _1>struct GetFuncInfo{
			typedef typename FunctorType<_1>::Result Method;
			template<typename>struct Helper;
			template<typename Ret, typename... Args>struct Helper<Ret(Args...)>{
				typedef Ret Return;
				typedef TypeArray<Args...> Arg;
			};
			template<typename Ret, typename... Args>struct Helper<Ret(Args..., ...)>{
				typedef Ret Return;
				typedef TypeArray<Args...> Arg;
			};
			typedef typename Helper<Method>::Return Ret;
			typedef typename Helper<Method>::Arg Args;
		};
		//result: <bool> 是否为空类
		template<typename _1>struct EmptyClassType{
			template<typename _2, bool check>struct Helper{
				static const bool result = false;
			};
			template<typename _2>struct Helper<_2, true>{
				class Derive : _2{
					int a;
				};
				static const bool result = sizeof(Derive) == sizeof(int);
			};
			static const bool result = Helper<_1, ClassType<_1>::result>::result;
		};
		//result: <bool> 是否含虚函数
		template<typename _1>struct VirtualClassType{
			template<typename _2, bool check>struct Helper{
				static const bool result = false;
			};
			template<typename _2>struct Helper<_2, true>{
				class Base : _2{
					int a;
				};
				class Derive : Base{
					virtual ~Derive(){};
				};
				static const bool result = sizeof(Derive) == sizeof(Base);
			};
			static const bool result = Helper<_1, ClassType<_1>::result>::result;
		};
		//result: <bool> 是否是抽象类
		template<typename _1>struct AbstractType{
			template<typename _2>static char helper(_2(*)[1]){};
			template<typename _2>static long helper(...){};
			static const bool result = VirtualClassType<_1>::result &&
				sizeof(helper<_1>(nullptr)) == sizeof(long);
		};
		//result: <bool> 是否是POD类型
		template<typename _1>struct PODType{
			static const bool result = std::is_pod<_1>::value;
		};
		//result: <bool> 两个类型能否进行隐式转换
		template<typename _1, typename _2>struct ImplicitConvertible{
			static char helper(_1){};
			static long helper(...){};
			static const bool result = sizeof(helper(_2())) == sizeof(char);
		};
		//result: <bool> 两个类型能否进行显式转换
		template<typename _1, typename _2>struct ExplicitConvertible{
			template<typename _3>
			static auto helper(_3* w)->decltype(_2(*w), true){};
			template<typename _3>static long helper(...){};
			static const bool result = sizeof(helper<_1>(nullptr)) == sizeof(bool);
		};
		//result: <bool> 是否是继承关系
		template<typename _1, typename _2>struct InheritType{
			template<bool X, typename _3>struct Helper{
				static const bool result = false;
			};
			template<typename _3>struct Helper<true, _3>{
				static char helper(_3*){};
				static long helper(...){};
				static const bool result = sizeof(helper((_2*)0)) == sizeof(char);
			};
			
			static const bool result = 
				Helper<ClassType<_1>::result && ClassType<_2>::result, _1>::result;
		};
		//call  : <template<_,_>> 反向调用元函数
		template<template<typename, typename>class Func>struct ReverseFunc{
			template<typename _1, typename _2>struct call:Func<_2, _1>{};
		};
		//call  : <template<_>> 将类型在二元元函数上调用
		template<template<typename, typename>class Func>struct SelfTwiceFunc{
			template<typename _1>struct call :Func<_1, _1>{};
		};
		//call  : <template<_>> 将函数递归的调用两次
		template<template<typename>class Func>struct RecursionFunc{
			template<typename _1>struct call :Func<typename Func<_1>::Result>{};
		};
		//Result: <type> 去除主线上的引用，指针，数组，CV标记后的类型
		template<typename _1>struct OriginalType{
			typedef typename ReferenceType<_1>::Result _2;
			//typedef typename RemoveCV<_2>::Result _3;
			template<bool a, bool b, typename _3, typename _4>struct Helper;
			template<bool b, typename _3, typename _4>struct Helper<false, b, _3, _4>{
				typedef typename Helper<
					(bool)PointerDepth<_4>::result, (bool)ArrayDepth<_4>::result,
					typename PointerDepth<_4>::Result, 
					typename ArrayDepth<_4>::Result>::Result Result;
			};
			template<bool a, typename _3, typename _4>struct Helper<a, false, _3, _4>{
				typedef typename Helper<
					(bool)PointerDepth<_3>::result, (bool)ArrayDepth<_3>::result,
					typename PointerDepth<_3>::Result,
					typename ArrayDepth<_3>::Result>::Result Result;
			};
			template<typename _3, typename _4>struct Helper<false, false, _3, _4>{
				typedef _3 Result;
			};
			typedef typename Helper<(bool)PointerDepth<_2>::result, 
				(bool)ArrayDepth<_2>::result,
				typename PointerDepth<_2>::Result, typename ArrayDepth<_2>::Result>
				::Result _5;
			typedef typename RemoveCV<_5>::Result Result;
		};
		//Result: <type> 添加const标记
		template<typename _1>struct AddConst{
			typedef _1 const Result;
		};
		//Result: <type> 添加volatile标记
		template<typename _1>struct AddVolatile{
			typedef _1 volatile Result;
		};
		//Result: <type> 添加CV标记
		template<typename _1>struct AddCV{
			typedef _1 const volatile Result;
		};
		//Result: <type> 添加左值引用标记
		template<typename _1>struct AddReference{
			typedef typename ReferenceType<_1>::Result _2;
			typedef _2& Result;
		};
		//Result: <type> 添加常量左值引用标记
		template<typename _1>struct AddConstReference{
			typedef typename ReferenceType<_1>::Result _2;
			typedef _2 const& Result;
		};
		//Result: <type> 添加右值引用标记
		template<typename _1>struct AddRValueReference{
			typedef typename ReferenceType<_1>::Result _2;
			typedef _2&& Result;
		};
		//Result: <type> 添加指针标记
		template<typename _1>struct AddPointer{
			typedef typename ReferenceType<_1>::Result _2;
			typedef _2* Result;
		};
		//Result: <type> 添加size重指针标记
		template<typename _1, size_t size>struct AddMultiPointer{
			typedef typename ReferenceType<_1>::Result _2;
			typedef typename AddMultiPointer<_2*, size - 1>::Result Result;
		};
		template<typename _1>struct AddMultiPointer<_1, 0>{
			typedef _1 Result;
		};
		//Result: <type> 添加常量指针标记
		template<typename _1>struct AddConstPointer{
			typedef typename ReferenceType<_1>::Result _2;
			typedef _2* const Result;
		};
		//Result: <type> 添加数组标记
		template<typename _1, size_t size>struct AddArray{
			typedef _1 Result[size];
		};
		//result: <size_t> 对应数字
		//Next  : <type> 下一个数字类型
		//Prev  : <type> 上一个数字类型
		template<size_t index>struct TypeForNumber{
			static const size_t result = index;
			typedef TypeForNumber<index + 1> Next;
			typedef TypeForNumber<index - 1> Prev;
		};
		//Result: <type> 对应的类型
		template<size_t index, typename Head, typename... Types>
		struct SelectTypeInGroup : SelectTypeInGroup<index - 1, Types...>{};
		template<typename Head, typename... Types>
		struct SelectTypeInGroup<0, Head, Types...>{
			typedef Head Result;
		};
		//Result: <TypeArray || void> 类型模板参数
		template<typename _1>struct TemplateClassType{
			template<template <typename...> class A, typename... B>
				static TypeArray<B...>* helper(A<B...>*){};
			template<template<template<typename>class, typename...> class A,
				template<typename>class C, typename... B>
				static TypeArray<B...>* helper(A<C, B...>*){};
			template<template<template<typename, typename>class, typename...> class A,
				template<typename, typename>class C, typename... B>
				static TypeArray<B...>* helper(A<C, B...>*){};
			template<template<template<typename...>class, typename...> class A,
				template<typename...>class C, typename... B>
				static TypeArray<B...>* helper(A<C, B...>*){};
			//static void* helper(...){};
			typedef decltype(helper((_1*)nullptr)) once;
			typedef typename PointerType<once>::Result Result;
			static const bool result = VoidType<once>::result;
		};
		//result: <bool> 可复制的类型
		template<typename _1>struct Replicable{
			static const bool result = ExplicitConvertible<_1, _1>::result;
		};
		//result: <bool> 有默认构造函数的类型
		template<typename _1>struct DefaultInitializable{
			template<typename _2>static auto helper(void*)->decltype(_2(), true){};
			template<typename _2>static long helper(...){};
			static const bool result = sizeof(helper<_1>(nullptr)) == sizeof(bool);
		};
		//result: <bool> 有移动构造函数的类型
		template<typename _1>struct MoveReplicable{
			static const bool result = 
				ExplicitConvertible<AddRValueReference<_1>::Result, _1>::result;
		};
		//result: <bool> 可赋值的类型
		template<typename _1>struct CopyAssignable{
			template<typename _2>
			static auto helper(void*)->decltype((*(_2*)0)=(*(_2*)0), true){};
			template<typename _2>static long helper(...){};
			static const bool result = sizeof(helper<_1>(nullptr)) == sizeof(bool);
		};
		//result: <bool> 可赋值的两个类型
		template<typename _1, typename _2>struct Assignable{
			template<typename _3>
			static auto helper(void*)->decltype((*(_3*)0)=(*(_2*)0), true){};
			template<typename _3>static long helper(...){};
			static const bool result = sizeof(helper<_1>(nullptr)) == sizeof(bool);
		};
		
		template<typename TS>struct TMPStringToCString;
		template<char... chs>struct TMPStringToCString<TMPString<chs...>>{
			template<bool check, size_t num>struct Helper{
				static const char result = '\0';
			};
			template<size_t num>struct Helper<true, num>{
				template<size_t x, char w, char... chs>struct SubHelper
				:SubHelper<x - 1, chs...>{};
				template<char w, char... chs>struct SubHelper<0, w, chs...>{
					static const char result = w;
				};
				static const char result = SubHelper<num, chs...>::result;
			};
			template<size_t num>struct SuperHelper{
				static const size_t once = sizeof...(chs);
				static const char result = Helper<(num < once), num>::result;
			};
			static const char result[258];
		};
		template<char... chs>
		const char TMPStringToCString<TMPString<chs...>>::result[258] = {
#define M(n,data) TMPStringToCString<TMPString<chs...>>::SuperHelper<n>::result,
			TMPStringToCString<TMPString<chs...>>::SuperHelper<0>::result,
			NOB_REPEAT(256, M, ~) '\0'
#undef M
		};

		template<size_t num>struct NumToStringHelper{
			template<size_t num_2, typename CA>struct Helper;
			template<size_t num_2, char... chs>struct Helper<num_2, TMPString<chs...>>
			:Helper<num_2 / 10, TMPString<num_2 % 10 + '0', chs...>>{};
			template<char... chs>struct Helper<0, TMPString<chs...>>
				:TMPStringToCString<TMPString<chs...>>{
					typedef TMPString<chs...> Result;
			};
			typedef Helper<num, TMPString<>> Result;
		};
		//value : <const char[]> 编译期数值转字符串
		template<size_t num>struct NumToString :NumToStringHelper<num>::Result{};

		template<size_t num>struct ArraySizeHelperHelper{
			template<typename>struct Helper;
			template<char... chs>struct Helper<TMPString<chs...>>{
				typedef TMPString<'[', chs..., ']'> Result;
			};
			typedef TMPStringToCString<typename Helper
				<typename NumToStringHelper<num>::Result::Result>::Result> Result;
		};
		template<size_t num>struct ArraySizeHelper :ArraySizeHelperHelper<num>::Result{};

		//Result: <type>最适合作为参数的类型
		template<typename _1>struct ParamType{
			typedef typename SelectType<IntrinsicType<_1>::result ||
				PointerType<_1>::result, _1, _1 const&>::Result Result;
		};
		template<typename _1>struct ParamType<_1&>{
			typedef _1& Result;
		};
		template<typename _1>struct ParamType<_1&&>{
			typedef _1&& Result;
		};
};


namespace Types{
	//size  : <size_t> 类型数组的大小
	//Result: <type> 首项【空数组无定义】
	//Next  : <TypeArray> 除首项以外的项【空数组无定义】
	//get   : <template<_>> 选出第N个项
	//add   : <template<_>> 在数组末端加上一项
	template<typename... Args>struct TypeArray;
	template<typename Head, typename... Others>struct TypeArray<Head, Others...>{
		typedef TypeArray<Others...> Next;
		typedef Head Result;
		template<int index>struct get{
			typedef typename SelectTypeInGroup<index,Head,Others...>::Result Result;
		};
		template<typename... _1>struct add{
			typedef TypeArray<Head, Others..., _1...> Result;
		};
		template<typename _1>struct shift{
			typedef TypeArray<_1, Head, Others...> Result;
		};
		static const size_t size = sizeof...(Others)+1;
	};
	template<>struct TypeArray<>{
		template<int index>struct get;
		template<typename... _1>struct add{
			typedef TypeArray<_1...> Result;
		};
		template<typename _1>struct shift{
			typedef TypeArray<_1> Result;
		};
		static const size_t size = 0;
	};
	//Result: <TypeArray> 连接两个类型数组
	template<typename Array1, typename Array2>struct ArrayConcat;
	template<typename... Array1, typename... Array2>
	struct ArrayConcat<TypeArray<Array1...>, TypeArray<Array2...> >{
		typedef TypeArray<Array1..., Array2...> Result;
	};
	//Array1: <TypeArray> 将第二个类型数组的首项加在第一个类型数组末端
	//Array2: <TypeArray> 第二个类型数组除去首项
	template<typename Array1, typename Array2>struct ArrayTransfer;
	template<typename Array, typename... Types>
	struct ArrayTransfer<TypeArray<Types...>,Array>{
		typedef TypeArray<Types..., typename Array::Result> Array1;
		typedef typename Array::Next Array2;
	};
	//Result: <TypeArray> 在类型数组前端弹出times个项
	template<size_t times,typename Array>struct ArrayShift{
		typedef typename ArrayShift<times - 1, typename Array::Next>::Result Result;
	};
	template<typename Array>struct ArrayShift<0,Array>{
		typedef typename Array Result;
	}; 
	//Result: <TypeArray> 从类型数组末端弹出times个项
	template<size_t times, typename Array>struct ArrayPop{
		template<size_t index, typename Array_1, typename Array_2>struct ArrayTransport{
			typedef typename ArrayTransfer<Array_1, Array_2>::Array1 Array1;
			typedef typename ArrayTransport<index - 1, Array1,
			typename Array_2::Next>::Result Result;
		};
		template<typename Array_1, typename Array_2>
		struct ArrayTransport<0, Array_1, Array_2>{
			typedef Array_1 Result;
		};
		typedef typename ArrayTransport<Array::size - times,
			TypeArray<>, Array>::Result Result;
	};
	template<typename Array>struct ArrayPop<0,Array>{
		typedef Array Result;
	};
	//Result: <TypeArray> 选出类型数组里面[index1,index2]的项
	template<size_t index1, size_t index2, typename Array>struct ArraySelect{
		typedef typename ArrayShift<index1,
			typename ArrayPop<Array::size - index2 - 1, Array>::Result>::Result Result;
	};
	//Result: <TypeArray> 删除类型数组里面[index1,index2]的项
	template<size_t index1, size_t index2, typename Array>struct ArrayDelete{
		typedef typename ArrayConcat<typename ArrayPop
			<Array::size - index1, Array>::Result, typename ArrayShift
			<index2 + 1, Array>::Result>::Result Result;
	};
	//result: <bool> 类型数组存在一个项经过函数得到true
	template<typename Array, template<typename> class Func>struct ArrayAny{
		static const bool result = Func<typename Array::Result>::result ||
			ArrayAny<typename Array::Next,Func>::result;
	};
	template<template<typename> class Func>
	struct ArrayAny<TypeArray<>,Func>{
		static const bool result = false;
	};
	//result: <bool> 类型数组每一个项经过函数得到true
	template<typename Array, template<typename> class Func>
	struct ArrayAll{
		static const bool result = Func<typename Array::Result>::result &&
		ArrayAny<typename Array::Next, Func>::result;
	};
	template<template<typename> class Func>
	struct ArrayAll<TypeArray<>, Func>{
		static const bool result = true;
	};
	//Result: <TypeArray> 将每个类型进行迭代转化为新类型
	template<typename Array,template<typename> class Func>struct ArrayMap;
	template<typename... Types, template<typename> class Func>
	struct ArrayMap<TypeArray<Types...>, Func>{
		typedef TypeArray<typename Func<Types>::Result...> Result;
	};
	//Result: <TypeArray> 将每个类型进行迭代转化为新类型
	template<typename Array, template<typename> class Func>
	using ArrayCollect = ArrayMap<Array,Func>;
	//Result: <TypeArray> 选出符合要求的类型
	template<typename Array, template<typename> class Func>
	struct ArraySelectIf{
		template<typename Array1, typename Array2>struct Transport{
			typedef typename Transport<typename SelectType<
				Func<typename Array2::Result>::result,
				typename ArrayTransfer<Array1, Array2>::Array1, Array1>::Result,
				typename Array2::Next>::Result Result;
		};
		template<typename Array1>struct Transport<Array1, TypeArray<> >{
			typedef Array1 Result;
		};
		typedef typename Transport<TypeArray<>, Array>::Result Result;
	};
	//Result: <TypeArray> 清除符合要求的类型
	template<typename Array, template<typename> class Func>
	struct ArrayDeleteIf{
		template<typename Array1, typename Array2>struct Transport{
			typedef typename Transport<typename SelectType
			<!Func<typename Array2::Result>::result,
			typename ArrayTransfer<Array1, Array2>::Array1, Array1>::Result,
			typename Array2::Next>::Result Result;
		};
		template<typename Array1>struct Transport<Array1, TypeArray<> >{
			typedef Array1 Result;
		};
		typedef typename Transport<TypeArray<>, Array>::Result Result;
	};
	//Result: <TypeArray> 在指定索引前插入类型
	template<typename Array, size_t index, typename Type>
	struct ArrayInsert{
		typedef typename ArrayConcat<typename ArrayConcat<typename ArrayPop
		<Array::size - index, Array>::Result, TypeArray<Type> >::Result,
		typename ArrayShift<index, Array>::Result>::Result Result;
	};
	//Result: <TypeArray> 将类型数组中第index个替换为Type
	template<typename Array, size_t index, typename Type>
	struct ArrayReplace{
		typedef typename ArrayInsert<typename ArrayDelete
			<index, index, Array>::Result, index,Type>::Result Result;
	};
	//result: <bool> 判断类型数组中是否含有该类型
	template<typename Array,typename Type>struct ArrayInclude;
	template<typename... Types, typename Type>
	struct ArrayInclude<TypeArray<Types...>, Type>{
		static const bool result = AnySameType<Type, Types...>::result;
	};
	template<typename Type>struct ArrayInclude<TypeArray<>, Type>{
		static const bool result = false;
	};
	//Result: <TypeArray> 对类型进行排序
	template<typename Array,template<typename,typename> class Func>struct ArraySort{
		typedef typename Array::Result First;
		template<typename Type>struct Compare_1{
			static const bool result = Func<Type, First>::result;
		};
		template<typename Type>struct Compare_2{
			static const bool result = !Func<Type, First>::result;
		};
		typedef typename ArraySelectIf<typename Array::Next, Compare_1>::Result Left;
		typedef typename ArraySelectIf<typename Array::Next, Compare_2>::Result Right;
		typedef typename ArrayConcat<typename ArraySort<Left, Func>::Result,
			typename ArrayConcat<TypeArray<First>,
			typename ArraySort<Right, Func>::Result>::Result>::Result Result;
	};
	template<typename Type1, typename Type2, template<typename, typename> class Func>
	struct ArraySort<TypeArray<Type1, Type2>, Func>{
		typedef typename SelectType<Func<Type1, Type2>::result,
			TypeArray<Type1, Type2>, TypeArray<Type2, Type1> >::Result Result;
	};
	template<typename Type, template<typename, typename> class Func>
	struct ArraySort<TypeArray<Type>, Func>{
		typedef TypeArray<Type> Result;
	};
	template<template<typename, typename> class Func>
	struct ArraySort<TypeArray<>, Func>{
		typedef TypeArray<> Result;
	};
	//Result: <TypeArray> 去除重复的类型
	template<typename Array>struct ArrayUnique{
		template<typename Array1,typename Array2>struct Transport{
			typedef typename Transport<typename SelectType<
				ArrayInclude<Array1,typename Array2::Result>::result, Array1,
				typename ArrayTransfer<Array1, Array2>::Array1
				>::Result,typename Array2::Next>::Result Result;
		};
		template<typename Array1>struct Transport<Array1, TypeArray<> >{
			typedef Array1 Result;
		};
		typedef typename Transport<TypeArray<>, Array>::Result Result;
	};
	//Result: <TypeArray> 按定制的函数去除重复的类型
	template<typename Array,template<typename,typename> class Func = SameType>
	struct ArrayUniqueIf{

		template<typename Array, typename Type>struct Helper{
			template<typename Type_1>struct SameUnary{
				static const bool result = Func<Type, Type_1>::result;
			};
			static const bool result = ArrayAny<Array, SameUnary>::result;
		};

		template<typename Array1, typename Array2>struct Transport{
			typedef typename Transport<typename SelectType<
			Helper<Array1, typename Array2::Result>::result, Array1,
			typename ArrayTransfer<Array1, Array2>::Array1
			>::Result, typename Array2::Next>::Result Result;
		};
		template<typename Array1>struct Transport<Array1, TypeArray<> >{
			typedef Array1 Result;
		};
		typedef typename Transport<TypeArray<>, Array>::Result Result;
	};
	template<typename Array>struct ArrayUniqueIf<Array,SameType>{
		template<typename Array1, typename Array2>struct Transport{
			typedef typename Transport<typename SelectType<
			ArrayInclude<Array1, typename Array2::Result>::result, Array1,
			typename ArrayTransfer<Array1, Array2>::Array1
			>::Result, typename Array2::Next>::Result Result;
		};
		template<typename Array1>struct Transport<Array1, TypeArray<> >{
			typedef Array1 Result;
		};
		typedef typename Transport<TypeArray<>, Array>::Result Result;
	};
	//Result: <TypeArray> 将索引与类型同时迭代获得新类型
	template<typename, template<size_t, typename>class>struct ArrayMapWithIndex;
	template<typename... Types, template<size_t, typename>class Func>
	struct ArrayMapWithIndex<TypeArray<Types...>, Func>{
		template<size_t n, typename Array>struct Iter{
			typedef typename TypeArray<Types...>::template
				get<TypeArray<Types...>::size - n>::Result _1;
			typedef typename Array::template add<typename Func
				<TypeArray<Types...>::size - n, _1>::Result>::Result NewArray;
			typedef typename Iter<n - 1, NewArray>::Result Result;
		};
		template<typename Array>struct Iter<0, Array>{
			typedef Array Result;
		};
		typedef typename Iter<TypeArray<Types...>::size, TypeArray<>>::Result Result;
	};
	//Result: <TypeArray> 将嵌套TypeArray展开
	template<typename Array>struct ArrayFlatten{
		template<typename T, typename NotCheck, typename Checked>struct Helper
			:Helper<typename NotCheck::Result, typename NotCheck::Next,
			typename Checked::template add<T>::Result>{};
		template<typename T, typename Checked>
		struct Helper<T, TypeArray<>, Checked>{
			typedef typename Checked::template add<T>::Result Result;
		};
		template<typename... Types, typename NotCheck, typename Checked>
		struct Helper<TypeArray<Types...>, NotCheck, Checked>
			:Helper<typename NotCheck::Result, typename NotCheck::Next,
			typename Checked::template add<Types...>::Result>{};
		template<typename... Types, typename Checked>
		struct Helper<TypeArray<Types...>, TypeArray<>, Checked>{
			typedef typename Checked::template add<Types...>::Result Result;
		};
		typedef typename Helper<typename Array::Result, typename Array::Next,
			TypeArray<>>::Result Result;
	};
	template<>struct ArrayFlatten<TypeArray<>>{
		typedef TypeArray<> Result;
	};
};

template<typename Func>
std::function<typename Types::FunctorType<Func>::Result> make_func(Func const& fun){
	return std::function<typename Types::FunctorType<Func>::Result>(fun);
};