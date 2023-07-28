#include "fixedVector.hpp"

//constructor
FixedVector::FixedVector(int max) : _fixedVec({})
{
	this->_maxSize = max;
}

int FixedVector::push_back(int val)
{
	if (this->_fixedVec.size() < this->_maxSize)
	{
		this->_fixedVec.push_back(val);
		return 1;
	}
	else
		return 0;
}

void FixedVector::pop_back()
{
	this->_fixedVec.pop_back();
}


bool FixedVector::empty()
{
	return this->_fixedVec.empty();
}


int& FixedVector::operator[](int ind)
{
	return this->_fixedVec[ind];
}

int FixedVector::size() const
{
	return this->_fixedVec.size();
}