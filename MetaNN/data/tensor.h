#pragma once

#include <MetaNN/data/facilities/continuous_memory.h>
#include <MetaNN/data/facilities/lower_access.h>
#include <MetaNN/data/facilities/shape.h>
#include <MetaNN/data/facilities/traits.h>
#include <MetaNN/facilities/cont_metafuns/helpers.h>
#include <MetaNN/facilities/traits.h>
#include <type_traits>
#include <stdexcept>

namespace MetaNN
{
	namespace NSTensor
	{
		template <typename TShape,typename TCurIndex,typename TNextParam,typename... TRemainParam>
		auto OffsetAndVal(const TShape& shape, size_t& gap, TCurIndex curIdx, TNextParam nextParam, TRemainParam... remPara)
		{
		}
	}
}