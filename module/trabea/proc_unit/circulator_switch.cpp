#include "circulator_switch.hpp"
#include <chrono>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>
#include <boost/tokenizer.hpp>

namespace module {
	namespace trabea {
		namespace proc_unit {
			const char* circulator_switch::chirality_to_string(const chirality state) {
				switch(state) {
				 case chirality::cw:
					return "cw";
				 case chirality::ccw:
					return "ccw";
				 default:
					throw std::invalid_argument(err_msg::_undhcse);
				}
			}
			
			circulator_switch::circulator_switch()
					: nIP(0) {
			}
			
			circulator_switch::~circulator_switch() {
			}
			
			::module::iproc_unit* circulator_switch::initialize() {
				return new circulator_switch();
			}
			
			void circulator_switch::string_initialize_parameters(const char* const parameters) {
				std::string tempParameters(parameters);
				std::string address;
				std::string txDispatcherLocation;
				
				// Tokenize string
				boost::escaped_list_separator<char> seperator("\\", "= ", "\"\'");
				boost::tokenizer<boost::escaped_list_separator<char> > tokens(tempParameters, seperator);
				std::vector<std::string> tokenStrings;
				std::copy_if(tokens.begin(),
						tokens.end(),
						std::back_inserter(tokenStrings),
						!boost::bind(&std::string::empty, _1));
				
				try {
					namespace po = boost::program_options;
					
					po::options_description desc("Options");
					desc.add_options()
						("e", po::value<std::string>(&address)->required(), "IP Address")
						("p", po::value<std::size_t>(&portCount)->required(), "Port Count")
						("td", po::value<std::string>(&txDispatcherLocation)->required(), "Dispatcher TX Location");
					
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
				
				char* end = 0;
				char* pnIP = (char*)&nIP;
				pnIP[3] = strtoul(address.c_str(), &end, 10);
				end++;
				char* end2 = 0;
				pnIP[2] = strtoul(end, &end2, 10);
				end2++;
				char* end3 = 0;
				pnIP[1] = strtoul(end2, &end3, 10);
				end3++;
				char* end4 = 0;
				pnIP[0] = strtoul(end3, &end4, 10);
				
				dispatcher = new ::net::simulation::client(txDispatcherLocation.c_str());
				
				// Send initial configuration
				state = chirality::ccw;
				
				::net::simulation::request request("configure_qswitch", false);
				request.add<unsigned int>(htonl(nIP))
						.add<const char*, false>("circulator_switch")
						.add<const char*, false>(chirality_to_string(state));
				auto rspns = dispatcher->call(request);
				
				if(rspns.get_error()) {
					throw std::runtime_error("Dispatcher call failed");
				}
			}
			
			bool circulator_switch::get_switch_state(const std::size_t inPort,
						const std::size_t outPort) {
				if(inPort + 1 == outPort || inPort + 1 == portCount) {
					// chirality::cw
					return state == chirality::cw;
				} else {
					// chirality::ccw
					return state == chirality::ccw;
				}
			}
			
			bool circulator_switch::set_switch_state(const std::size_t inPort,
						const std::size_t outPort) {
				if(UNLIKELY(inPort >= portCount || outPort >= portCount)) {
					return false;
				}
				
				chirality newState;
				if(inPort + 1 == outPort || inPort + 1 == portCount) {
					newState = chirality::cw;
				} else {
					newState = chirality::ccw;
				}
				
				if(newState != state) {
					state = newState;
					::net::simulation::request request("configure_qswitch", false);
					request.add<unsigned int>(htonl(nIP))
							.add<const char*, false>("circulator_switch")
							.add<const char*, false>(chirality_to_string(state));
					const auto result = dispatcher->call(request);
					
					return !result.get_error();
				}
				
				// We are already in the state we need to be
				return true;
			}
			
		}
	}
}
