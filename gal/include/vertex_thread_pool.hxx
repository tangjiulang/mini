#pragma once
#include <thread>
#include <unordered_map>
#include <vector>
#include <bs_thread_pool.hpp>
#include "painter.hxx"

namespace MINI {
class VertexThreadPool {
public:
	VertexThreadPool() {
		m_threadCount = std::thread::hardware_concurrency();
		m_threadPool = new BS::thread_pool(m_threadCount);
	}

	~VertexThreadPool() {
		for (PAINTER* painter : m_painters)
			delete painter;

		m_painters.clear();
		delete m_threadPool;
		m_threadPool = nullptr;
	}

	VertexThreadPool(const VertexThreadPool&) = delete;
	VertexThreadPool& operator=(const VertexThreadPool&) = delete;
public:
	int m_threadCount;
	std::vector<PAINTER*> m_painters;
	BS::thread_pool<0> *m_threadPool;
	std::unordered_map<std::thread::id, int> m_threadToPainter;
};
}

