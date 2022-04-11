#include "connection.h"

//sf::Packet& operator<<(sf::Packet& packet, const message_t& message) {
//	packet << (sf::Int32)(int)message.type;
//	switch (message.type) {
//		case message_t::STRING: packet << *(std::string*)message.data.get();
//			break;
//
//	}
//	return packet;
//}
//sf::Packet& operator>>(sf::Packet& packet, message_t& message) {
//	sf::Int32 t;
//	packet >> t;
//	message.type = (message_t::type_t)(int)t;
//	switch (message.type) {
//		case message_t::STRING: {
//			std::string buf;
//			packet >> buf;
//			message.data = std::make_unique<std::string>(buf);
//			break;
//		}
//	}
//	return packet;
//}

void letterSender::send(const std::string& msg) {
	if (s != MAILING)return;
	sf::Packet packet;
	packet << msg;
	mtx.lock();
	socket.send(packet);
	mtx.unlock();
	printf("send: [%s]\n", msg.c_str());
}
bool letterSender::tryReceive() {
	socket.setBlocking(false);

	sf::Packet packet;
	std::this_thread::sleep_for(std::chrono::seconds(1));
	mtx.lock();
	sf::Socket::Status status = socket.receive(packet);
	mtx.unlock();
	if (status == sf::Socket::Done) {
		packet >> msgbuf;
		return true;
	}
	return false;
}
std::string letterSender::getmsg() const {
	return msgbuf;
}

void letterSenderServer::lifeCycle(letterSenderServer* ls) {
	sf::IpAddress address = sf::IpAddress::getLocalAddress();
	sf::TcpListener listener;
	listener.setBlocking(false);
	if (listener.listen(sf::Socket::AnyPort) != sf::Socket::Done) {
		ls->s = ERROR;
		return;
	}
	ls->s = GOTIP;
	int port = listener.getLocalPort();

	ls->IPnPort = address.toString() + "  " + std::to_string(port);
	while (listener.accept(ls->socket) != sf::Socket::Done) {
		if (ls->s == STOP) {
			return;
		}
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	ls->selector.add(ls->socket);

	ls->s = MAILING;
	while (true) {
		if (ls->s == MAILING) {
			ls->tryReceive();
			printf("tryReceive: [%s]\n", ls->msgbuf.c_str());
		} else if (ls->s == ERROR || ls->s == STOP) {
			break;
		}
	}
}
letterSenderServer::letterSenderServer() {
	s = CONNECTING;
	waiter = std::make_unique<std::thread>(lifeCycle, this);
}
letterSenderServer::~letterSenderServer() {
	s = STOP;
	waiter->join();
}
std::string letterSenderServer::getIPnPort() const {
	return IPnPort;
}

void letterSenderClient::lifeCycle(letterSenderClient* ls, sf::IpAddress addr, int port) {
	std::cout << "<" << addr << "> <" << port << ">\n";
	sf::Socket::Status status;
	do {
		status = ls->socket.connect(addr, port, sf::seconds(1));
	} while (ls->s != STOP && status == sf::Socket::NotReady);
	if (status != sf::Socket::Done) {
		ls->s = ERROR;
		return;
	}
	ls->s = MAILING;
	while (true) {
		//printf("*\n");
		if (ls->s == MAILING) {
			ls->tryReceive();
			printf("tryReceive: [%s]\n", ls->msgbuf.c_str());
		} else if (ls->s == ERROR || ls->s == STOP) {
			break;
		}
	}
}
letterSenderClient::letterSenderClient(const std::string& args) {
	std::stringstream ss;
	ss << args;
	sf::IpAddress ip;
	int port;
	ss >> ip >> port;
	s = CONNECTING;
	waiter = std::make_unique<std::thread>(lifeCycle, this, ip, port);
}
letterSenderClient::~letterSenderClient() {
	s = STOP;
	waiter->join();
}