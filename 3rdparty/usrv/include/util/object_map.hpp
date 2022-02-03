// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#ifndef USRV_OBJECT_MAP_HPP
#define USRV_OBJECT_MAP_HPP

#include <vector>

#include "common.h"

NAMESPACE_OPEN

template<typename T>
class ObjectMap
{
public:
	ObjectMap(size_t alloc_num) : _alloc_num(alloc_num)
	{
		_Allocate();
	}

	~ObjectMap() = default;

	const std::shared_ptr<T> & operator[](size_t id)
	{
		return _objects[id];
	}

	bool Find(size_t id)
	{
		if (id >= _objects.size() || !_objects[id])
		{
			return false;
		}

		return true;
	}

	size_t Insert(std::shared_ptr<T> && obj)
	{
		auto id = _indexs[_head];
		_objects[id] = std::move(obj);

		_head = (_head + 1) % _objects.size();
		if (_head == _tail)
		{
			_Allocate();
		}

		return id;
	}

	size_t Insert(T && obj)
	{
		return Insert(std::make_shared<T>(obj));
	}

	void Erase(size_t id)
	{
		if (id >= _objects.size() || _objects[id] == nullptr)
		{
			return;
		}

 		_objects[id] = nullptr;
		_indexs[_tail] = id;
		_tail = (_tail + 1) % _objects.size();
	}

	void Clear()
	{
		_objects.clear();
		_head = 0;
		_tail = 0;
		_indexs.clear();
	}

private:
	void _Allocate()
	{
		auto size = _objects.size();
		_objects.resize(size + _alloc_num);
		_head = size;
		_tail = 0;
		_indexs.resize(size + _alloc_num);
		for (size_t i = size; i < size + _alloc_num; ++i)
		{
			_indexs[i] = i;
		}
	}

private:
	size_t _alloc_num;
	std::vector<std::shared_ptr<T>> _objects;
	size_t _head = 0;
	size_t _tail = 0;
	std::vector<size_t> _indexs;
};

NAMESPACE_CLOSE

#endif // USRV_OBJECT_MAP_HPP
