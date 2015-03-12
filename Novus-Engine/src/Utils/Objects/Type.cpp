#include "Type.h"
#include "Utils/StringUtils.h"

namespace novus
{

Type::Type(const char* className)
{
	mTypeHash = reinterpret_cast<uint32_t>(HashedString::hash_name(className)); //TODO: replace with MurmurHash
}

Type::~Type()
{}

bool Type::operator ==(const Type& other) const
{
	return this->mTypeHash == other.mTypeHash;
}

bool Type::operator !=(const Type& other) const
{
	return this->mTypeHash == other.mTypeHash;
}

}