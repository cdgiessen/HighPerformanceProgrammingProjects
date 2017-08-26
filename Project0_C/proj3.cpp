#include <iostream>
#include <ostream>
#include <iomanip>
#include <cmath>
#include <limits>
#include <fstream>
#include <string>
#include <vector>

template <typename T>
class Vector {
public:
	std::vector<T> data;

	Vector() {}
	Vector(int vecSize) : data(vecSize) { }

	//put to output
	friend std::ostream& operator<<(std::ostream &strm, const Vector<T> &v) {

		strm << "[";
		for (typename std::vector<T>::const_iterator ii = v.data.begin(); ii != v.data.end(); ++ii)
		{
			strm << ", " << *ii;
		}
		strm << "]";

		return strm;
	}
};

template <typename T>
class Matrix{
public:
	std::vector<std::vector<T>> data;

	Matrix() {}
	Matrix(int rowSize, int colSize) : data(rowSize, std::vector<T>(colSize)) {	}

	Vector<T> operator*(Vector<T> vec) {
		Vector<T> out(vec.data.size());

		for (int i = 0; i < data.size(); i++)
		{
			T sum = 0;
			for (int j = 0; j < data.at(i).size(); j++)
			{
				sum += data.at(i).at(j) * vec.data.at(j);
			}
			out.data.at(i) = sum;
		}
		return out;
	}
};

bool ReadMatrixFile(std::string fileName, Matrix<int> &mat, Vector<int> &vec) {
	std::ifstream inputFile;
	inputFile.open(fileName);
	if (inputFile.is_open()) {

		int rows, cols;
		inputFile >> rows >> cols;
		mat = Matrix<int>(rows, cols);
		for (int i = 0; i < rows; i++)
		{
			for (int j = 0; j < cols; j++)
			{
				int n;
				inputFile >> n;
				mat.data.at(i).at(j) = n;
			}
		}

		int vecSize;
		inputFile >> vecSize;
		vec = Vector<int>(vecSize);
		for (int i = 0; i < vecSize; i++)
		{
			int n;
			inputFile >> n;
			vec.data.at(i) = n;
		}
		inputFile.close();
		return true;
	}
	else {
		return false;
	}
}

int main(int argc, char *argv[]) 

{
	Matrix<int> mat;
	Vector<int> vec;

	if (ReadMatrixFile("mv.txt", mat, vec)) {
		std::cout << "Result is " << (mat*vec) << std::endl;
		system("PAUSE");
	}
	else {
		std::cout << "Failed to read file" << std::endl;
	}
}