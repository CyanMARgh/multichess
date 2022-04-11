#pragma once
#include <sstream>
#include <thread>
#include <SFML/Network.hpp>
#include <mutex>
#include <iostream>
#include <variant>

struct message_t {
	enum type_t : sf::Int32 {
		STRING
	} type;
	std::unique_ptr<void> data;

	friend sf::Packet& operator<<(sf::Packet& packet, const message_t& message);
	friend sf::Packet& operator>>(sf::Packet& packet, message_t& message);
};

class letterSender {
protected:
	std::string msgbuf;
	std::mutex mtx;
	sf::TcpSocket socket;
	sf::SocketSelector selector;
public:
	letterSender& operator=(letterSender&) = delete;
	void send(const std::string& msg);
	bool tryReceive();
	std::string getmsg() const;
	//std::string popmsg();
	enum status {
		START,
		CONNECTING,
		GOTIP,
		MAILING,
		ERROR,
		STOP
	} s = START;
};
class letterSenderServer : public letterSender {
	std::string IPnPort;
	std::unique_ptr<std::thread> waiter;

	static void lifeCycle(letterSenderServer* ls);
public:
	letterSenderServer();
	~letterSenderServer();
	std::string getIPnPort() const;
};
class letterSenderClient : public letterSender {
	std::unique_ptr<std::thread> waiter;
	static void lifeCycle(letterSenderClient* ls, sf::IpAddress addr, int port) ;
public:
	explicit letterSenderClient(const std::string& args);
	~letterSenderClient() ;
};