#include "Type.h"
#include "Utils/StringUtils.h"

namespace novus
{

Type::Type(const std::string& className)
{
	mTypeHash = reinterpret_cast<uint32_t>(HashedString::hash_name(className.c_str())); //TODO: replace with MurmurHash
}

Type::Type(uint32_t classNameHash)
	: mTypeHash(classNameHash)
{}

Type::~Type()
{}

bool Type::operator ==(const Type& other)
{
	return this->mTypeHash == other.mTypeHash;
}

bool Type::operator !=(const Type& other)
{
	return this->mTypeHash == other.mTypeHash;
}

}