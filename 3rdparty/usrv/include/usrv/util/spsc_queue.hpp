// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>
// A single producer single consumer lock-free queue based on ring buffer.

#ifndef USRV_SPSC_QUEUE_HPP
#define USRV_SPSC_QUEUE_HPP

#include <atomic>
#include <cassert>
#include <cstring>
#include <new>

#include "common.h"

NAMESPACE_OPEN

class SpscQueue
{
public:
#define MIN(x, y) x < y ? x : y

#ifdef __cpp_lib_hardware_interference_size
	static constexpr size_t CACHELINE_SIZE = std::hardware_destructive_interference_size;
	static constexpr size_t BLOCK_SIZE = CACHELINE_SIZE;
#else
	static constexpr size_t CACHELINE_SIZE = 64;
	static constexpr size_t BLOCK_SIZE = CACHELINE_SIZE;
#endif

	struct Header
	{
		uint16_t size;
		uint16_t data16;
		uint32_t data32;
	};
	static constexpr size_t HEADER_SIZE = sizeof(Header);

	SpscQueue(size_t block_num):_block_num(block_num), _bytes(_block_num * BLOCK_SIZE)
	{
		assert(_block_num && !(_block_num & (_block_num - 1)));
		_buffer = (char *)aligned_alloc(CACHELINE_SIZE, _bytes);
	}

	~SpscQueue()
	{
		free(_buffer);
	}

	// block push
	void Push(const char * data, const Header & header)
	{
		const auto write_idx = _write_idx.load(std::memory_order_relaxed);
		const auto need_block = (header.size + HEADER_SIZE - 1) / BLOCK_SIZE + 1;
		while (_empty_block < need_block)
		{
			_empty_block = _block_num - write_idx + _read_idx.load(std::memory_order_acquire);
		}

		const auto offset = (write_idx & (_block_num - 1)) * BLOCK_SIZE;
		memcpy(_buffer + offset, &header, HEADER_SIZE);
		const auto len = MIN(header.size, _bytes - offset - HEADER_SIZE);
		memcpy(_buffer + offset + HEADER_SIZE, data, len);
		memcpy(_buffer, data + len, header.size - len);

		_empty_block -= need_block;

		_write_idx.store(write_idx + need_block, std::memory_order_release);
	}

	bool TryPush(const char * data, const Header & header)
	{
		const auto write_idx = _write_idx.load(std::memory_order_relaxed);
		const auto need_block = (header.size + HEADER_SIZE - 1) / BLOCK_SIZE + 1;
		if (_empty_block < need_block)
		{
			_empty_block = _block_num - write_idx + _read_idx.load(std::memory_order_acquire);
			if (_empty_block < need_block)
			{
				return false;
			}
		}

		const auto offset = (write_idx & (_block_num - 1)) * BLOCK_SIZE;
		memcpy(_buffer + offset, &header, HEADER_SIZE);
		const auto len = MIN(header.size, _bytes - offset - HEADER_SIZE);
		memcpy(_buffer + offset + HEADER_SIZE, data, len);
		memcpy(_buffer, data + len, header.size - len);

		_empty_block -= need_block;

		_write_idx.store(write_idx + need_block, std::memory_order_release);
		return true;
	}

	bool TryPop(char * data, Header & header)
	{
		const auto read_idx = _read_idx.load(std::memory_order_relaxed);
		if (_used_block == 0)
		{
			_used_block = _write_idx.load(std::memory_order_acquire) - read_idx;
			if(_used_block == 0)
			{
				return false;
			}
		}

		const auto offset = (read_idx & (_block_num - 1)) * BLOCK_SIZE;
		memcpy(&header, _buffer + offset, HEADER_SIZE);
		const auto len = MIN(header.size, _bytes - offset - HEADER_SIZE);
		memcpy(data, _buffer + offset + HEADER_SIZE, len);
		memcpy(data + len, _buffer, header.size - len);

		const auto read_block = (header.size + HEADER_SIZE - 1) / BLOCK_SIZE + 1;
		_used_block -= read_block;

		_read_idx.store(read_idx + read_block, std::memory_order_release);
		return true;
	}

	bool Empty()
	{
		return _write_idx.load(std::memory_order_acquire) == _read_idx.load(std::memory_order_acquire);
	}

private:
	alignas(CACHELINE_SIZE) size_t _block_num;
	size_t _bytes;
	char * _buffer;
	alignas(CACHELINE_SIZE) std::atomic<size_t> _write_idx = {0};
	size_t _empty_block = 0;
	alignas(CACHELINE_SIZE) std::atomic<size_t> _read_idx = {0};
	size_t _used_block = 0;
	char _padding[CACHELINE_SIZE - sizeof(std::atomic<size_t>) - sizeof(size_t)];
};

NAMESPACE_CLOSE

#endif // USRV_SPSC_QUEUE_HPP