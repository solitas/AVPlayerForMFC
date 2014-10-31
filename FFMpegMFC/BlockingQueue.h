#pragma once
#include "stdafx.h"

template<typename T>
class BlockingQueue
{
public:
	explicit BlockingQueue(std::size_t max_size) :_max_size(max_size), _q()
	{}

	BlockingQueue(const BlockingQueue&) = delete;

	BlockingQueue& operator=(const BlockingQueue&) = delete;

	~BlockingQueue() = default;

	std::size_t size();
	bool push(const T& item, const std::chrono::milliseconds& timeout);
	bool pop(T& item, const std::chrono::milliseconds& timeout);

private:
	std::size_t _max_size;
	std::queue<T> _q;
	std::mutex _mutex;
	std::condition_variable _item_pushed_cond;
	std::condition_variable _item_popped_cond;
};


template<typename T>
std::size_t BlockingQueue<T>::size()
{
	std::lock_guard<std::mutex> lock(_mutex);
	return _q.size();
}

template<typename T>
bool BlockingQueue<T>::pop(T& item, const std::chrono::milliseconds& timeout)
{
	std::unique_lock<std::mutex> ul(_mutex);
	if (_q.empty()) {
		if (_item_pushed_cond.wait_for(ul, timeout) == std::cv_status::timeout || _q.empty())
			return false;
	}
	item = _q.front();
	_q.pop();
	if (_q.size() >= _max_size - 1)
		_item_popped_cond.notify_all();
	return true;
}

template<typename T>
bool BlockingQueue<T>::push(const T& item, const std::chrono::milliseconds& timeout)
{
	std::unique_lock<std::mutex> ul(_mutex);
	if (_q.size() >= _max_size) {
		if (_item_popped_cond.wait_for(ul, timeout) == std::cv_status::timeout || _q.size() >= _max_size)
			return false;
	}
	_q.push(item);
	if (_q.size() <= 1)
		_item_pushed_cond.notify_all();
	return true;
}