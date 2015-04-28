#include "Type.h"
#include "Utils/StringUtils.h"
#include "Application/Common.h"

namespace novus
{

Type::Type(const char* className)
{
	int length = static_cast<int>(strlen(className));
	ZeroMemory(mTypeName, 64);
	memcpy(mTypeName, className, length);
	mTypeHash = reinterpret_cast<TypeId>(HashedString::hash_name(className)); //TODO: replace with MurmurHash
}

Type::~Type()
{
}

bool Type::operator ==(const Type& other) const
{
	return this->mTypeHash == other.mTypeHash;
}

bool Type::operator !=(const Type& other) const
{
	return this->mTypeHash == other.mTypeHash;
}

bool Type::operator <(const Type& other) const
{
	return mTypeHash < other.mTypeHash;
}

}