#pragma once
#include "Type.hpp"
#include <tuple>

#ifndef normax
#define normax(a,b) (((a)>(b)) ? (a) : (b))
#define normin(a,b) (((a)<(b)) ? (a) : (b))
#endif

namespace Types{
	//wrapper
	template<typename T>class ObjectWrapper{
		T item;
	public:
		ObjectWrapper(T item) :item(item){};
		ObjectWrapper& operator=(T item){
			this->item = item;
			return *this;
		};
		operator T()const{
			return item;
		};
	};
	template<typename T>class ObjectWrapper<T&&>;
	template<typename T>ObjectWrapper<T> wrapper(T item){
		return ObjectWrapper<T>(item);
	};
	//占位符
	template<size_t n>struct Args{
		static const size_t id = n;
	};
	//函数式占位符
	template<size_t n, typename Func, typename Array>struct FuncArgs;
	template<size_t n, typename Func, typename... Args>
	struct FuncArgs<n, Func, TypeArray<Args...>>{
		Func fun;
		FuncArgs(Func const& fun) :fun(fun){};
		typedef typename GetFuncInfo<Func>::Ret Ret;
		Ret operator()(Args... args)const{
			return fun(args...);
		};
	};
	//从参数包内分离第N个参数
	template<size_t n>struct ArgsDetacher{
		template<typename Head, typename... Args>
		static typename SelectTypeInGroup<n, Head, Args...>::Result call
			(Head, Args &&... args){
			return ArgsDetacher<n - 1>::call(std::forward<Args&&>(args)...);
		};
	};
	template<>struct ArgsDetacher<0>{
		template<typename Head>static Head call(Head&& head, ...){
			return std::forward<Head&&>(head);
		};
	};
	//特殊tuple
	template<typename... Args>std::tuple<ObjectWrapper<Args>...>
		make_cache_tuple(Args &&... args){
		return std::make_tuple(wrapper<Args>(args)...);
	};
	
	template<size_t n, typename... Args>struct PMPArgsHelper{
		static const bool check = n < sizeof...(Args);
		template<bool check>struct Helper{
			typedef typename SelectTypeInGroup<n, Args...>::Result Result;
			static Result call(std::tuple<Args...> const& tuple){
				return std::get<n>(tuple);
			};
		};
		struct A{
			template<typename... W>
			A operator()(W...){ return *this; };
		};
		template<>struct Helper<false>{
			typedef A Result;
			static Result call(std::tuple<Args...> const& tuple){
				return A();
			};
		};
		typedef typename Helper<check>::Result Result;
		static Result call(std::tuple<Args...> const& tuple){
			return Helper<check>::call(tuple);
		};
	};
	template<size_t n, typename... Args>struct PMPArgsHelper2{
		static const bool check = n < sizeof...(Args);
		template<bool check>struct Helper{
			typedef typename SelectTypeInGroup<n, Args...>::Result Result;
			static Result call(Args... tuple){
				return ArgsDetacher<n>::call(tuple...);
			};
		};
		struct A{
			template<typename... W>
			A operator()(W...){ return *this; };
		};
		template<>struct Helper<false>{
			typedef A Result;
			static Result call(Args&&... tuple){
				return A();
			};
		};
		typedef typename Helper<check>::Result Result;
		static Result call(Args... tuple){
			return Helper<check>::call(tuple...);
		};
	};
	//tuple里分离出特定参数，位置不存在返回void*(0)
	template<size_t n, typename... Args>
	typename PMPArgsHelper<n, Args...>::Result 
		unpack_args(std::tuple<Args...> const& tuple){
			return PMPArgsHelper<n, Args...>::call(tuple);
	};
	//tuple展开成参数~Helper
	template<typename Func, typename... Args>
	struct TransponderHelper{
		typedef typename GetFuncInfo<Func>::Ret Ret;
		static Ret unpack(Func const& fun, Args... args, ...){
			return fun(args...);
		};
		static Ret call(Func const& fun, std::tuple<Args...> const& tuple){
#define M(n, data) ,unpack_args<n>(tuple)
			return unpack(unpack_args<0>(tuple)
				NOB_REPEAT(256,M,~)
				);
#undef M
		};
	};
	//tuple展开成参数
	template<typename Func, typename... Args>
	typename GetFuncInfo<Func>::Ret 
		transponder(Func const& fun, std::tuple<Args...> const& tuple){
			return TransponderHelper<Func, Args...>::call(fun, tuple);
	};
	//临时lambda
	template<typename Ret, typename Args>
	struct ArgsCatcher{
		ObjectWrapper<Args> item;
		ArgsCatcher(Args a) :item(a){};
		template<typename... W>
		Ret operator()(W &&... args)const{
			return item;
		};
	};
	template<typename Ret, size_t n>
	struct ArgsCatcher<Ret, Args<n>>{
		ArgsCatcher(...) {};
		template<typename... W>
		Ret operator()(W &&... args)const{
			return ArgsDetacher<n - 1>::call(std::forward<W&&>(args)...);
		};
	};
	template<typename Ret, size_t m, typename Func, typename... Array>
	struct ArgsCatcher<Ret, FuncArgs<m, Func, TypeArray<Array...>>>{
		Func fun;

		ArgsCatcher(FuncArgs<m, Func, TypeArray<Array...>> const& w)
		:fun(w.fun){};

		Ret unpack(Array... arr, ...)const{
			return fun(arr...);
		};
		template<typename... W>
		Ret operator()(W &&... args)const{
#define M(n, data) ,(PMPArgsHelper2<m - 1 + n, W...>::call(std::forward<W&&>(args)...))
			return unpack(PMPArgsHelper2<m - 1, W...>::call(std::forward<W&&>(args)...)
				NOB_REPEAT(256,M,~)
				);
#undef M
		};
	};
	//形成参数列表
	template<typename, typename>struct ParamListMaker;
	template<typename... Args, typename... Param>
	struct ParamListMaker<TypeArray<Args...>, TypeArray<Param...>>{
		template<size_t n, typename S>struct Sign{
			typedef S Type;
			static const size_t result = n;
		};
		template<size_t n, typename T>
		struct Enumerator{
			typedef Sign<99999, void> Result;
		};
		template<size_t n, size_t m>
		struct Enumerator<n, Types::Args<m>>{
			typedef Sign<m, typename SelectTypeInGroup<n, Args...>::Result> Result;
		};
		template<size_t n, size_t m, typename Func, typename... Arr>
		struct Enumerator<n, FuncArgs<m, Func, TypeArray<Arr...>>>{
			template<typename Array, size_t w>struct Helper{
				typedef typename Array::template shift<Sign<w - 1 + m, 
				typename SelectTypeInGroup<w - 1, Arr...>::Result>>::Result NewArray;
				typedef typename Helper<NewArray, w - 1>::Result Result;
			};
			template<typename Array>struct Helper<Array, 0>{
				typedef Array Result;
			};
			static const size_t size = sizeof...(Arr);
			typedef typename Helper<TypeArray<>, size>::Result Result;
		};
		typedef typename ArrayMapWithIndex<TypeArray<Param...>, Enumerator>
			::Result XOnce;
		typedef typename ArrayFlatten<XOnce>::Result Once;
		template<typename T>struct Deleter{
			static const bool result = T::result == 99999;
		};
		typedef typename ArrayDeleteIf<Once, Deleter>::Result Twice;
		template<typename v1, typename v2>struct Sorter{
			static const bool result = (v1::result < v2::result);
		};
		typedef typename ArraySort<Twice, Sorter>::Result Third;
		template<typename v1, typename v2>struct Uniquer{
			static const bool result = (v1::result == v2::result);
		};
		typedef typename ArrayUniqueIf<Third, Uniquer>::Result Fourth;
		template<typename T>struct Unpacker{
			typedef typename T::Type Result;
		};
		typedef typename ArrayMap<Fourth, Unpacker>::Result Result;
	};
	//Bind类
	template<typename Func, typename Function, typename Tuple>
	class BindFunction;
	template<typename Func, typename Ret, typename... XArgs, typename Tuple>
	class BindFunction<Func, Ret(XArgs...), Tuple>{
		Func fun;
		Tuple tuple;
		template<typename ArgsList>struct Helper;
		template<typename... ArgsList>struct Helper<TypeArray<ArgsList...>>{
			static Ret unpack(Func const& fun, ArgsList... args, ...){
				return fun(args...);
			};
		};
		Ret call(XArgs... args)const{
#define M(n, data) ,(unpack_args<n>(tuple))(args...)
			return Helper<typename GetFuncInfo<Func>::Args>::unpack
				(fun, (unpack_args<0>(tuple))(args...)
				NOB_REPEAT(256,M,~)
				);
#undef M
		};
	public:
		BindFunction(Func const& fun, Tuple const& tuple) :fun(fun), tuple(tuple){};
		Ret operator()(XArgs &&... args)const{
			return call(args...);
		};
	};
	//bind函数~helper
	template<typename List, typename Ret, typename FuncArgs, typename Func,
		typename... Params>struct BindHelper;
	template<typename... List, typename Ret, typename... FuncArgs, typename Func,
		typename... Params>struct BindHelper<TypeArray<List...>, Ret,
		TypeArray<FuncArgs...>, Func, Params...>{
		typedef std::tuple<ArgsCatcher<FuncArgs, Params>...> Tuple;
		typedef BindFunction<Func, Ret(List...), Tuple> Result;
		static Result call(Func const& fun, Params &&... param){
			auto a = std::make_tuple(ArgsCatcher<FuncArgs, Params>(param)...);
			return Result(fun, a);
		};
	};
	//函数转化为占位符
	template<size_t n, typename Func>
	FuncArgs<n, Func, typename GetFuncInfo<Func>::Args> func(Func const& fun){
		return FuncArgs<n, Func, typename GetFuncInfo<Func>::Args>(fun);
	};
	//bind函数
	template<typename Func, typename... Params>
	auto bind(Func const& fun, Params &&... param)
	->typename BindHelper<typename ParamListMaker<typename GetFuncInfo<Func>::Args,
		TypeArray<Params...>>::Result, typename GetFuncInfo<Func>::Ret,
		typename GetFuncInfo<Func>::Args, Func, Params...>::Result{
			return BindHelper<typename ParamListMaker<typename GetFuncInfo<Func>::Args,
		TypeArray<Params...>>::Result, typename GetFuncInfo<Func>::Ret, typename 
		GetFuncInfo<Func>::Args, Func, Params...>::call
		(fun, std::forward<Params&&>(param)...);
	};
};