// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#ifndef USRV_SERVER_UNIT_H
#define USRV_SERVER_UNIT_H

#include <thread>

#include "asio.hpp"

#include "unit.h"
#include "util/object_map.hpp"
#include "util/object_pool.hpp"
#include "util/spsc_queue.hpp"

NAMESPACE_OPEN

class Peer;

class ServerUnit : public Unit, public std::enable_shared_from_this<ServerUnit>
{
public:
	static constexpr uint16_t OPF = 1024;	// operation per frame

	using OnRecvFunc = std::function<void(NETID, char *, uint16_t)>;

	ServerUnit(size_t pp_alloc_num, size_t ps_alloc_num, size_t spsc_blk_num);
	virtual ~ServerUnit() = default;

	virtual bool Init() override final;
	virtual bool Start() override final;
	virtual bool Update(intvl_t interval) override final;
	virtual void Stop() override final;
	virtual void Release() override final;

public:
	void Listen(PORT port);
	NETID Connect(const IP & ip, PORT port, uint32_t timeout);
	void Disconnect(NETID net_id);
	bool Send(NETID net_id, const char * data, uint16_t size);
	void OnRecv(OnRecvFunc func);

private:
	bool _Recv(NETID & net_id, char * data, uint16_t & size);
	void _IoStart();
	asio::awaitable<void> _IoUpdate();
	asio::awaitable<void> _IoListen(PORT port);
	asio::awaitable<void> _IoConnect(const IP & ip, PORT port, std::promise<NETID> & promise_net_id);
	void _IoDisconnect(NETID net_id);
	void _IoRecv(NETID net_id, const char * data, uint16_t size);
	NETID _IoAddPeer(asio::ip::tcp::socket && socket);
	void _IoDelPeer(NETID net_id);

private:
	friend class Peer;

	std::thread _io_thread;
	asio::io_context _io_context;
	asio::steady_timer _timer;
	intvl_t _io_interval = 0;

	ObjectPool<Peer> _peer_pool;
	ObjectMap<Peer> _peers;

	SpscQueue _send_queue;
	SpscQueue _recv_queue;
	char _send_buffer[MESSAGE_BODY_SIZE];

	OnRecvFunc _on_recv;
	char _recv_buffer[MESSAGE_BODY_SIZE];
};

class Peer
{
public:
	Peer() = default;
	virtual ~Peer() = default;

public:
	void Start(NETID net_id, asio::ip::tcp::socket && socket, const std::shared_ptr<ServerUnit> & server);
	void Stop();
	asio::awaitable<void> Send(const char * data, uint16_t size);
	
private:
	asio::awaitable<void> _Recv();

private:
	NETID _net_id = INVALID_NET_ID;
	std::shared_ptr<asio::ip::tcp::socket> _socket;
	std::shared_ptr<ServerUnit> _server;
	char _send_buffer[MESSAGE_HEAD_SIZE + MESSAGE_BODY_SIZE];
	char _recv_buffer[MESSAGE_BODY_SIZE];
};

NAMESPACE_CLOSE

#endif // USRV_SERVER_UNIT_H
