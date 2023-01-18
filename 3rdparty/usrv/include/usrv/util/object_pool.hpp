// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#ifndef USRV_OBJECT_POOL_HPP
#define USRV_OBJECT_POOL_HPP

#include <memory>
#include <vector>

#include "common.h"

NAMESPACE_OPEN

template<typename T>
class ObjectPool
{
public:
	ObjectPool(size_t alloc_num): _alloc_num(alloc_num)
	{
		_Allocate();
	}

	~ObjectPool() = default;

	std::shared_ptr<T> Get()
	{
		auto obj = std::move(_objects[_size]);

		++_size;
		if(_size == _objects.size())
		{
			_Allocate();
		}

		return obj;
	}

	void Put(const std::shared_ptr<T> && obj)
	{
		--_size;
		_objects[_size] = std::move(obj);
	}

private:
	void _Allocate()
	{
		_objects.reserve(_objects.capacity() + _alloc_num);
		for (size_t i = 0; i < _alloc_num; ++i)
		{
			_objects.emplace_back(std::make_shared<T>());
		}
	}

public:
	size_t _alloc_num;
	std::vector<std::shared_ptr<T>> _objects;
	size_t _size = 0;
};

NAMESPACE_CLOSE

#endif // USRV_OBJECT_POOL_HPP
