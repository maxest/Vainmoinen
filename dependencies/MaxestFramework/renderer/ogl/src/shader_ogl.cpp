#ifdef RENDERER_OGL

#include <renderer_ogl/shader_ogl.hpp>
#include <renderer_ogl/renderer_ogl.hpp>
#include <common/logger.hpp>



void CShader::createFromFile(const string& fileName, ShaderType::TYPE type, const string& macros)
{
	destroy();

	this->name = fileName;

	ifstream file(fileName.c_str());
	
	if (!file)
	{
		Logger << "\tERROR: Couldn't load shader from file: " << fileName << endl;
		exit(1);
	}

	string source = "#version 120\n";
	vector<string> macrosVector = split(macros, '+');
	for (uint i = 0; i < macrosVector.size(); i++)
		source += "#define " + macrosVector[i] + "\n";

	string fileLine;
	while (getline(file, fileLine))
		source += fileLine + string("\n");

	file.close();

	const char* source_pchar = source.c_str();

	id = glCreateShader(type);
	if (!id)
	{
		Logger << "\tERROR: Couldn't create shader: " << name << endl;
		exit(1);
	}
	glShaderSource(id, 1, &source_pchar, 0);
	glCompileShader(id);

	int compilationStatus;
	glGetShaderiv(id, GL_COMPILE_STATUS, &compilationStatus);
	if (compilationStatus == GL_FALSE)
	{
		// log the compilation errors to file
		// note that the log will contain wrong references to lines in which the errors occured; that's due to adding version and defines to the beginning of a source code

		int infoLogLength = 0;
		char *infoLogData;

		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &infoLogLength);
		infoLogData = new char[infoLogLength];

		glGetShaderInfoLog(id, infoLogLength, NULL, infoLogData);

		string errorsFileName = addPrefixToShortFileName(fileName, "_errors---") + ".txt";

		ofstream file(errorsFileName.c_str());
		file << "==== all lines numbers are offseted by " << 1 + macrosVector.size() << " ====" << endl;
		file << infoLogData;
		file.close();

		Logger << "\tERROR: Couldn't compile shader from file: " << fileName << endl;
		exit(1);
	}

	Logger << "\tOK: Shader loaded from file: " << fileName << endl;
}



void CShader::destroy()
{
	if (id != 0)
	{
		glDeleteShader(id);
		id = 0;
	}
}



#endif
