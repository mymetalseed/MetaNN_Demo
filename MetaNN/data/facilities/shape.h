#pragma once
#include <MetaNN/data/facilities/category_tags.h>
#include <MetaNN/facilities/cont_metafuns/helpers.h>
#include <array>
#include <cassert>
#include <numeric>
#include <stdexcept>

namespace MetaNN
{
	namespace NSShape
	{
		/*
		Shape是为了描述实例的形状，如矩阵就是Shape<2>，即二维形状，里面包含行数和列数

		*/

		//typename...代表参数套件
		//https://www.ibm.com/docs/zh-tw/i/7.5?topic=c11-parameter-packs
		//IndexToOffset提供了索引到数组偏移量的功能
		template <size_t uDimNum, typename TCurIndexType, typename... TRemainIndexType>
		size_t IndexToOffset(const std::array<size_t, uDimNum>& dims, size_t& gap, TCurIndexType curIdx, TRemainIndexType... remIdx) {
			constexpr size_t indexPos = uDimNum - sizeof...(TRemainIndexType) - 1;
			if (static_cast<size_t>(curIdx) >= dims[indexPos])
				throw std::runtime_error("Invalid dimension index.");
			//sizeof...后面的TRemainIndexType可以是一个args(一套东西的大小)
			//比如这里TRemainIndexType的模板声明就是...
			//这里表示如果参数套件个数为0，则只需要返回下标的大小
			if constexpr (sizeof...(TRemainIndexType) == 0)
			{
				return static_cast<size_t>(curIdx);
			}
			else {
				size_t curGap = 1;
				//每次把remIdx给减少一个，下一次递归的curIdx则为remIdx的第一个,直到remIdx为空的时候，即上面的 if constexpr的结束条件
				//然后把每个维度的乘和加起来
				//这个是高维的计算方式(如三维矩阵的索引等)
				size_t res = IndexToOffset(dims, curGap, remIdx...);
				gap = curGap * dims[indexPos + 1];
				res += static_cast<size_t>(curIdx) * gap;
				return res;
			}
		}

		template <size_t uDimNum, int...N>
		size_t IndexToOffset(const std::array<size_t, uDimNum>& dims,
			const std::array<size_t, uDimNum>& indexes,
			Helper::IndexSequence<N...>*) {
			size_t gap = 1;
			return IndexToOffset(dims, gap, std::get<N>(indexes)...);
		}

		template <size_t ID, typename TShape>
		void FillShape(TShape& pShape) {
			return;
		}

		template<size_t ID, typename TShape, typename TCurParam, typename... TShapeParameter>
		void FillShape(TShape& pShape, TCurParam curParams, TShapeParameter... shapes)
		{
			pShape[ID] = static_cast<size_t>(curParams);
			FillShape<ID + 1>(pShape, shapes...);
		}
	}
	//uDimNum即为数组的维度
	//m_dims为每个维度的长度
	template<size_t uDimNum>
	class Shape {
		static_assert(uDimNum > 0);
	public:
		constexpr static size_t DimNum = uDimNum;

		explicit Shpae() = default;

		//设置每个维度的长度
		template <typename... TIntTypes,
			//上面提供了一个可变长模板参数TIntTypes，这个是限制了每个维度的长度
			//下面的则是限制TIntTypes必须具备可以转换成size_t类型
			//使用方式: Shape<2> s(2,5);
			std::enable_if_t<(std::is_convertible_v<TIntTypes, size_t>&& ...)>* = nullptr>
		explicit Shape(TIntTypes... shapes)
		{
			static_assert(sizeof...(TIntTypes) == uDimNum);
			NSShape::FillShape<0>(m_dims，shapes...);
		}

		//两个维度相等则Shape相等
		bool operator == (const Shape& val) const
		{
			return m_dims == val.m_dims;
		}

		template <size_t vDimNum>
		bool operator == (const Shape<vDimnum>&) const
		{
			return false;
		}

		size_t Count() const
		{
			return std::accumulate(std::begin(m_dims), std::end(m_dims),
				static_cast<size_t>(1), std::multiplies<>());
		}

		template <typename... TIntTypes,
			std::enable_if_t<(std::is_constructible_v<TIntTypes, size_t>&&...)> *= nullptr>
		size_t IndexToOffset(TIntTypes... indexes) const {
			static_assert(sizeof...(TIntTypes) == uDimNum);
			size_t gap = 1;
			return NSShape::IndexToOffset(m_dims, gap, indexes...);
		}

		size_t IndexToOffset(const std::array<size_t, DimNum>& indexes) const
		{
			using TSeq = Helper::MakeIndexSequence<DimNum>;
			return NSShape::IndexToOffset(m_dims, indexes, (TSeq*)nullptr);
		}

		std::array<size_t, DimNum> OffsetToIndex(size_t offset) const
		{
			std::array<size_t, DimNum> res;
			for (int i = (int)DimNum - 1; i >= 0; --i) {
				res[i] = offset % m_dims[i];
				offset = (offset - res[i]) / m_dims[i];
			}

			if (offset != 0) {
				throw std::runtime_error("Offset out of bound!");
			}

			return res;
		}

		void ShiftIndex(std::array<size_t, DimNum>& indexes, int carry = 1) const {
			if (carry == 0) return;
			if (carry > 0) {
				size_t uCarry = (size_t)carry;
				for (int i = (int)DimNum - 1; i >= 0; --i) {
					indexes[i] += uCarry;
					uCarry = indexes[i] / m_dims[i];
					indexes[i] %= m_dims[i];
					if (uCarry == 0) return;
				}

				if (uCarry) {
					throw std::runtime_error("Overflow");
				}
			}
			else {
				throw std::runtime_error("Not implemented yet.");
			}
		}

		size_t operator[](size_t idx) const
		{
			assert(idx < DimNum);
			return m_dims[idx];
		}

		size_t& operator[](size_t idx)
		{
			assert(idx < DimNum);
			return m_dims[idx];
		}

		decltype(auto) begin() {
			return m_dims.begin();
		}
		decltype(auto) begin() const
		{
			return m_dims.begin();
		}

		decltype(auto) end()
		{
			return m_dims.end();
		}

		decltype(auto) end() const
		{
			return m_dims.end();
		}

		decltype(auto) rbegin()
		{
			return m_dims.rbegin();
		}

		decltype(auto) rbegin() const
		{
			return m_dims.rbegin();
		}

		decltype(auto) rend()
		{
			return m_dims.rend();
		}

		decltype(auto) rend() const
		{
			return m_dims.rend();
		}

		decltype(auto) size() const
		{
			return m_dims.size();
		}
	private:
		std::array<size_t, uDimNum> m_dims{};
	};

	//特化0维，即标量(一个数)
	template <>
	class Shape<0> {
	public:
		constexpr static size_t DimNum = 0;

		explicit Shape() = default;

		bool operator == (const Shape& val) const
		{
			return true;
		}

		template <size_t vDimNum>
		bool operator == (const Shape<vDimNum>&)const {
			return false;
		}

		size_t Count() const {
			return 1;
		}
		decltype(auto) size() const
		{
			return 0;
		}

	};

	template <size_t v1, size_t v2>
	bool operator != (const Shape<v1> val1, const Shape<v2> val2) {
		return !(val1 == val2);
	}

	//引入泛型推断
	//调用方式从
	// auto x = Shape<2>(2,3);
	//变成:
	// auto x = Shape(2,3);
	template <typename... TShapeParameter,
		std::enable_if_t<(std::is_convertible_v<TShapeParameter, size_t> && ...)>* = nullptr>
	explicit Shape(TShapeParameter...)->Shape<sizeof...(TShapeParameter)>;

}