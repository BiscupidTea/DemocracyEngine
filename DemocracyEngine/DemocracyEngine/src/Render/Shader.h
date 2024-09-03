#pragma once
#include <iostream>

using namespace std;

namespace DemoEngine_Renderer
{
	class Shader
	{
	public:
		Shader();
		~Shader();
		unsigned int CreateShader(const string& vertexShader, const string& fragmentShader);
		unsigned int CompileShader(unsigned int type, const string& source);

	private:

	};


}