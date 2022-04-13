#include "connection.h"

//sf::Packet& operator<<(sf::Packet& packet, const Message& message) {
//	packet << (sf::Int32)(int)message.type;
//	switch (message.type) {
//		case Message::STRING: packet << *(std::string*)message.data.get();
//			break;
//
//	}
//	return packet;
//}
//sf::Packet& operator>>(sf::Packet& packet, Message& message) {
//	sf::Int32 t;
//	packet >> t;
//	message.type = (Message::Type)(int)t;
//	switch (message.type) {
//		case Message::STRING: {
//			std::string buf;
//			packet >> buf;
//			message.data = std::make_unique<std::string>(buf);
//			break;
//		}
//	}
//	return packet;
//}

void LetterSender::Send(const std::string& msg) {
	if (status != MAILING)return;
	sf::Packet packet;
	packet << msg;
	mtx.lock();
	socket.send(packet);
	mtx.unlock();
	printf("Send: [%status]\n", msg.c_str());
}
bool LetterSender::TryReceive() {
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
std::string LetterSender::GetMessage() const {
	return msgbuf;
}

void LetterSenderServer::lifeCycle(LetterSenderServer* ls) {
	sf::IpAddress address = sf::IpAddress::getLocalAddress();
	sf::TcpListener listener;
	listener.setBlocking(false);
	if (listener.listen(sf::Socket::AnyPort) != sf::Socket::Done) {
		ls->status = ERROR;
		return;
	}
	ls->status = GOTIP;
	int port = listener.getLocalPort();

	ls->IPnPort = address.toString() + "  " + std::to_string(port);
	while (listener.accept(ls->socket) != sf::Socket::Done) {
		if (ls->status == STOP) {
			return;
		}
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	ls->selector.add(ls->socket);

	ls->status = MAILING;
	while (true) {
		if (ls->status == MAILING) {
			ls->TryReceive();
			printf("TryReceive: [%status]\n", ls->msgbuf.c_str());
		} else if (ls->status == ERROR || ls->status == STOP) {
			break;
		}
	}
}
LetterSenderServer::LetterSenderServer() {
	status = CONNECTING;
	waiter = std::make_unique<std::thread>(lifeCycle, this);
}
LetterSenderServer::~LetterSenderServer() {
	status = STOP;
	waiter->join();
}
std::string LetterSenderServer::GetIPnPort() const {
	return IPnPort;
}

void LetterSenderClient::LifeCycle(sf::IpAddress addr, int port) {
	std::cout << "<" << addr << "> <" << port << ">\n";
	sf::Socket::Status socketStatus;
	do {
		socketStatus = socket.connect(addr, port, sf::seconds(1));
	} while (status != STOP && socketStatus == sf::Socket::NotReady);
	if (socketStatus != sf::Socket::Done) {
		status = ERROR;
		return;
	}
	status = MAILING;
	while (true) {
		if (status == MAILING) {
			TryReceive();
			printf("TryReceive: [%socketStatus]\n", msgbuf.c_str());
		} else if (status == ERROR || status == STOP) {
			break;
		}
	}
}
LetterSenderClient::LetterSenderClient(const std::string& args) {
	std::stringstream ss;
	ss << args;
	sf::IpAddress ip;
	int port;
	ss >> ip >> port;
	status = CONNECTING;
	waiter = std::make_unique<std::thread>(&LetterSenderClient::LifeCycle, this, ip, port);
}
LetterSenderClient::~LetterSenderClient() {
	status = STOP;
	waiter->join();
}