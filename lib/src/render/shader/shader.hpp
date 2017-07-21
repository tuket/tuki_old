#ifndef SHADER_HPP
#define SHADER_HPP

typedef int ShaderId;

// SHADER
class Shader
{

protected:

	Shader() : shaderId(-1) {}

	void loadFromString(const char* src, unsigned type);
	void loadFromFile(const char* fileName, unsigned type);

public:
	
	virtual void loadFromString(const char* src) = 0;
	virtual void loadFromFile(const char* fileName) = 0;

	bool isLoaded() const { return shaderId >= 0; }

	ShaderId getId() const { return shaderId; }

	// returns false on failure
	bool compile();

	void destroy();

protected:
	ShaderId shaderId;
};

// VERTEX SHADER
class VertexShader : public Shader
{
public:
	void loadFromString(const char* src);
	void loadFromFile(const char* fileName);
};

// FRAGMENT SHADER
class FragmentShader : public Shader
{
public:
	void loadFromString(const char* src);
	void loadFromFile(const char* fileName);
};

// GEOMETRY SHADER
class GeometryShader : public Shader
{
public:
	void loadFromString(const char* src);
	void loadFromFile(const char* fileName);
};


// SHADER PROGRAM
class ShaderProgram
{

protected:

	ShaderProgram() : program(-1), compiledOk(false) {}

public:

	void setVertexShader(VertexShader vertShad) { this->vertShad = vertShad; }
	void setFragmentShader(FragmentShader fragShad) { this->fragShad = fragShad;  }
	void setGeometryShader(GeometryShader geomShad) { this->geomShad = geomShad; }

	bool hasGeometryShader() { return geomShad.isLoaded(); }

	// return false if fail
	bool compile();
	bool link();

	void use();

protected:

	int program;
	VertexShader vertShad;
	FragmentShader fragShad;
	GeometryShader geomShad;

	bool compiledOk;
};


#endif