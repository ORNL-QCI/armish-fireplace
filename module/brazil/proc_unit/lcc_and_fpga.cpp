#include "lcc_and_fpga.hpp"
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/program_options.hpp>
#include <boost/tokenizer.hpp>

namespace module {
	namespace brazil {
		namespace proc_unit {
			lcc_and_fpga::lcc_and_fpga()
					: controller0(0), controller1(0), isReceiving(false) {
			}
			
			lcc_and_fpga::~lcc_and_fpga() {
				if(controller0 != 0) {
					delete controller0;
				}
				if(controller1 != 0) {
					delete controller1;
				}
				
				boost::system::error_code ec;
			}
			
			::module::iproc_unit* lcc_and_fpga::initialize() {
				return new lcc_and_fpga();
			}
			
			void lcc_and_fpga::string_initialize_parameters(const char* const parameters) {
				std::string tempParameters(parameters);
				std::string configurationFile;
				std::vector<std::string> parameterTokenStrings;
				std::string lccDev0;
				std::uint_fast32_t lccSpeed0 = 0;
				std::string lccDev1;
				std::uint_fast32_t lccSpeed1 = 0;
				std::string zedboardIp;
				std::uint_fast16_t zedboardPort = 0;
				std::string handshakeIp;
				nIp = 0;
				qPort = 0;
				std::string ppVoltage1;
				std::string ppVoltage2;
				std::string pspVoltage1;
				std::string pspVoltage2;
				std::string pmVoltage1;
				std::string pmVoltage2;
				std::string psmVoltage1;
				std::string psmVoltage2;
				
				boost::escaped_list_separator<char> seperator("\\", "= ", "\"\'");
				boost::tokenizer<boost::escaped_list_separator<char> > tokens(tempParameters, seperator);
				std::copy_if(tokens.begin(), tokens.end(), std::back_inserter(parameterTokenStrings), !boost::bind(&std::string::empty, _1));
				
				try {
					namespace po = boost::program_options;
					
					po::options_description desc("Options");
					desc.add_options()
						("config", po::value<std::string>(&configurationFile)->required(), "configuration file")
						("lccDev0", po::value<std::string>(&lccDev0), "lcc device 0")
						("lccSpeed0", po::value<std::uint_fast16_t>(&lccSpeed0), "lcc speed 0")
						("lccDev1", po::value<std::string>(&lccDev1), "lcc device 1")
						("lccSpeed1", po::value<std::uint_fast16_t>(&lccSpeed1), "lcc speed 1")
						("zedboardIp", po::value<std::string>(&zedboardIp), "zedboard IP")
						("zedboardPort", po::value<std::uint_fast16_t>(&zedboardPort), "zedboard Port")
						("handshakeIp", po::value<std::string>(&handshakeIp), "handshake IP")
						("handshakePort", po::value<std::uint_fast16_t>(&qPort), "handshake Port")
						("ppVoltage1", po::value<std::string>(&ppVoltage1), "psiplus voltage 1")
						("ppVoltage2", po::value<std::string>(&ppVoltage2), "psiplus voltage 2")
						("pspVoltage1", po::value<std::string>(&pspVoltage1), "phiplus voltage 1")
						("pspVoltage2", po::value<std::string>(&pspVoltage2), "phiplus voltage 2")
						("pmVoltage1", po::value<std::string>(&pmVoltage1), "psiminus voltage 1")
						("pmVoltage2", po::value<std::string>(&pmVoltage2), "psiminus voltage 2")
						("psmVoltage1", po::value<std::string>(&psmVoltage1), "phiminus voltage 1")
						("psmVoltage2", po::value<std::string>(&psmVoltage2), "phiminus voltage 2");
					
					po::variables_map vm;
					po::store(po::command_line_parser(parameterTokenStrings).options(desc).run(), vm);
					po::notify(vm);
					po::store(po::parse_config_file<char>(configurationFile.c_str(), desc), vm);
					po::notify(vm);
				} catch(boost::program_options::required_option &e) {
					std::cerr << e.what() << std::endl;
					exit(-1);
				} catch(boost::program_options::error &e) {
					std::cerr << e.what() << std::endl;
					exit(-1);
				}
				
				if(lccDev0.empty() != lccDev1.empty()) {
					throw std::invalid_argument("Missing lc controller");
				} else if(!lccDev0.empty()) {
					if(lccSpeed0 == 0 || lccSpeed1 == 0) {
						throw std::invalid_argument("Missing lc controller speed");
					} else {
						controller0 = new serial_connection(lccDev0.c_str(), lccSpeed0);
						controller1 = new serial_connection(lccDev1.c_str(), lccSpeed1);
						
						if(ppVoltage1.empty() || ppVoltage2.empty() ||
								pspVoltage1.empty() || pspVoltage2.empty() ||
								pmVoltage1.empty() || pmVoltage2.empty() ||
								psmVoltage1.empty() || psmVoltage2.empty()) {
							throw std::invalid_argument("Missing voltage");
						}
					}
				}
				
				if(!zedboardIp.empty() && zedboardPort == 0) {
					throw std::invalid_argument("Missing zedboard port");
				} else if(!zedboardIp.empty()) {
					isRx = true;
					
					// Compute numerical ip address for quicker matching
					char* end = 0;
					char* pnIP = (char*)&nIp;
					pnIP[3] = strtoul(handshakeIp.c_str(), &end, 10);
					end++;
					char* end2 = 0;
					pnIP[2] = strtoul(end, &end2, 10);
					end2++;
					char* end3 = 0;
					pnIP[1] = strtoul(end2, &end3, 10);
					end3++;
					char* end4 = 0;
					pnIP[0] = strtoul(end3, &end4, 10);
					
					// Connection to zedboard
					//hardwareConnection = new ::net::tcp_client<response, request>(ioService, zedboardIp.c_str(), zedboardPort);
					
					// Server for requests
					/** \todo fix memory leak here! (well it should be strdup) */
					auto tip = new char[strlen(handshakeIp.c_str()) + 1];
					strcpy(tip, handshakeIp.c_str());
					//thread = new std::thread(&lcc_and_fpga::work, this, std::ref(ioService), tip, qPort);
				} else {
					isRx = false;
				}
			}
			
			void lcc_and_fpga::async_work(::buffer::queue_buffer& out) {
				UNUSED(out);
			}
			
			::module::iproc_unit::response* lcc_and_fpga::proc_act_request(
						const ::module::iproc_unit::request& request) {
				UNUSED(request);
				
				return NULL;
			}
			
			bool lcc_and_fpga::proc_act_push(
					const ::module::iproc_unit::request& request) {
				UNUSED(request);
				
				return false;
			}
			
			/*
			void lcc_and_fpga::source_work(::buffer::queue_buffer& buffer) {
				auto items = buffer.pop_all();
				
				while(items.size() != 0) {
					::buffer::buffer_item& item(items.front());
					
					switch(((char*)item.data())[0]) {
						case 0:
						case '0':
							controller0->write("mode=1\r");
							controller0->dump();
							controller1->write("mode=1\r");
							controller1->dump();
							break;
						case 1:
						case '1':
							controller0->write("mode=2\r");
							controller0->dump();
							controller1->write("mode=1\r");
							controller1->dump();
							break;
						case 2:
						case '2':
							controller0->write("mode=1\r");
							controller0->dump();
							controller1->write("mode=2\r");
							controller1->dump();
							break;
						case 3:
						case '3':
							controller0->write("mode=2\r");
							controller0->dump();
							controller1->write("mode=2\r");
							controller1->dump();
							break;
						default:
							throw std::exception();
					}
					 
					::net::client<::net::response, ::net::request>* requestClient;
					std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(400));
					std::uint_fast32_t ipNum = item.parameters()[0][0] +
							(item.parameters()[0][1] << 8) +
							(item.parameters()[0][2] << 16) +
							(item.parameters()[0][3] << 24);
					
					requestClient = new ::net::client<::net::response, ::net::request>(ioService, ipNum, qPort);
					requestClient->write(std::move(::net::request(nIp, ipNum, 2, 0)));
					requestClient->read();
					
					std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(30000));
					
					requestClient->write(std::move(::net::request(nIp, ipNum, 2, 0)));
					requestClient->read();
					delete requestClient;
					items.pop();
				}
			}*/
			
			/*
			void lcc_and_fpga::sink_work(::buffer::queue_buffer& buffer) {
				if(isRx) {
					std::unique_lock<std::mutex> lock(_bufferMutex);
					if(_buffer.size() != 0) {
						for(auto i : _buffer) {
							std::string temps = std::string("{\"result\":")+std::to_string(i)+std::string("}");
							buffer.push(::buffer::buffer_item((char*)temps.data(), temps.size(), 0, 0, false));
						}
						_buffer.clear();
					}
				}
			}*/
			
			void lcc_and_fpga::work(boost::asio::io_service& ioService,
					const char* const address,
					const std::uint_fast16_t port) {
				//auto requestServer = new ::net::server<lcc_and_fpga, ::net::request, ::net::response>(ioService,
				//		address,
				//		port,
				//		*this,
				//		&lcc_and_fpga::process);
						
				// Block until ioService.stop() is called
				boost::asio::io_service::work work(ioService);
				ioService.run();
				UNUSED(port);
				//delete requestServer;
				delete[] address;
			}
			
			void lcc_and_fpga::process(::net::request& incomingMessage,
						::net::response& outgoingMessage) {
				UNUSED(incomingMessage);
				outgoingMessage.set_status(::net::response_status_code::OK);
				
				if(isReceiving) {
					hardwareConnection->write(request(false));
					
					response data(hardwareConnection->read());
					
					std::size_t count[4] = {0};
					for(std::size_t i = 0; i < data.length(); i++) {
						switch(data.data()[i]) {
							case 0:
								count[0]++;
								break;
							case 1:
								count[1]++;
								break;
							case 2:
								count[2]++;
								break;
							case 3:
								count[3]++;
								break;
						}
					}
					
					char temp[1];
					auto max = std::max_element(count, count+4);
					if(*max == count[0]) {
						temp[0] = 0;
					} else if(*max == count[1]) {
						temp[0] = 1;
					} else if(*max == count[2]) {
						temp[0] = 2;
					} else if(*max == count[3]) {
						temp[0] = 3;
					} else {
						throw std::exception();
					}
					std::unique_lock<std::mutex> lock(_bufferMutex);
					_buffer.push_back(temp[0]);
				} else {
					hardwareConnection->write(request(true));
				}
				
				isReceiving = !isReceiving;
			}
		}
	}
}
