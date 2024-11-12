#pragma once

#include <MetaNN/facilities/traits.h>

namespace MetaNN
{
	//lower access
	template <typename TData>
	struct LowerAccessImpl;

	template <typename TData>
	auto LowerAccess(TData&& p) {
		using RawType = RemConstRef<TData>;
		//LowerAccessImpl用于实现一些不希望向框架用户暴露，但对MetaNN暴露的信息模板
		//forward是完美转发
		return LowerAccessImpl<RawType>(std::forward<TData>(p));
	}
}