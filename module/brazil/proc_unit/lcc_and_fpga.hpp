#ifndef _MODULE_BRAZIL_PROC_UNIT_LCC_AND_FPGA_HPP
#define _MODULE_BRAZIL_PROC_UNIT_LCC_AND_FPGA_HPP

#include "../../iproc_unit.hpp"
#include "../../../net/server.hpp"
#include "../../../net/tcp_client.hpp"
#include <algorithm>
#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>

namespace module {
	namespace brazil {
		namespace proc_unit {
			/**
			 * \brief 
			 */
			class lcc_and_fpga : public ::module::iproc_unit {
			 public:
				/**
				 * \brief Constructor.
				 */
				lcc_and_fpga();
				
				/**
				 * \brief Destructor.
				 */
				~lcc_and_fpga();
				
				/**
				 * \brief Return a new instance of the class.
				 */
				static ::module::iproc_unit* initialize();
				
				/**
				 * \brief Initialize the transceiver with a parameter string.
				 */
				void string_initialize_parameters(const char* const parameters);
				
				/**
				 * \brief This processing unit does not currently have any async actions
				 * that can be processed.
				 * 
				 * \note Threadsafe.
				 */
				void async_work(::buffer::queue_buffer& out);
				
				/**
				 * \brief Process a request action.
				 * 
				 * \note Threadsafe.
				 */
				::module::iproc_unit::response* proc_act_request(
						const ::module::iproc_unit::request& request);
				
				/**
				 * \brief Process a push action.
				 * 
				 * \note Threadsafe.
				 */
				bool proc_act_push(const ::module::iproc_unit::request& request);
			
			 private:
				/**
				 * \brief A serial connection on a particular port.
				 */
				struct serial_connection {
				 public:
					/**
					 * \brief Constructor takes a serial port address and baud speed.
					 * 
					 * The serial port address would be something along the lines of
					 * "/dev/ttyS0" on POSIX or "COM1" on Windows.
					 */
					serial_connection(const char* const address,
							const std::uint_fast64_t baud)
							: _io(), _serial(_io, address) {
						_serial.set_option(boost::asio::serial_port_base::baud_rate(baud));
					}
					
					/**
					 * \brief Write a string onto the serial port.
					 */
					void write(const char* const str) {
						boost::asio::write(_serial, boost::asio::buffer(str, strlen(str)));
					}
					
					/**
					 * \brief Read a string from the serial port.
					 */
					void dump() {
						char c = '0';
						std::string result;
						boost::system::error_code ec;
						do {
							boost::asio::read(_serial, boost::asio::buffer(&c, 1), ec);
						} while (c != '\r' && ec != boost::asio::error::eof);
						// LCC gives us space
						if(ec != boost::asio::error::eof) {
							boost::asio::read(_serial, boost::asio::buffer(&c, 1));
						}
					}
				
				 private:
					boost::asio::io_service _io;
					boost::asio::serial_port _serial;
				};
				
				/**
				 * \brief A request message to the hardware.
				 */
				struct request {
				 public:
					/**
					 * \brief Empty constructor.
					 */
					request()
							: _data{0} {
					}
					
					/**
					 * \brief Initialization constructor.
					 */
					request(const bool doCorrelate) : request() {
						_data[0] = doCorrelate;
					}
					
					/**
					 * \brief Copy constructor is disabled.
					 */
					request(request&) = delete;
					
					/**
					 * \brief Move constructor.
					 */
					request(request&& old) {
						memcpy(_data, old._data, sizeof(_data));
					}
					
					/**
					 * \brief Assignment operator.
					 */
					request& operator=(const request&) = delete;
					
					/**
					 * \brief Move assignment operator.
					 */
					request& operator=(request&& old) {
						memcpy(_data, old._data, sizeof(_data));
						
						return *this;
					}
					
					/**
					 * \brief Access data, length given by length.
					 */
					inline const char* data() {
						return _data;
					}
					
					/**
					 * \brief Return the number of bytes in header.
					 */
					inline std::size_t header_length() const {
						return _header_length;
					}
					
					/**
					 * \brief Return the number of bytes of data.
					 */
					inline std::size_t length() const {
						return _length;
					}
					
				 private:
					/**
					 * \brief Header length.
					 */
					static const std::size_t _header_length = 0;
					
					/**
					 * \brief Data length.
					 */
					static const std::size_t _length = 1;
					
					/**
					 * \brief Data.
					 * 
					 * Static length given by length.
					 */
					char _data[_length];
				};
				
				/**
				 * \brief A response message.
				 */
				struct response {
				 public:
					/**
					 * \brief Empty constructor.
					 */
					response() 
							: _length(0),
							_data(0) {
					}
					
					/**
					 * \brief Copy constructor is disabled.
					 */
					response(request&) = delete;
					
					/**
					 * \brief Move constructor.
					 */
					response(response&& old)
							: _length(old._length),
							_data(old._data) {
						old._data = 0;
					}
					
					/**
					 * \brief Assignment operator is disabled.
					 */
					response& operator=(const response&) = delete;
					
					/**
					 * \brief Move assignment operator.
					 */
					response& operator=(response&& old) {
						_data = old._data;
						old._data = 0;
						_length = old._length;
						
						return *this;
					}
					
					~response() {
						if(_data != 0) {
							delete[] _data;
						}
					}
					
					/**
					 * \brief Access data, length given by length.
					 */
					inline char* data() {
						return _data;
					}
					
					/**
					 * \brief Set the length of the data.
					 */
					inline void allocate(const std::size_t length) {
						if(_data != 0) {
							delete[] _data;
						}
						_length = length;
						_data = new char[_length];
					}
					
					/**
					 * \brief Return the number of bytes in header.
					 */
					constexpr static std::size_t header_length() {
						return _header_length;
					}
					
					/**
					 * \brief Return the number of bytes of data.
					 */
					inline std::size_t length() const {
						return _length;
					}
					
				 private:
					/**
					 * \brief Number of bytes in data.
					 */
					static const std::size_t _header_length = 4;
					
					/**
					 * \brief Data length.
					 */
					std::size_t _length;
					
					/**
					 * \brief Data.
					 */
					char* _data;
				};
				
				/**
				 * \brief LCC 0.
				 */
				serial_connection* controller0;
				
				/**
				 * \brief LCC 1.
				 */
				serial_connection* controller1;
				
				/**
				 * \brief Connection used for classical communicaton requests and for the
				 * hardware receiving client.
				 */
				boost::asio::io_service ioService;
				
				::net::tcp_client<response, request>* hardwareConnection;
				
				/**
				 * \brief Whether or not we are Rx-ing
				 */
				bool isRx;
				
				/**
				 * \brief Whether or not we are currently receiving from the detection
				 * board.
				 */
				std::atomic_bool isReceiving;
				
				std::mutex _bufferMutex;
				
				std::vector<unsigned char> _buffer;
				
				/**
				 * \brief Our numerical IP address.
				 */
				std::uint_fast32_t nIp;
				
				/**
				 * \brief Port used for quantum communication requests.
				 */
				std::uint_fast16_t qPort;
				
				/**
				 * \brief Thread the communication request server runs on.
				 */
				std::thread* thread;
				
				/**
				 * \brief Function to launch server listening for communication requests.
				 */
				void work(boost::asio::io_service& ioService,
						const char* const address,
						const std::uint_fast16_t port);
				
				/**
				 * \brief Process an incoming request for quantum communication.
				 */
				void process(::net::request& incomingMessage,
						::net::response& outgoingMessage);
			};
		}
	}
}

#endif
