#include <array>

template<typename T, size_t MAX, size_t... MAXs>
class XMatrix{
public:
	typedef XMatrix Self;
	typedef Self& SR;
	typedef Self const& SCR;
	typedef XMatrix<T, MAXs...> Sub;

	std::array<Sub, MAX> data;
	static size_t const CAPACITY = MAX * Sub::CAPACITY;
	static bool const SIMPLE = false;

	size_t size()const{ return MAX; };
	size_t capacity()const{ return CAPACITY; };
	Sub& operator[](int i){
		if (i < 0) i += size();
		return data[i]
	};
	Sub const& operator[](int i)const{
		if (i < 0) i += size();
		return data[i]
	};

	SR replace(std::initializer_list<T> const& list){
		return replace(list.begin(), list.end());
	};
	SR replace(std::initializer_list<Sub> const& list){
		auto xbegin = list.begin();
		auto xend = list.end();
		for (auto& item : data){
			if (xbegin == xend) break;
			data[i] = *xbegin++;
		};
	};
	template<typename Iter>SR replace(Iter xbegin, Iter xend){
		static size_t const capa = Sub::CAPACITY;
		for (size_t i = 0; i != MAX; ++i, xbegin += capa){
			if (xbegin >= xend) break;
			auto mend = xbegin + capa;
			data[i].replace(xbegin, mend);
		};
		return *this;
	};

};

template<typename T, size_t MAX>
class XMatrix<T, MAX>{
	typedef XMatrix Self;
	typedef Self& SR;
	typedef Self const& SCR;

	std::array<T, MAX> data;
public:
	static size_t const CAPACITY = MAX;
	static bool const SIMPLE = true;

	size_t size()const{ return MAX; };
	size_t capacity()const{ return CAPACITY; };
	T& operator[](int i){
		if (i < 0) i += size();
		return data[i]
	};
	T const& operator[](int i)const{
		if (i < 0) i += size();
		return data[i]
	};

	SR replace(std::initializer_list<T> const& list){ return replace(list.begin(), list.end()) };
	template<typename Iter>SR replace(Iter xbegin, Iter xend){
		for (auto& item : data){
			if (xbegin == xend) break;
			data[i] = *xbegin++;
		};
	};

};

template<typename T, size_t MAX, size_t... OTHER>
class Matrix{
	typedef XMatrix<T, MAX, OTHER...> Data;
	Data* data;
	class Count{
	private:
		int *count;
		Count& operator=(const Count&c);
	public:
		Count() :count(new int(1)){};
		Count(const Count &c) :count(c.count){ ++*count; };
		~Count(){ if (!(--*count)) delete count; };
		bool only()const{ return *count == 1; };
		bool reattach(const Count&c){
			++*c.count;
			if (!(--*count)){
				delete count;
				count = c.count;
				return true;
			};
			count = c.count;
			return false;
		};
	} wcount;

	Data& self(){ return *data; };
	Data const& self()const{ return *data; };
public:
	Matrix() :data(new Data){};
	~Matrix(){ if (wcount.only()) delete data; };
	Matrix& operator=(Matrix const& other){
		if (wcount.reattach(other.wcount))
			delete data;
		data = other.data;
		return *this;
	};

	Matrix(std::initializer_list<T> const& list) :Matrix(){
		data->replace(list);
	};
	Matrix(std::initializer_list<Data::Sub> const& list) :Matrix(){
		data->replace(list);
	};

};

template<typename T, size_t MAX>
class Matrix<T, MAX>{
	typedef XMatrix<T, MAX> Data;
	Data* data;
	class Count{
	private:
		int *count;
		Count& operator=(const Count&c);
	public:
		Count() :count(new int(1)){};
		Count(const Count &c) :count(c.count){ ++*count; };
		~Count(){ if (!(--*count)) delete count; };
		bool only()const{ return *count == 1; };
		bool reattach(const Count&c){
			++*c.count;
			if (!(--*count)){
				delete count;
				count = c.count;
				return true;
			};
			count = c.count;
			return false;
		};
	} wcount;

	Data& self(){ return *data; };
	Data const& self()const{ return *data; };
public:
	Matrix() :data(new Data){};
	~Matrix(){ if (wcount.only()) delete data; };
	Matrix& operator=(Matrix const& other){
		if (wcount.reattach(other.wcount))
			delete data;
		data = other.data;
		return *this;
	};

	Matrix(std::initializer_list<T> const& list){
		self().replace(list.begin(), list.end());
	};
	Matrix(std::initializer_list<T> const& list){
		self().replace(list.begin(), list.end());
	};

};

template<typename Contain, bool SIMPLE>
class XMBPtr{
	typedef Contain::Sub Sub;

};

template<typename Contain>
class XMBPtr<Contain, true>{
	
};

template<typename Contain>
class XMatrixPtr: XMBPtr<Contain, Contain::SIMPLE>{};