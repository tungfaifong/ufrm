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
	static constexpr size_t INVALID_ID = -1;

	class Iterator
	{
		public:
			Iterator(ObjectMap & map, size_t id):_map(map), _id(id) {}
			~Iterator() = default;

			void operator=(const Iterator & iter)
			{ 
				_map = iter._map;
				_id = iter._id;
			}
			auto operator *() { return std::pair<size_t, const std::shared_ptr<T> &>(_id, _map._objects[_id].obj); }
			Iterator & operator ++()
			{
				_id = _map._objects[_id].next;
				return *this; 
			}
			bool operator !=(const Iterator & iter) { return _id != iter._id; }

			size_t ID() { return _id; }

		private:
			ObjectMap & _map;
			size_t _id;
	};

	struct Node
	{
		std::shared_ptr<T> obj = nullptr;
		size_t prev = INVALID_ID;
		size_t next = INVALID_ID;

		void Reset()
		{
			obj = nullptr;
			prev = INVALID_ID;
			next = INVALID_ID;
		}
	};

	ObjectMap(size_t alloc_num) : _alloc_num(alloc_num)
	{
		_Allocate();
	}

	~ObjectMap() = default;

	const std::shared_ptr<T> & operator[](size_t id)
	{
		return _objects[id].obj;
	}

	bool Find(size_t id)
	{
		if (id >= _objects.size() || !_objects[id].obj)
		{
			return false;
		}

		return true;
	}

	size_t Insert(std::shared_ptr<T> && obj)
	{
		auto id = _indexs[_head];
		if(_head_id == INVALID_ID)
		{
			_head_id = id;
		}
		if(_tail_id != INVALID_ID)
		{
			_objects[_tail_id].next = id;
			_objects[id].prev = _tail_id;
		}
		_tail_id = id;
		_objects[id].obj = std::move(obj);

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
		if (id >= _objects.size() || _objects[id].obj == nullptr)
		{
			return;
		}

		auto prev_id = _objects[id].prev;
		auto next_id = _objects[id].next;
		if(id == _head_id)
		{
			_head_id = next_id;
			if(_head_id != INVALID_ID)
			{
				_objects[_head_id].prev = INVALID_ID;
			}
		}
		else
		{
			if(prev_id != INVALID_ID)
			{
				_objects[prev_id].next = next_id;
			}
		}
		if(id == _tail_id)
		{
			_tail_id = prev_id;
			if(_tail_id != INVALID_ID)
			{
				_objects[_tail_id].next = INVALID_ID;
			}
		}
		else
		{
			if(next_id != INVALID_ID)
			{
				_objects[next_id].prev = prev_id;
			}
		}
		_objects[id].Reset();

		_indexs[_tail] = id;
		_tail = (_tail + 1) % _objects.size();
	}

	Iterator Erase(Iterator iter)
	{
		auto id = iter.ID();
		++iter;
		Erase(id);
		return iter;
	}

	void Clear()
	{
		_objects.clear();
		_head = 0;
		_tail = 0;
		_indexs.clear();
		_head_id = INVALID_ID;
		_tail_id = INVALID_ID;
	}

	size_t Size()
	{
		return _tail >= _head ? _tail - _head : _objects.size() + _tail - _head;
	}

	Iterator begin()
	{
		return Iterator(*this, _head_id);
	}

	Iterator end()
	{
		return _tail_id == INVALID_ID ? Iterator(*this, INVALID_ID) : Iterator(*this, _objects[_tail_id].next);
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
	std::vector<Node> _objects;
	size_t _head = 0;
	size_t _tail = 0;
	std::vector<size_t> _indexs;
	size_t _head_id = INVALID_ID;
	size_t _tail_id = INVALID_ID;
};

NAMESPACE_CLOSE

#endif // USRV_OBJECT_MAP_HPP
