#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/version.hpp>
#include <boost/beast/ssl.hpp>

namespace asio = boost::asio;
namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace http = beast::http;
using tcp = boost::asio::ip::tcp;

int main()
{
    asio::io_context io_context;
    asio::ssl::context ctx(asio::ssl::context::tlsv12);
    ctx.set_verify_mode(asio::ssl::verify_none);
    tcp::resolver resolver(io_context);
    websocket::stream<beast::ssl_stream<tcp::socket>> ws(io_context, ctx);
    auto const results = resolver.resolve("localhost", "12345");
    asio::connect(ws.next_layer().next_layer(), results.begin(), results.end());
    ws.next_layer().handshake(asio::ssl::stream_base::client);
    ws.handshake("localhost", "/");
    std::string message;
    beast::flat_buffer buffer;
    ws.read(buffer);
    message = beast::buffers_to_string(buffer.data());
    std::cout << "Received message from server: " << message << std::endl;
    return 0;
}