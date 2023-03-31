#include <bits/stdc++.h>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/enable_shared_from_this.hpp>

using boost::asio::ip::tcp;

const int max_length = 1024;

class TcpConnection : public boost::enable_shared_from_this<TcpConnection> {
public:
    typedef boost::shared_ptr<TcpConnection> pointer;

    static pointer create(boost::asio::io_context& io_context) {
        return pointer(new TcpConnection(io_context));
    }

    tcp::socket& socket() {
        return socket_;
    }

    void start(std::vector<pointer>& connections) {
        connections.push_back(shared_from_this());
        do_read(connections);
    }

    void deliver(const std::string& message) {
        bool write_in_progress = !write_msgs_.empty();
        write_msgs_.push_back(message);
        if (!write_in_progress) {
            do_write();
        }
    }

private:
    TcpConnection(boost::asio::io_context& io_context)
        : socket_(io_context) {}

    void do_read(std::vector<pointer>& connections) {
        auto self(shared_from_this());
        socket_.async_read_some(boost::asio::buffer(data_, max_length),
            [this, self, &connections](boost::system::error_code ec, std::size_t length) {
                if (!ec) {
                    std::string message(data_, length);
                    std::cout << "Received message: " << message << std::endl;
                    for (auto it = connections.begin(); it != connections.end(); ++it) {
                        (*it)->deliver(message);
                    }
                    do_read(connections);
                }
            });
    }

    void do_write() {
    auto self(shared_from_this());
    boost::asio::async_write(socket_, boost::asio::buffer(write_msgs_.front().data(), write_msgs_.front().length()),
        [this, self](boost::system::error_code ec, std::size_t bytes_transferred) {
            if (!ec) {
                std::cout << "Sent message (" << bytes_transferred << " bytes): " << write_msgs_.front() << std::endl;
                write_msgs_.pop_front();
                if (!write_msgs_.empty()) {
                    do_write();
                }
            } else {
                std::cerr << "Error sending message: " << ec.message() << std::endl;
            }
        });
    }

    tcp::socket socket_;
    char data_[max_length];
    std::deque<std::string> write_msgs_;
};

class TcpServer {
public:
    TcpServer(boost::asio::io_context& io_context, short port)
        : io_context_(io_context), acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) {
        start_accept();
    }

private:
    void start_accept() {
        TcpConnection::pointer new_connection =
            TcpConnection::create(io_context_);

        acceptor_.async_accept(new_connection->socket(),
            boost::bind(&TcpServer::handle_accept, this, new_connection,
                boost::asio::placeholders::error));
    }

    void handle_accept(TcpConnection::pointer new_connection,
        const boost::system::error_code& error) {
        if (!error) {
            new_connection->start(connections_);
        }

        start_accept();
    }

    boost::asio::io_context& io_context_;
    tcp::acceptor acceptor_;
    std::vector<TcpConnection::pointer> connections_;
};

int main() {
    boost::asio::io_context io_context;
    TcpServer server(io_context, 12345);
    boost::thread t(boost::bind(&boost::asio::io_context::run, &io_context));
    t.join();
    return 0;
}