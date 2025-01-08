#include <asio/high_resolution_timer.hpp>
#include <asio/this_coro.hpp>
#include <chrono>
#include <stdio.h>
#include <asio.hpp>
#include "../protocol.h"
#include <map>

using udp = asio::ip::udp;
using Clock = std::chrono::high_resolution_clock;
using Timer = asio::high_resolution_timer;


constexpr Clock::duration updateInterval = std::chrono::milliseconds(10);


// Map ip to id
std::map<asio::ip::address, Player> players;


void handleMessage(char buf[], size_t n) {
}

asio::awaitable<void> sendUpdates(udp::socket& socket, asio::ip::address addr) {
	auto executor = co_await asio::this_coro::executor;

	Timer timer{executor};
	for(;;) {
		timer.expires_after(updateInterval);
		co_await timer.async_wait(asio::use_awaitable);
     		char buf[] = "fucking hell";
		co_await socket.async_send_to(
			asio::buffer(buf, sizeof buf),
			udp::endpoint{addr, 6969},
			asio::use_awaitable);
	}
}

asio::awaitable<void> listen(udp::socket& socket) {
	auto executor = co_await asio::this_coro::executor;
	char buf[2048];
	for (;;) {
      		udp::endpoint peer;
		size_t n = co_await socket.async_receive_from(
      				asio::buffer(buf, sizeof buf),
      				peer,
      				asio::use_awaitable);

		const auto addr = peer.address();
		if (players.contains(addr)) {
			handleMessage(buf, n);
		} else {
			players[addr] = Player{};
			asio::co_spawn(executor, sendUpdates(socket, addr), asio::detached);
		}
	}
}

int main() {

	asio::io_context ioc;
	udp::socket socket{ioc, udp::endpoint{udp::v4(), 6969}};
	asio::co_spawn(ioc, listen(socket), asio::detached);
	ioc.run();
}
