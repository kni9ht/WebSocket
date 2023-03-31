#include <iostream>
#include <boost/asio.hpp>
#include <thread>

using boost::asio::ip::tcp;

void read_messages(tcp::socket& socket) {
    try {
        for (;;) {
            boost::asio::streambuf buf;
            boost::asio::read_until(socket, buf, "\n");
            std::string message{boost::asio::buffers_begin(buf.data()), boost::asio::buffers_end(buf.data())};
            std::cout << "Received message: " << message;
        }
    } catch (std::exception& e) {
        std::cerr << "Error reading message: " << e.what() << std::endl;
    }
}

int main() {
    boost::asio::io_context io_context;
    tcp::socket socket(io_context);

    socket.connect(tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 12345));

    std::thread t(read_messages, std::ref(socket));

    std::string message;
    while (getline(std::cin, message)) {
        boost::asio::write(socket, boost::asio::buffer(message + "\n"));
    }

    t.join();

    return 0;
}