#ifndef COMMON_COMMON_H
#define COMMON_COMMON_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <map>

using namespace std;



typedef unsigned char byte;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;



// the class in its interface uses (column, row) pair combination, but in fact the data are laid row by row
template <class TYPE>
struct Array2D
{
public:
	Array2D()
	{
		data2D = NULL;
		data = NULL;
	}

	Array2D(int colsNum, int rowsNum)
	{
		create(colsNum, rowsNum);
	};

	Array2D(int colsNum, int rowsNum, const TYPE& fillValue)
	{
		create(colsNum, rowsNum);
		fill(fillValue);
	};

	~Array2D()
	{
		destroy();
	}

	void create(int colsNum, int rowsNum)
	{
		this->rowsNum = rowsNum;
		this->colsNum = colsNum;

		data = new TYPE[rowsNum * colsNum];
		data2D = new TYPE*[rowsNum];

		for (int i = 0; i < rowsNum; i++)
			data2D[i] = &data[i * colsNum];
	}

	void destroy()
	{
		delete[] data2D;
		delete[] data;
	}

	void fill(const TYPE& value)
	{
		for (int j = 0; j < rowsNum; j++)
			for (int i = 0; i < colsNum; i++)
				data2D[j][i] = value;
	}

	const int& getRowsNum() const { return rowsNum; }
	const int& getColsNum() const { return colsNum; }

	TYPE& operator () (int column, int row)
	{
		return data2D[row][column];
	}

public:
	TYPE **data2D; // this only allocates rows of pointers that point to particular this->data addresses; this way all rows are coalesced

private:
	int rowsNum;
	int colsNum;

	TYPE *data; // one big chunk of coalesced data
};



template <class TYPE>
string toString(const TYPE& arg)
{
	ostringstream out;
	out << arg;
	return (out.str());
}



inline string revert(const string& s)
{
	string temp = s;

	for (uint i = 0; i < s.length(); i++)
		temp[s.length() - 1 - i] = s[i];

	return temp;
}



inline string extractExtension(const string& fileName)
{
	string extension = "";

	for (int i = fileName.length() - 1; i > -1; i--)
	{
		if (fileName[i] == '.')
			return revert(extension);
		else
			extension += fileName[i];
	}

	return "";
}



inline int getLastSlashIndex(const string& fileName)
{
	for (int i = fileName.length() - 1; i > -1; i--)
	{
		if (fileName[i] == '/')
		{
			return i;
		}
	}

	return -1;
}



inline string extractDir(const string& fileName)
{
	return fileName.substr(0, getLastSlashIndex(fileName) + 1);
}



inline string extractShortFileName(const string& fileName)
{
	int slashIndex = getLastSlashIndex(fileName);
	return fileName.substr(slashIndex + 1, fileName.length() - slashIndex - 1);
}



inline string addPrefixToShortFileName(const string& fileName, const string& prefix)
{
	return extractDir(fileName) + prefix + extractShortFileName(fileName);
}



inline vector<string> split(string input, char separator)
{
    vector<string> strings;
	int currentStringIndex = 0;
	uint stringEndingCharIndex;

	if (input.empty())
		return strings;

	input += separator;

	while ( (stringEndingCharIndex = input.find(separator, 0)) != string::npos )
	{
	    strings.push_back("");

		for (uint i = 0; i < stringEndingCharIndex; i++)
			strings[currentStringIndex].push_back(input[i]);

		input.erase(0, stringEndingCharIndex + 1);
		currentStringIndex++;
	}

	return strings;
}



#endif
