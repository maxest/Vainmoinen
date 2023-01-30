#ifndef COMMON_LOGGER_H
#define COMMON_LOGGER_H

#include "common.hpp"



class CLogger
{
private:
	ofstream stream;

public:
	void open(const string& fileName)
	{
		stream.open(fileName.c_str());
	}

	void close()
	{
		stream.close();
	}

	template <class TYPE>
	ofstream& operator << (const TYPE& arg)
	{
		stream << arg;
		return stream;
	}
};
extern CLogger Logger;



#endif
