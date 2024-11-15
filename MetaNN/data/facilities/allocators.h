#pragma once
#include <MetaNN/data/facilities/device_tags.h>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <deque>

/// <summary>
/// MetaNN通过Allocator与ContinuousMemory这两个类模板来维护存储空间
/// </summary>
namespace MetaNN
{	
	//模板声明,接收一个Device标记的参数
	template <typename TDevice>
	struct Allocator;

	//通过特化引入不同Device的Allocator实例
	template <>
	struct Allocator<DeviceTags::CPU>
	{
	private:
		struct AllocHelper {
			std::unordered_map<size_t, std::deque<void*> > memBuffer;

			~AllocHelper() {
				for (auto& p : memBuffer) {
					auto& refVec = p.second;
					for (auto& p1 : refVec) {
						char* buf = (char*)(p1);
						delete buf;
					}
					refVec.clear();
				}
			}
		};

		struct DesImpl {
			DesImpl(std::deque<void*>& p_refPool)
				: m_refPool(p_refPool)
			{

			}

			void operator () (void* p_val) const
			{
				std::lock_guard<std_mutex> guard(m_mutex);
				m_refPool.push_back(p_val);
			}
		private:
			std::deque<void*>& m_refPool;
		};

	public:
		template<typename T>
		static std::shared_ptr<T> Allocate(size_t p_elemSize) {
			if (p_elemSize == 0) return nullptr;
			p_elemSize *= sizeof(T);

			if (p_elemSize & 0x3ff) {
				p_elemSize = ((p_elemSize >> 10) + 1) << 10;
			}
			//lock_guard会在构造时上锁，析构时解锁
			std::lock_guard<std::mutex> guard(m_mutex);

			static AllocHelper allocateHelper;
			auto& slot = allocateHelper.memBuffer[p_elemSize];
			if (slot.empty()) {
				auto raw_buf = (T*)new char[p_elemSize];
				return std::shared_ptr<T>(raw_buf, DesImpl(slot));
			}
			else {
				void* mem = slot.back();
				slot.pop_back();
				return std::shared_ptr<T>((T*)mem, DesImpl(slot));
			}
		}

	private:
		inline static std::mutex m_mutex;
	};
}
