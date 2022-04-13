#pragma once
#include <sstream>
#include <thread>
#include <SFML/Network.hpp>
#include <mutex>
#include <iostream>
#include <variant>

struct Message {
	enum Type : sf::Int32 {
		STRING
	} type;
	std::unique_ptr<void> data;

	friend sf::Packet& operator<<(sf::Packet& packet, const Message& message);
	friend sf::Packet& operator>>(sf::Packet& packet, Message& message);
};

class LetterSender {
protected:
	std::string msgbuf;
	std::mutex mtx;
	sf::TcpSocket socket;
	sf::SocketSelector selector;
public:
	LetterSender& operator=(LetterSender&) = delete;
	void Send(const std::string& msg);
	bool TryReceive();
	std::string GetMessage() const;
	//std::string popmsg();
	enum Status {
		START,
		CONNECTING,
		GOTIP,
		MAILING,
		ERROR,
		STOP
	} status = START;
};
class LetterSenderServer : public LetterSender {
	std::string IPnPort;
	std::unique_ptr<std::thread> waiter;

	static void lifeCycle(LetterSenderServer* ls);
public:
	LetterSenderServer();
	~LetterSenderServer();
	std::string GetIPnPort() const;
};
class LetterSenderClient : public LetterSender {
	std::unique_ptr<std::thread> waiter;
	void LifeCycle(sf::IpAddress addr, int port);
public:
	explicit LetterSenderClient(const std::string& args);
	~LetterSenderClient() ;
};