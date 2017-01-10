#ifndef _NET_TCP_CLIENT_HPP
#define _NET_TCP_CLIENT_HPP

#include <common.hpp>
#include "response.hpp"
#include "request.hpp"
#include <algorithm>
#include <boost/asio.hpp>

namespace net {
	/**
	 * \brief A synchronous TCP client.
	 * 
	 * Calls to read() and write() are blocking.
	 * 
	 * \warning Do not use this class directly. This is shared code for the tcp_client
	 * class that has a version that is threadsafe and one that isn't.
	 */
	template <typename imsg_t, typename omsg_t>
			class _tcp_client_base {
	 public:
		/**
		 * \brief Alias type declaration for an IPv6 address.
		 */
		using ipv6_t = boost::asio::ip::address_v6::bytes_type;
		
		/**
		 * \brief Copy constructor is disabled.
		 */
		_tcp_client_base(_tcp_client_base&) = delete;
		
		/**
		 * \brief Move constructor.
		 */
		_tcp_client_base(_tcp_client_base&& old)
				: socket(std::move(old.socket)) {
		}
		
		/**
		 * \brief Assignment operator is disabled.
		 */
		_tcp_client_base& operator=(const _tcp_client_base&) = delete;
		
		/**
		 * \brief Move assignment operator.
		 */
		_tcp_client_base& operator=(_tcp_client_base&& old) {
			socket = std::move(old.socket);
			
			return *this;
		}
		
		/**
		 * \brief Destructor.
		 * 
		 * Cleanly disconnect and close socket.
		 */
		virtual ~_tcp_client_base() {
			socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
			socket.close();
		}
		
		/**
		 * \brief Return the remote IPv4 we are connected to.
		 * 
		 * Returns address in host byte order.
		 * 
		 * \note Not threadsafe.
		 * 
		 * \throws Throws std::logic_error if the underlying address is IPv6 and we map to
		 * an IPv4 version.
		 */
		inline std::uint_fast64_t ip4() {
			auto address = socket.remote_endpoint().address();
			
			if(address.is_v6() && !address.to_v6().is_v4_compatible()) {
				throw std::logic_error(err_msg::_malinpt);
			}
			
			return address.to_v4().to_ulong();
		}
		
		/**
		 * \brief Return the remote IPv6 we are connected to.
		 * 
		 * Returns address in host byte order.
		 * 
		 * \note Not threadsafe.
		 */
		inline ipv6_t ipv6() {
			auto address = socket.remote_endpoint().address().to_v6().to_bytes();
			
			// Boost returns in network byte order for some reason (even though for IPv4
			// it is host byte order), so we make things uniform and flip it
			std::reverse(address.begin(), address.end());
			
			return address;
		}
		
		/**
		 * \brief Return the remote port we are connect to.
		 * 
		 * \note Not threadsafe.
		 */
		inline std::uint_fast16_t port() {
			return socket.remote_endpoint().port();
		}
		
		/**
		 * \brief Read incoming data on a TCP socket into a message.
		 * 
		 * Block until the number of bytes within a message are received.
		 * 
		 * \note Not threadsafe.
		 */
		imsg_t read() {
			boost::system::error_code ec;
			imsg_t message;
			std::size_t size = message.length();
			
			if(imsg_t::header_length() != 0) {
				// Receive a header
				size = 0;
				
				static_assert(sizeof(size) > imsg_t::header_length(),
						"Size not large enough to hold header");
				
				boost::asio::read(socket,
					boost::asio::buffer((char*)&size, imsg_t::header_length()),
					boost::asio::transfer_all(),
					ec);
				
				// In the future, we can use this to implement more robust error handling
				// on transient network problems. For now we map boost errors to std
				// errors.
				if(ec.value() != boost::system::errc::success) {
					throw std::runtime_error(ec.message().c_str());
				}
				
				message.allocate(size);
			}
			
			boost::asio::read(socket,
				boost::asio::buffer(message.data(), size),
				boost::asio::transfer_all(),
				ec);
			
			// In the future, we can use this to implement more robust error handling on
			// transient network problems. For now we map boost errors to std errors.
			if(ec.value() != boost::system::errc::success) {
				throw std::runtime_error(ec.message().c_str());
			}
			
			return message;
		}
		
		/**
		 * \brief Write the contents of a message to a TCP socket.
		 * 
		 * Block until the number of bytes within a message are transmitted.
		 * 
		 * \note Not threadsafe.
		 */
		std::size_t write(omsg_t&& message) {
			boost::system::error_code ec;
			
			const std::size_t val = boost::asio::write(socket,
					boost::asio::buffer(message.data(), message.length()),
					boost::asio::transfer_all(),
					ec);
			
			// In the future, we can use this to implement more robust error handling on
			// transient network problems. For now we map boost errors to std errors.
			if(ec.value() != boost::system::errc::success) {
				throw std::runtime_error(ec.message().c_str());
			}
			
			return val;
		}
	
	 protected:
		/**
		 * \brief Constructor for IPv4 addresses.
		 */
		_tcp_client_base(boost::asio::io_service& ioService,
				const std::uint_fast64_t address,
				const std::uint_fast16_t port)
				: socket(ioService) {
			socket.connect(boost::asio::ip::tcp::endpoint(
						boost::asio::ip::address_v4(address),
						port));
		}
		
		/**
		 * \brief Constructor for IPv6 addresses.
		 * 
		 * \warning Address must be in host byte order
		 */
		_tcp_client_base(boost::asio::io_service& ioService,
				const ipv6_t& address,
				const std::uint_fast16_t port)
				: socket(ioService) {
			auto netBytesAddr(address);
			std::reverse(netBytesAddr.begin(), netBytesAddr.end());
			
			socket.connect(boost::asio::ip::tcp::endpoint(
						boost::asio::ip::address_v6(netBytesAddr),
						port));
		}
	
	 private:
	 	/**
		 * \brief The socket we are communicating with.
		 */
		boost::asio::ip::tcp::socket socket;
	};
	
	/**
	 * \brief A synchronous TCP client.
	 * 
	 * \note Not threadsafe. See the specialization of this class on isThreadSafe = true
	 * for the threadsafe version.
	 */
	template <typename imsg_t, typename omsg_t, bool isThreadSafe = true>
			class tcp_client : public _tcp_client_base<imsg_t, omsg_t> {
	 private:
		/**
		 * \brief Alias declaration type of the base class we inherit form.
		 */
		using base_t = _tcp_client_base<imsg_t, omsg_t>;
	
	 public:
		/**
		 * \brief Alias type declaration for an IPv6 address.
		 */
		using ipv6_t = typename base_t::ipv6_t;
		
		/**
		 * \brief Constructor for IPv4 addresses.
		 */
		tcp_client(boost::asio::io_service& ioService,
				const std::uint_fast64_t address,
				const std::uint_fast16_t port)
				: base_t(ioService,
					address,
					port) {
		}
		
		/**
		 * \brief Constructor for IPv6 addresses.
		 * 
		 * \warning Address must be in host byte order
		 */
		tcp_client(boost::asio::io_service& ioService,
				const ipv6_t& address,
				const std::uint_fast16_t port)
				: base_t(ioService,
				address,
				port) {
		}
		
		/**
		 * \brief Constructor.
		 */
		tcp_client(boost::asio::io_service& ioService,
				const char* const address,
				const std::uint_fast16_t port)
				:  base_t(ioService,
					address,
					port) {
		}
		
		/**
		 * \brief Copy constructor is disabled.
		 */
		tcp_client(tcp_client&) = delete;
		
		/**
		 * \brief Move constructor.
		 */
		tcp_client(tcp_client&& old) = default;
		
		/**
		 * \brief Assignment operator is disabled.
		 */
		tcp_client& operator=(const tcp_client&) = delete;
		
		/**
		 * \brief Move assignment operator.
		 */
		tcp_client& operator=(tcp_client&& old) = default;
		
		/**
		 * \brief Destructor.
		 */
		~tcp_client() = default;
	};
	
	/**
	 * \brief A synchronous TCP client.
	 * 
	 * \note Threadsafe. See the specialization of this class on isThreadSafe = false for
	 * the version that is not threadsafe.
	 */
	template <typename imsg_t, typename omsg_t>
			class tcp_client<imsg_t, omsg_t, true> :
				public _tcp_client_base<imsg_t, omsg_t> {
	 private:
		/**
		 * \brief Alias declaration type of the base class we inherit form.
		 */
		using base_t = _tcp_client_base<imsg_t, omsg_t>;
		
	 	/**
		 * \brief Alias declaration type of the mutex lock type for the class.
		 */
		using lock_t = std::lock_guard<std::mutex>;
	
	 public:
		/**
		 * \brief Alias type declaration for an IPv6 address.
		 */
		using ipv6_t = typename base_t::ipv6_t;
		
		/**
		 * \brief Constructor for IPv4 addresses.
		 */
		tcp_client(boost::asio::io_service& ioService,
				const std::uint_fast64_t address,
				const std::uint_fast16_t port)
				: base_t(ioService,
					address,
					port) {
			socketMutex = new std::mutex;
		}
		
		/**
		 * \brief Constructor for IPv6 addresses.
		 * 
		 * \warning Address must be in host byte order
		 */
		tcp_client(boost::asio::io_service& ioService,
				const ipv6_t& address,
				const std::uint_fast16_t port)
				: base_t(ioService,
				address,
				port) {
			socketMutex = new std::mutex;
		}
		
		/**
		 * \brief Constructor.
		 * 
		 * This can be used for both IPv4 and IPv6 addresses, as the address parameter is
		 * a string.
		 */
		tcp_client(boost::asio::io_service& ioService,
				const char* const address,
				const std::uint_fast16_t port)
				: base_t(ioService,
					address,
					port) {
			socketMutex = new std::mutex;
		}
		
		/**
		 * \brief Copy constructor is disabled.
		 */
		tcp_client(tcp_client&) = delete;
		
		/**
		 * \brief Move constructor.
		 */
		tcp_client(tcp_client&& old)
				: base_t(std::move(old)) {
			socketMutex = old.socketMutex;
			old.socketMutex = NULL;
		}
		
		/**
		 * \brief Assignment operator is disabled.
		 */
		tcp_client& operator=(const tcp_client&) = delete;
		
		/**
		 * \brief Move assignment operator.
		 */
		tcp_client& operator=(tcp_client&& old) {
			base_t::operator=(std::move(old));
			socketMutex = old.socketMutex;
			old.stateMutex = NULL;
			
			return *this;
		}
		
		/**
		 * \brief Destructor.
		 * 
		 * Socket is cleaned up in the base class.
		 */
		~tcp_client() {
			if(socketMutex != NULL) {
				delete socketMutex;
			}
		}
	 
		/**
		 * \brief Return the remote IPv4 we are connected to.
		 * 
		 * \note Threadsafe.
		 */
		inline std::uint_fast64_t ip4() {
			lock_t lock(*socketMutex);
			
			return base().ip4();
		}
		
		/**
		 * \brief Return the remote IPv6 we are connected to.
		 * 
		 * \note Threadsafe.
		 */
		inline std::uint_fast64_t ip6() {
			lock_t lock(*socketMutex);
			
			return base().ip6();
		}
		
		/**
		 * \brief Return the remote port we are connected to.
		 * 
		 * \note Threadsafe.
		 */
		inline std::uint_fast16_t port() {
			lock_t lock(*socketMutex);
			
			return base().port();
		}
		
		/**
		 * \brief Read incoming data on a TCP socket into a message.
		 * 
		 * Block until the number of bytes within a message are received.
		 * 
		 * \note Threadsafe.
		 */
		inline imsg_t read() {
			lock_t lock(*socketMutex);
			
			return base().read();
		}
		
		/**
		 * \brief Write the contents of a message to a TCP socket.
		 * 
		 * Block until the number of bytes within a message are transmitted.
		 * 
		 * \note Threadsafe.
		 */
		inline std::size_t write(omsg_t&& message) {
			lock_t lock(*socketMutex);
			
			return base().write(std::move(message));
		}
	
	 private:
		/**
		 * \brief The mutex protector of the socket object.
		 * 
		 * The mutex object does not support moving and because we still want maximum
		 * compatibility with the non-threadsafe version, we have to put this in the heap.
		 */
		std::mutex* socketMutex;
		
		/**
		 * \brief Helper function to return a reference to the parent class.
		 * 
		 * \note Not threadsafe.
		 */
		inline base_t& base() {
			return *static_cast<base_t*>(this);
		}
	};
}

#endif
