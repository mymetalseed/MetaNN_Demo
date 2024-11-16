#pragma once

namespace MetaNN
{
	namespace Helper {

		template <auto N>
		struct Int_
		{
			constexpr static auto value = N;
		};
		// ================================================================================
	// MakeIndexSequence ==============================================================
		template <int... I> struct IndexSequence;

	}
}