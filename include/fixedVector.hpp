#pragma once
#include <vector>

class FixedVector
{
	std::vector<int> _fixedVec;
	int _maxSize;

public:
	FixedVector(int);
	int push_back(int);
	void pop_back();
	bool empty();
	int& operator[](int ind);
	int size() const;
};