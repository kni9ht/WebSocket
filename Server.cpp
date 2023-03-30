#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/version.hpp>
#include <boost/beast/ssl.hpp>

namespace asio = boost::asio;
namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace http = beast::http;
using tcp = boost::asio::ip::tcp;

void do_accept(tcp::acceptor& acceptor, asio::ssl::context& ctx);

int main()
{
    asio::io_context io_context;
    asio::ssl::context ctx(asio::ssl::context::tlsv12);
    ctx.use_certificate_chain_file("server.crt");
    ctx.use_private_key_file("server.key", asio::ssl::context::pem);
    ctx.set_verify_mode(asio::ssl::verify_none);
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 12345));
    do_accept(acceptor, ctx);
    io_context.run();
}

void do_accept(tcp::acceptor& acceptor, asio::ssl::context& ctx)
{
    acceptor.async_accept([&](beast::error_code ec, tcp::socket socket) {
        if (!ec) {
            std::make_shared<websocket::stream<beast::ssl_stream<tcp::socket>>>(std::move(socket), ctx)->async_accept([&](beast::error_code ec) {
                if (!ec) {
                    std::cout << "Client connected" << std::endl;
                    // send a message to the client
                    std::string message = "Welcome to the server!";
                    boost::asio::async_write(socket, boost::asio::buffer(message), [](const beast::error_code&, std::size_t){});
                    // handle messages from client here
                }
            });
        }
        do_accept(acceptor, ctx);
    });
}