#include "bobwire_circuit.hpp"
#include <chrono>
#include <iostream>
#include <fstream>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>
#include <boost/tokenizer.hpp>

namespace module {
	namespace brazil {
		namespace proc_unit {
			bobwire_circuit::bobwire_circuit()
					: dispatcher(NULL),
					socket(::net::global_zcontext, ZMQ_SUB),
					nIP(0),
					nIP_hbo(0),
					basisByte(0),
					basisBytePos(0) {
			}
			
			bobwire_circuit::~bobwire_circuit() {
				if(dispatcher != NULL) {
					delete dispatcher;
				}
			}
			
			::module::iproc_unit* bobwire_circuit::initialize() {
				return new bobwire_circuit();
			}
			
			void bobwire_circuit::string_initialize_parameters(const char* const parameters) {
				std::string basesLocation;
				std::string requestEndpoint;
				std::string rxDispatcherLocation;
				std::string txDispatcherLocation;
				
				try {
					// Tokenize string
					std::string tparameters(parameters);
					boost::escaped_list_separator<char> seperator("\\", "= ", "\"\'");
					boost::tokenizer<boost::escaped_list_separator<char>> tokens(tparameters, seperator);
					std::vector<std::string> tokenStrings;
					std::copy_if(tokens.begin(), tokens.end(), std::back_inserter(tokenStrings), !boost::bind(&std::string::empty, _1));
					
					namespace po = boost::program_options;
					
					po::options_description desc("Options");
					desc.add_options()
						("b", po::value<std::string>(&basesLocation)->required(), "list of bases")
						("e", po::value<std::string>(&requestEndpoint)->required(), "request endpoint [ip:port]")
						("rd", po::value<std::string>(&rxDispatcherLocation)->required(), "dispatcher rx location")
						("td", po::value<std::string>(&txDispatcherLocation)->required(), "dispatcher tx location");
					
					po::variables_map vm;
					po::store(po::command_line_parser(tokenStrings).options(desc).run(), vm);
					po::notify(vm);
				} catch(boost::program_options::required_option &e) {
					std::cerr << e.what() << std::endl;
					exit(-1);
				} catch(boost::program_options::error &e) {
					std::cerr << e.what() << std::endl;
					exit(-1);
				}
				
				basesFile.open(basesLocation, std::ios::binary);
				if(!basesFile.is_open()) {
					std::string msg = "Could not open bases file: ";
					msg += basesLocation;
					throw std::invalid_argument(msg);
				}
				
				auto requestEPDelim = requestEndpoint.find(':');
				if(UNLIKELY(requestEPDelim == std::string::npos)) {
					throw std::invalid_argument(err_msg::_malinpt);
				}
				
				std::string requestAddress = requestEndpoint.substr(0, requestEPDelim);
				if(UNLIKELY(requestAddress.size() == 0)) {
					throw std::invalid_argument(err_msg::_malinpt);
				}
				
				std::string requestPort = requestEndpoint.substr(requestEPDelim+1);
				if(UNLIKELY(requestPort.size() == 0)) {
					throw std::invalid_argument(err_msg::_malinpt);
				}
				
				set_request_endpoint(requestAddress.c_str(),
						std::stoul(requestPort, nullptr, 10));
				
				// Because we only modify the request endpoint here, we cache the IP
				// within the class to avoid locking a mutex each time we need to get the
				// value. Network byte order.
				nIP = get_request_address();
				nIP_hbo = NTH_BYTE_ORD(nIP);
				
				dispatcher = new ::net::simulation::client(txDispatcherLocation.c_str());
				
				// Start to listen for measurements
				socket.setsockopt(ZMQ_SUBSCRIBE, (char*)&nIP_hbo, sizeof(nIP_hbo));
				static int rx_receive_timeout = 20;
				socket.setsockopt(ZMQ_RCVTIMEO, &rx_receive_timeout, sizeof(rx_receive_timeout));
				socket.connect(rxDispatcherLocation);
				
				start_request_listening();
			}
			
			bool bobwire_circuit::transmit(const unsigned long ip,
					const unsigned short port,
					const char* const buf,
					const std::size_t len) {
				
				for(std::size_t i = 0; i < len; i++) {
					// Create circuit
					std::string circuit("init 2\nh 0\nc 0,1");
					
					switch(buf[i]) {
						case 0:
						case '0':
							circuit += "\ni 0";
							break;
						case 1:
						case '1':
							circuit += "\nx 0";
							break;
						case 2:
						case '2':
							circuit += "\nz 0";
							break;
						case 3:
						case '3':
							circuit += "\ny 0";
							break;
					}
					
					auto connection = open_connection(ip, port);
					
					// Send circuit to dispatcher
					::net::simulation::request request("tx", false);
					request.add<unsigned int>(nIP)
							.add<const char*, false>("chexp")
							.add<const char*, false>(circuit.c_str())
							.add<const char*, false>("\n");
					::net::simulation::response response(dispatcher->call(request));
					
					close_connection(std::move(connection));
					
					if(response.get_error()) {
						
						return false;
					}
				}
				
				return true;
			}
			
			void bobwire_circuit::async_work(::buffer::queue_buffer& out) {
					::zmq::message_t msg;
					if(socket.recv(&msg)) {
						ulock_t uLock(requestMutex);
						
						if(!isReceiving) {
							// Should never happen
							throw std::runtime_error(err_msg::_unrchcd);
						}
						
						// Receive our actual data
						socket.recv(&msg);
						
						out.push(
								::buffer::buffer_item(
									(char*)msg.data(),
									msg.size(),
									0,
									0,
									false)
								);
						
						hasReceived = true;
						// Unlocking before notifying prevents blocking in other thread
						uLock.unlock();
						hasReceivedCV.notify_all();
					}
			}
			
			::module::iproc_unit::response* bobwire_circuit::proc_act_request(
						const ::module::iproc_unit::request& request) {
				const auto method = request.method();
				
				if(strcmp(method, "configure_detector") == 0) {
					throw std::logic_error("unimplemented");
					
					
					/** \todo: return a value here */
					
				} else {
					throw std::logic_error(err_msg::_unrchcd);
				}
			}
		}
	}
}
