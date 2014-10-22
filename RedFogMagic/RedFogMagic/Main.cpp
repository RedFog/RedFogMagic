#include <iostream>
#include "Matrix.h"

int main(){
	Matrix<int, 3, 5> a{
			{1, 3, 6, 9, 10},
			{6, 9, 1, 6, 3},
			{7, 6, 9, 4, 3}
	};

	system("PAUSE");
	return 0;
};