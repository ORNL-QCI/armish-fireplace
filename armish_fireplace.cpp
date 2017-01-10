#include <common.hpp>
#include "buffer/queue_buffer.hpp"
#include "module/module_manager.hpp"
#include "net/middleware/zmq_server.hpp"
#include <csignal>
#include <iostream>
#include <unistd.h>
#include "boost/program_options.hpp"

/**
 * \brief Macro helper for required string arguments.
 */ 
#define B_PO_HELPER_REQ(var) po::value<std::string>(&var)->required()

/**
 * \brief Macro helper for optional string arguments.
 */ 
#define B_PO_HELPER(var) po::value<std::string>(&var)

std::sig_atomic_t signalCode = 0;

void sig_hand(int code) {
	signalCode = code;
}

int main(int argc, char *argv[]) {
	module::module_manager moduleManager;
	
	net::middleware::zmq_server serverInstance(moduleManager);
	net::middleware::server& mwServer = serverInstance;
	
	try {
		/*
		 * Program parameters
		 */
		
		// This is the endpoint that AF binds to and accepts data from the software layer
		// through asynchronous listening.
		std::string iEndpoint;
		
		// This is the endpoint that AF connects to and sends data to the software layer
		// trhough synchronous transmission.
		std::string oEndpoint;
		
		// This is the name of the module to which load processing units from. A module
		// provides code common to processing units.
		std::string moduleName;
		
		// The parameter string to be passed to the module for configuration.
		std::string moduleParam;
		
		// This is the name of the processing unit that is responsible for doing all the
		// domain specific processing.
		std::string procUnitName;
		
		// The parameter string to be passed to the processing unit for configuration.
		std::string procUnitParam;
		
		namespace po = boost::program_options;
		
		po::options_description desc("Options");
		desc.add_options()
			("help,h",										"Print help")
			("iendpoint,i",	B_PO_HELPER_REQ(iEndpoint),		"Input endpoint")
			("oendpoint,o",	B_PO_HELPER_REQ(oEndpoint),		"Output endpoint")
			("mname,m",		B_PO_HELPER_REQ(moduleName),	"Module name")
			("mparam,n",	B_PO_HELPER(moduleParam),		"Module parameters")
			("puname,t",	B_PO_HELPER_REQ(procUnitName),	"Processing unit name")
			("puparam,u",	B_PO_HELPER(procUnitParam),		"Processing unit parameters");
		
		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, desc), vm);
		
		if(vm.count("help")) {
			// Print help and exit
			std::cout << desc << std::endl;
			exit(EXIT_SUCCESS);
		}
		
		po::notify(vm);
		
		// Set server endpoints
		mwServer.setup(iEndpoint.c_str(), oEndpoint.c_str());
		
		// Load module and processing unit
		// The server responds to the change in state automatically
		moduleManager.load_module(moduleName.c_str(), moduleParam.c_str());
		moduleManager.load_proc_unit(procUnitName.c_str(), procUnitParam.c_str());
	} catch(const boost::program_options::required_option& e) {
		std::cerr << e.what() << std::endl;
		exit(EXIT_FAILURE);
	} catch(const boost::program_options::error& e) {
		std::cerr << e.what() << std::endl;
		exit(EXIT_FAILURE);
	} catch(std::exception& e) {
		std::cerr << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}
	
	// Setup termination signal handling
	struct sigaction sigIntHandler;
	sigIntHandler.sa_handler = sig_hand;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;
	// External interrupt
	sigaction(SIGINT, &sigIntHandler, 0);
	// Termination request
	sigaction(SIGTERM, &sigIntHandler, 0);
	
	// Wait for signal
	pause();
	
	// We call destructors of the objects we've created above in the proper order per the
	// standard, and these destructors shutdown everything gracefully.
	return signalCode;
}
