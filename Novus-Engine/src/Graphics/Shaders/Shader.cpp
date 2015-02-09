#include "Shader.h"

namespace novus
{

Shader::Shader(const std::string& name)
	:
	mName(name),
	mpVertexShader(nullptr),
	mpInputLayout(nullptr),
	mpPixelShader(nullptr),
	mpGeometryShader(nullptr),
	mpComputeShader(nullptr),
	mpHullShader(nullptr),
	mpDomainShader(nullptr)
{
}

Shader::~Shader()
{
	ReleaseCOM(mpInputLayout);
	ReleaseCOM(mpVertexShader);
	ReleaseCOM(mpPixelShader);
	ReleaseCOM(mpGeometryShader);
	ReleaseCOM(mpComputeShader);
	ReleaseCOM(mpHullShader);
	ReleaseCOM(mpDomainShader);
}

}