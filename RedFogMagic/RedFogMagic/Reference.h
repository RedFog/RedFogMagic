#pragma once
#include "Type.hpp"

#define $adsof(n) ((typename Types::AddPointer<decltype(n)>::Result)&(char&)(n))


template<typename T>class PseRef;

template<typename T>
class PseRef{
	T* item;
	template<typename Y>friend class PseRef;
public:
	PseRef() :item(nullptr){};
	explicit PseRef(T& val) :item($adsof(val)){};
	PseRef(PseRef const& w):item(w.item){};
	template<typename Y>PseRef(PseRef<Y> w) :item(w.item){};
	PseRef const& operator=(PseRef const& w)const{
		*item = *(w.item);
		return *this;
	};
	template<typename Y>PseRef const& operator=(Y&& w)const{
		*item = std::forward<Y&&>(w);
		return *this;
	};
	PseRef& reattach(T& val){ item = $adsof(val); return *this; };
	template<typename Y>PseRef& reattach(Y&& w){
		T& m = std::forward<Y&&>(w);
		item = $adsof(m);
		return *this;
	};
	template<typename Y>PseRef& reattach(PseRef<Y> && w){
		item = w.item;
		return *this;
	};
	PseRef& clear(){ item = nullptr; return *this; };
	PseRef& unattach(){ item = nullptr; return *this; };

	operator T&()const{ return *item; };
	operator PseRef<T const>()const{ return PseRef<T>(*item); };
	T& call()const{ return *item; };
	T* address()const{ return item; };
	PseRef* myaddress()const{ return this; };

	template<typename A>auto operator++(A)const->decltype((*item)++){
		return (*item)++;
	};
	template<typename A>auto operator--(A)const->decltype((*item)--){
		return (*item)--;
	};
	template<typename... A>auto operator++(A...)const->decltype(++(*item)){
		return ++(*item);
	};
	template<typename... A>auto operator--(A...)const->decltype(--(*item)){
		return --(*item);
	};
	template<typename... A>auto operator()(A&&... w)const
		->decltype((*item)(std::forward<A&&>(w)...)){
		return (*item)(std::forward<A&&>(w)...);
	};
	template<typename A>auto operator[](A&& w)const
		->decltype((*item)[std::forward<A&&>(w)]){
		return (*item)[std::forward<A&&>(w)];
	};
	template<typename... A>auto operator+(A...)const->decltype(+(*item)){
		return +(*item);
	};
	template<typename... A>auto operator-(A...)const->decltype(-(*item)){
		return -(*item);
	};
	template<typename A>auto operator+(A&& w)const
		->decltype((*item) + std::forward<A&&>(w)){
		return (*item) + std::forward<A&&>(w);
	};
	template<typename A>auto operator-(A&& w)const
		->decltype((*item) - std::forward<A&&>(w)){
		return (*item) - std::forward<A&&>(w);
	};
	template<typename... A>auto operator*(A...)const->decltype(*(*item)){
		return *(*item);
	};
	template<typename... A>auto operator&(A...)const->decltype(&(*item)){
		return &(*item);
	};
	template<typename A>auto operator*(A&& w)const
		->decltype((*item) * std::forward<A&&>(w)){
		return (*item) * std::forward<A&&>(w);
	};
	template<typename A>auto operator&(A&& w)const
		->decltype((*item) & std::forward<A&&>(w)){
		return (*item) & std::forward<A&&>(w);
	};
	template<typename A>auto operator->*(A&& w)const
		->decltype((*item)->*(std::forward<A&&>(w))){
		return (*item)->*(std::forward<A&&>(w));
	};
	template<typename A>auto operator/(A&& w)const
		->decltype((*item)/(std::forward<A&&>(w))){
		return (*item)/(std::forward<A&&>(w));
	};
	template<typename A>auto operator%(A&& w)const
		->decltype((*item)%(std::forward<A&&>(w))){
		return (*item)%(std::forward<A&&>(w));
	};
	template<typename A>auto operator>>(A&& w)const
		->decltype((*item)>>(std::forward<A&&>(w))){
		return (*item)>>(std::forward<A&&>(w));
	};
	template<typename A>auto operator<<(A&& w)const
		->decltype((*item)<<(std::forward<A&&>(w))){
		return (*item)<<(std::forward<A&&>(w));
	};
	template<typename A>auto operator>(A&& w)const
		->decltype((*item) > (std::forward<A&&>(w))){
		return (*item) > (std::forward<A&&>(w));
	};
	template<typename A>auto operator<(A&& w)const
		->decltype((*item) < (std::forward<A&&>(w))){
		return (*item) < (std::forward<A&&>(w));
	};
	template<typename A>auto operator>=(A&& w)const
		->decltype((*item) >= (std::forward<A&&>(w))){
		return (*item) >= (std::forward<A&&>(w));
	};
	template<typename A>auto operator<=(A&& w)const
		->decltype((*item) <= (std::forward<A&&>(w))){
		return (*item) <= (std::forward<A&&>(w));
	};
	template<typename A>auto operator==(A&& w)const
		->decltype((*item) == (std::forward<A&&>(w))){
		return (*item) == (std::forward<A&&>(w));
	};
	template<typename A>auto operator!=(A&& w)const
		->decltype((*item) != (std::forward<A&&>(w))){
		return (*item) != (std::forward<A&&>(w));
	};
	template<typename A>auto operator|(A&& w)const
		->decltype((*item) | (std::forward<A&&>(w))){
		return (*item) | (std::forward<A&&>(w));
	};
	template<typename A>auto operator^(A&& w)const
		->decltype((*item) ^ (std::forward<A&&>(w))){
		return (*item) ^ (std::forward<A&&>(w));
	};
	template<typename A>auto operator+=(A&& w)const
		->decltype((*item) += (std::forward<A&&>(w))){
		return (*item) += (std::forward<A&&>(w));
	};
	template<typename A>auto operator-=(A&& w)const
		->decltype((*item) -= (std::forward<A&&>(w))){
		return (*item) -= (std::forward<A&&>(w));
	};
	template<typename A>auto operator*=(A&& w)const
		->decltype((*item) *= (std::forward<A&&>(w))){
		return (*item) *= (std::forward<A&&>(w));
	};
	template<typename A>auto operator/=(A&& w)const
		->decltype((*item) /= (std::forward<A&&>(w))){
		return (*item) /= (std::forward<A&&>(w));
	};
	template<typename A>auto operator%=(A&& w)const
		->decltype((*item) %= (std::forward<A&&>(w))){
		return (*item) %= (std::forward<A&&>(w));
	};
	template<typename A>auto operator<<=(A&& w)const
		->decltype((*item) <<= (std::forward<A&&>(w))){
		return (*item) <<= (std::forward<A&&>(w));
	};
	template<typename A>auto operator>>=(A&& w)const
		->decltype((*item) >>= (std::forward<A&&>(w))){
		return (*item) >>= (std::forward<A&&>(w));
	};
	template<typename A>auto operator&=(A&& w)const
		->decltype((*item) &= (std::forward<A&&>(w))){
		return (*item) &= (std::forward<A&&>(w));
	};
	template<typename A>auto operator|=(A&& w)const
		->decltype((*item) |= (std::forward<A&&>(w))){
		return (*item) |= (std::forward<A&&>(w));
	};
	template<typename A>auto operator^=(A&& w)const
		->decltype((*item) ^= (std::forward<A&&>(w))){
		return (*item) ^= (std::forward<A&&>(w));
	};
	template<typename A>auto operator,(A&& w)const
		->decltype((*item) , (std::forward<A&&>(w))){
		return (*item) , (std::forward<A&&>(w));
	};
};


template<typename T>struct PseRefHelper{
	template<typename Y>static auto pomemhelper(Y* w)->decltype(w->operator->(), 0){};
	template<typename Y>static bool pomemhelper(...){};
	static const bool pomem = sizeof(pomemhelper<T>(nullptr)) == sizeof(0);
	template<typename Y>static auto nothelper(Y* w)->decltype(w->operator!(), 0){};
	template<typename Y>static bool nothelper(...){};
	static const bool notb = sizeof(nothelper<T>(nullptr)) == sizeof(0);
	template<typename Y>static auto bnothelper(Y* w)->decltype(w->operator~(), 0){};
	template<typename Y>static bool bnothelper(...){};
	static const bool bnot = sizeof(bnothelper<T>(nullptr)) == sizeof(0);

	template<bool w>struct PomemHelper :virtual public PseRef<T>{
		auto operator->()->decltype(call().operator->())const{
			return call().operator->();
		};
	};
	template<>struct PomemHelper<false>{};
	template<bool w>struct NotHelper :virtual public PseRef<T>{
		auto operator!()->decltype(!call())const{
			return !call();
		};
	};
	template<>struct NotHelper<false>{};
	template<bool w>struct BNotHelper :virtual public PseRef<T>{
		auto operator~()->decltype(~call())const{
			return ~call();
		};
	};
	template<>struct BNotHelper<false>{};

	struct Result:virtual public PseRef<T>, public PomemHelper<pomem>,
		public NotHelper<notb>, public BNotHelper<bnot>{
		Result() :PseRef(){};
		explicit Result(T& val) :PseRef(val){};
		Result(PseRef const& w) :PseRef(w){};
		template<typename Y>Result(typename PseRefHelper<Y>::Result w) :PseRef(w){};
	};
};

template<typename T>using Ref = typename PseRefHelper<T>::Result;
template<typename T>using CRef = typename PseRefHelper<T const>::Result;

template<typename T>struct MakeRefHelper;
template<typename T>struct MakeRefHelper<T&>{
	static Ref<T> ref(T& m){ return Ref<T>(m); };
	static CRef<T> cref(T& m){ return CRef<T>(m); };
};
template<typename T>auto make_ref(T&& w)
->decltype(MakeRefHelper<T>::ref(w)){
	return MakeRefHelper<T>::ref(w);
};
template<typename T>auto make_cref(T&& w)
->decltype(MakeRefHelper<T>::cref(w)){
	return MakeRefHelper<T>::cref(w);
};