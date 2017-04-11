#ifndef _MODULE_BRAZIL_ITRX_PROC_UNIT_HPP
#define _MODULE_BRAZIL_ITRX_PROC_UNIT_HPP

#include <common.hpp>
#include <module/iproc_unit.hpp>
#include <net/server.hpp>
#include <net/tcp_client.hpp>
#include <arpa/inet.h>
#include <atomic>
#include <condition_variable>
#include <thread>
#include <boost/asio.hpp>

namespace module {
	namespace brazil {
		/**
		 * \brief Interface for specialized processing unit for brazil transceivers.
		 */
		template <typename T>
				class itrx_proc_unit : public ::module::iproc_unit {
		 private:
			/**
			 * \brief Alias declaration type of communication request server.
			 */
			using requestServer_t = ::net::server<itrx_proc_unit,
					::net::request,
					::net::response>;
		
		 protected:
			/**
			 * \brief Alias declaration type of the mutex lock type for the class.
			 */
			using lock_t = std::lock_guard<std::mutex>;
			
			/**
			 * \brief Alias declaration type of the unlockable mutex lock type for the
			 * class.
			 */
			using ulock_t = std::unique_lock<std::mutex>;
		
		 public:
			/**
			 * \brief Alias declaration type of the request object supplied to processing
			 * functions.
			 */
			using request = ::module::iproc_unit::request;
			
			/**
			 * \brief Alias declaration type of the response object created by processing
			 * functions.
			 */
			using response = ::module::iproc_unit::response;
			
			/**
			 * \brief Alias declaration type of the communication request client.
			 */
			using crqst_clnt_t = ::net::tcp_client<::net::response, ::net::request, true>;
			
			/**
			 * \brief Constructor.
			 */
			itrx_proc_unit()
					: isReceiving(false),
					hasReceived(false) {
			}
			
			/**
			 * \brief Copy constructor.
			 */
			itrx_proc_unit(const itrx_proc_unit&) = default;
			
			/**
			 * \brief Move constructor.
			 */
			itrx_proc_unit(itrx_proc_unit&&) = default;
			
			/**
			 * \brief Assignment operator.
			 */
			itrx_proc_unit& operator=(const itrx_proc_unit&) = default; 
			
			/**
			 * \brief Move assignment operator.
			 */
			itrx_proc_unit& operator=(itrx_proc_unit&&) = default;
			
			/**
			 * \brief Virtual destructor.
			 */
			virtual ~itrx_proc_unit() {
				ioService.stop();
				stop_request_listening();
				requestThread.join();
			}
			
			/**
			 * \brief Initialize a new instance of the processing unit.
			 * 
			 * \warning We must override this in an implementation.
			 */
			static itrx_proc_unit* initialize() {
				throw std::runtime_error(err_msg::_stcimpl);
			}
			
			/**
			 * \brief Process a request action.
			 * 
			 * \note Threadsafe.
			 */
			response* proc_act_request(const request& request) {
				// Push to child
				return static_cast<T*>(this)->proc_act_request(request);
			}
			
			/**
			 * \brief Process a push action.
			 * 
			 * \note Threadsafe.
			 */
			bool proc_act_push(const request& request) {
				const auto method = request.method();
				
				if(strcmp(method, "tx") == 0) {
					auto rIP = request.parameter<unsigned int>(0);
					auto rPort = request.parameter<unsigned short>(1);
					auto rData = request.parameter<const char*>(2);
					auto rDataLen = request.parameter_str_size(2);

					return transmit(rIP, rPort, rData, rDataLen);
				} else {
					throw std::runtime_error(err_msg::_malinpt);
				}
			}
		
		 protected:
			/**
			 * \brief \todo
			 */
			inline void start_request_listening() {
				requestThread = std::thread(&itrx_proc_unit::work, this);
			}
			
			/**
			 * \brief \todo
			 */
			inline void stop_request_listening() {
				ioService.stop();
			}
			
			/**
			 * \brief Set the address and port we use to accept communication requests on.
			 * 
			 * \note Threadsafe.
			 */
			inline void set_request_endpoint(const char* const address,
					const unsigned short port) {
				lock_t lock(requestMutex);
				
				if(inet_aton(address, &netInfo.sin_addr) == 0) {
					throw std::invalid_argument(err_msg::_malinpt);
				}
				
				netInfo.sin_port = HTN_BYTE_ORD(port);
			}
			
			/**
			 * \brief Get the address we accept communication requests on.
			 * 
			 * \note Threadsafe.
			 */
			inline unsigned int get_request_address() {
				lock_t lock(requestMutex);
				
				return netInfo.sin_addr.s_addr;
			}
			
			/**
			 * \brief Get the port we accept communication requests on.
			 * 
			 * \note Threadsafe.
			 */
			inline unsigned short get_request_port() {
				lock_t lock(requestMutex);
				
				return netInfo.sin_port;
			}
			
			/**
			 * \brief Transmit a data buffer and return whether the transmission was
			 * successful.
			 * 
			 * \note The implementation of this must be threadsafe.
			 */
			virtual bool transmit(const unsigned long ip,
					const unsigned short port,
					const char* const buf,
					const std::size_t len) = 0;
			
			/**
			 * \brief \todo
			 */
			inline crqst_clnt_t open_connection(const unsigned long ip,
					const unsigned short port) {
				/** \todo: find a better method of thread safety */
				//std::unique_lock<std::mutex> rLock(requestMutex); 
				
				crqst_clnt_t connection(ioService, ip, port);
				
				//rLock.lock();
				auto request = ::net::request(0,
						1,
						0);
				connection.write(std::move(request));
				//rLock.unlock();
				
				auto response = connection.read();
				
				UNUSED(response);
				
				/** \todo: throw exception if response was error */
				return connection;
			}
			
			/**
			 * \brief \todo
			 */
			inline void close_connection(crqst_clnt_t&& connection) {
				auto request = ::net::request(0,
						2,
						0);
				connection.write(std::move(request));
				connection.read();
			}
			
			
			/** \todo: make these private. */
			
			/**
			 * \brief Mutex protector of the receiving state of the transceiver.
			 */
			std::mutex requestMutex;
			
			/**
			 * \brief Whether or not we are currently receiving.
			 * 
			 * \warning Use the requestMutex when getting/setting this value.
			 */
			bool isReceiving;
			
			/**
			 * \brief Whether or not we have received our expected rx packet from the
			 * simulator yet.
			 * 
			 * \warning Use the requestMutex when getting/setting this value.
			 */
			bool hasReceived;
			
			/**
			 * \brief Condition variable to signal that hasReceived has been set.
			 * 
			 * \warning Use the requestMutex with this.
			 */
			std::condition_variable hasReceivedCV;
		
		// private:
			/**
			 * \brief The network information for accepting communication requests.
			 */
			sockaddr_in netInfo;
			
			/**
			 * \brief IO service for the receive server and client.
			 */
			boost::asio::io_service ioService;
			
			/**
			 * \brief Thread the communication request server runs on.
			 */
			std::thread requestThread;
			
			/**
			 * \brief Function launched by requestThread that starts our server listening
			 * for communication requests.
			 * 
			 * \warning Do not call directly.
			 * 
			 * \note \todo
			 */
			void work() {
				// Protect until we have launched
				std::unique_lock<std::mutex> uLock(requestMutex);
				requestServer_t requestServer(
						ioService,
						NTH_BYTE_ORD(netInfo.sin_addr.s_addr),
						NTH_BYTE_ORD(netInfo.sin_port),
						*this,
						&itrx_proc_unit::process);
				
				// Unlock while we block below
				uLock.unlock();
				
				// Block until ioService.stop() is called
				boost::asio::io_service::work work(ioService);
				ioService.run();
			}
			
			/**
			 * \brief Callback to process an incoming request for quantum communication.
			 */
			void process(::net::request& incomingMessage,
					::net::response& outgoingMessage) {
				UNUSED(incomingMessage);
				UNUSED(outgoingMessage);
				
				ulock_t  uLock(requestMutex);
				
				if(isReceiving) {
					// Wait for the simulated result
					hasReceivedCV.wait(uLock, [this] { return hasReceived == true; });
					
					// uLock still locked
					hasReceived = false;
					
					// todo: send proper outgoing
				} else {
					// todo: send proper outgoing
				}
				isReceiving = !isReceiving;
			}
		};
	}
}

#endif
