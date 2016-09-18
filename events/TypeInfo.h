#pragma once

#include <typeinfo>

class TypeInfo
{
public:
    explicit TypeInfo(const std::type_info& info) : mTypeInfo(info) {};

	bool operator < (const TypeInfo& rhs) const
	{
		return mTypeInfo.before(rhs.mTypeInfo) != 0;
	}

private:
    const std::type_info& mTypeInfo;
};
