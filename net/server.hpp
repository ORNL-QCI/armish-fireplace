#ifndef _NET_SERVER_HPP
#define _NET_SERVER_HPP

#include <common.hpp>
#include "request.hpp"
#include "response.hpp"
#include <boost/asio.hpp>

#define NET_SERVER_LINGER_TIME 30 // Socket linger time

namespace net {
	/**
	 * \brief Asynchronous TCP server to process a request, send a reply, and repeat until
	 * disconnect.
	 * 
	 * The template parameter T is the class type to be used for the member function
	 * pointer. This function pointer is evaluated to process an incoming request.
	 * 
	 * The template parameter U is the incoming message struct.
	 * The template parameter V is the outgoing message struct.
	 */
	template <typename T, typename U, typename V> class server {
	 private:
		typedef void (T::*process_callback)(request&, response&);
		
	 	/**
		 * \brief A session between the server and a client.
		 */
		struct session {
			friend server;
		
		 public:
			/**
			 * \brief Constructor.
			 */
			session(boost::asio::io_service& ioService,
					T& instance,
					process_callback processCallback)
					: _socket(boost::asio::ip::tcp::socket(ioService)),
					_instance(instance),
					_processCallback(processCallback) {
			}
			
			/**
			 * \brief Copy constructor disabled.
			 * 
			 * Our socket member variable does not support the copy constructor operator.
			 */
			session(session&) = delete;
			
			/**
			 * \brief Move constructor.
			 */
			session(session&& old)
					: _socket(std::move(old._socket)),
					_instance(old._instance),
					_processCallback(old._processCallback) {
			}
			
			/**
			 * \brief Assignment operator disabled.
			 * 
			 * Our socket member variable does not support the assignment operator.
			 */
			session& operator=(const session&) = delete;
			
			/**
			 * \brief Move assignment operator.
			 */
			session& operator=(session&& old) {
				_socket = std::move(old._socket);
				_instance = old._instance;
				_processCallback = old._processCallback;
				
				return *this;
			}
			
			/**
			 * \brief Destructor.
			 */
			~session() {
			}
			
			/**
			 * \brief Listen for a request, process the request, send the reply, and repeat
			 * until an exception occurs (such as disconnect).
			 */
			void run() {
				boost::asio::socket_base::linger linger(true, NET_SERVER_LINGER_TIME);
				_socket.set_option(linger);
				
				boost::system::error_code ec;
				
				do {
					U incoming;
					
					// Read
					if(incoming.header_length() != 0) {
						/** \todo */
						
					} else {
						boost::asio::read(_socket,
							boost::asio::buffer(incoming.data(), incoming.length()),
							boost::asio::transfer_all(),
							ec);
					}
					
					if(ec) {
						break;
					}
					
					V outgoing;
					
					// Process
					(_instance.*_processCallback)(incoming, outgoing);
					
					// Write
					boost::asio::write(_socket,
						boost::asio::buffer(outgoing.data(), outgoing.length()),
						boost::asio::transfer_all(),
						ec);
					
					if(ec) {
						break;
					}
				} while(true);
				
				// Delete this instance and clean up  memory allocated for it
				delete this;
			}
		
		 protected:
			/**
			 * \brief Return the socket object.
			 */
			boost::asio::ip::tcp::socket& socket() {
				return _socket;
			}
		
		 private:
			/**
			 * \brief Socket for the session.
			 */
			boost::asio::ip::tcp::socket _socket;
			
			/**
			 * \brief Reference to the instance of the calling class that contains the
			 * processing function.
			 */
			T& _instance;
			
			/**
			 * \brief Processing function.
			 */
			process_callback _processCallback;
		};
	  
	 public:
		/**
		 * \brief Constructor.
		 */
		
		server(boost::asio::io_service& ioService,
				const unsigned long address,
				const unsigned short port,
				T& instance,
				process_callback processCallback)
				: _ioService(ioService),
				_acceptor(_ioService, boost::asio::ip::tcp::endpoint(boost::asio::ip::address_v4(address), port)),
				_instance(instance),
				_processCallback(processCallback),
				_lastUnusedSession(0) {
			accept();
		}
		
		/**
		 * \brief Copy constructor disabled.
		 * 
		 * Our acceptor member variable does not support the copy constructor operator.
		 */
		server(server&) = delete;
		
		/**
		 * \brief Move constructor.
		 */
		server(server&& old)
				: _ioService(old._ioService),
				_acceptor(std::move(old._acceptor)),
				_instance(old._instance),
				_processCallback(old._processCallback),
				_lastUnusedSession(old._lastUnusedSession) {
			old._lastUnusedSession = 0;
		}
		
		/**
		 * \brief Assignment operator disabled.
		 * 
		 * Our acceptor member variable does not support the assignment operator.
		 */
		server& operator=(const server&) = delete;
		
		/**
		 * \brief Move assignment operator.
		 */
		server& operator=(server&& old) {
			_ioService = old._ioService;
			_acceptor = std::move(old._acceptor);
			_instance = old._instance;
			_processCallback = old._processCallback;
			_lastUnusedSession = old._lastUnusedSession;
			old._lastUnusedSession = 0;
		}
		
		/**
		 * \brief Destructor.
		 */
		~server() {
			// Cleanup the last session created if it was unused, i.e. before run() was
			// called on the session object
			if(_lastUnusedSession != 0) {
				delete _lastUnusedSession;
			}
		}
	
	 private:
		/**
		 * \brief Reference to the io_service used by the server.
		 */
		boost::asio::io_service& _ioService;
		
		/**
		 * \brief Acceptor used to accept new clients.
		 */
		boost::asio::ip::tcp::acceptor _acceptor;
	 
		/**
		 * \brief Reference to the instance of the calling class that contains the
		 * processing function.
		 */
		T& _instance;
		
		/**
		 * \brief Processing function.
		 */
		process_callback _processCallback;
		
		/**
		 * \brief The last session created that was not used, i.e. that did not have run()
		 * called on it.
		 */
		session* _lastUnusedSession;
		
		/**
		 * \brief Accept a client and create a new session.
		 */
		void accept() {
			// The session object is responsible for deallocating this memory when the
			// client disconnects
			session* clientSession = new session(_ioService, _instance, _processCallback);
			_lastUnusedSession = clientSession;
			
			_acceptor.async_accept(clientSession->socket(),
					[this, clientSession](boost::system::error_code ec) {
						UNUSED(ec);
						_lastUnusedSession = 0;
						clientSession->run();
						accept();
					});
		}
	};
}

#endif
