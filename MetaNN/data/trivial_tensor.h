#pragma once

#include <MetaNN/data/facilities/traits.h>
#include <MetaNN/data/facilities/traits.h>
#include <MetaNN/evaluate/eval_buffer.h>
#include <MetaNN/evaluate/eval_plan.h>
#include <MetaNN/facilities/_.h>
#include <type_traits>

namespace MetaNN
{
	namespace NSTrivialTensor
	{
		template <typename TScalarHandle,typename TOutputHandle,size_t uDim>
		class EvalItem : public BaseEvalItem
		{

		};
	}
}