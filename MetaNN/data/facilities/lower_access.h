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
		//LowerAccessImpl����ʵ��һЩ��ϣ�������û���¶������MetaNN��¶����Ϣģ��
		//forward������ת��
		return LowerAccessImpl<RawType>(std::forward<TData>(p));
	}
}