#include "server.hpp"

namespace net {
	namespace middleware {
		server::server(::module::module_manager& moduleManager)
				: moduleManager(moduleManager),
				moduleAsyncBuffer(NULL),
				iEndpoint{'\0'},
				oEndpoint{'\0'},
				isRunning(false),
				doExit(false),
				startupState(0),
				startupStateTarget(0),
				workThreads{} {
			auto callback = ::module::module_manager::callback_t<server>{
					this,
					&::net::middleware::server::notify
				};
			
			moduleManager.register_callback<::net::middleware::server>(std::move(callback));
		}
		
		server::~server() {
			{
				// Unregister from module manager
				lock_t stateLock(stateMutex);
				
				moduleManager.unregister_callback();
			}
			
			stop();
		}
		
		void server::setup(const char* const iEndpoint,
				const char* const oEndpoint) {
			lock_t lock(startMutex);
			
			if(isRunning) {
				return;
			}
			
			const auto iEndpointLen = strlen(iEndpoint) + 1;
			const auto oEndpointLen = strlen(oEndpoint) + 1;
			
			if(UNLIKELY(iEndpointLen > sizeof(this->iEndpoint) ||
					oEndpointLen > sizeof(this->oEndpoint))) {
				throw std::invalid_argument(err_msg::_arybnds);
			}
			
			memcpy(this->iEndpoint, iEndpoint, iEndpointLen);
			memcpy(this->oEndpoint, oEndpoint, oEndpointLen);
		}
		
		void server::stop() {
			lock_t stateLock(stateMutex);
			
			if(!isRunning) {
				return;
			}
			
			doExit = true;
			
			for(auto& t : workThreads) {
				#ifdef THROW
				try {
					// This may be superfluous but it makes reordering or other nonsense
					// obvious.
					if(!doExit) {
						throw std::logic_error(err_msg::_unrchcd);
					}
				#endif
					if(t.joinable()) {
						t.join();
					}
				#ifdef THROW
				} catch(const std::system_error& e) {
					if(e.code() == std::errc::no_such_process ||
							e.code() == std::errc::invalid_argument ||
							e.code() == std::errc::resource_deadlock_would_occur) {
						// This should never ever happen and if it does you should run
						throw std::logic_error(err_msg::_unrchcd);
					}
				}
				#endif
			}
			
			doExit = false;
			isRunning = false;
		}
		
		void server::notify(::buffer::queue_buffer& asyncBuffer,
				const ::actions::actions_list_t supActs) {
			stop();
			
			lock_t stateLock(stateMutex);
			
			startupStateTarget = 0;
			
			moduleAsyncBuffer = &asyncBuffer;
			
			if(::actions::check<::actions::actions_t::REQUEST>(supActs) ||
					::actions::check<::actions::actions_t::PUSH>(supActs)) {
				#ifdef THROW
				if(UNLIKELY(iEndpoint[0] == '\0')) {
					throw std::runtime_error(err_msg::_zrlngth);
				}
				#endif
				
				workThreads[startupStateTarget++] = std::thread(&server::sync_work, this);
			}
			if(::actions::check<::actions::actions_t::REPLY>(supActs) ||
					::actions::check<::actions::actions_t::WAIT>(supActs)) {
				#ifdef THROW
				if(UNLIKELY(oEndpoint[0] == '\0')) {
					throw std::runtime_error(err_msg::_zrlngth);
				}
				#endif
				
				workThreads[startupStateTarget++] = std::thread(&server::async_work, this);
			}
			
			// Wait until launched threads have signaled that they are ready
			start_lock_t startLock(startMutex);
			startCV.wait(startLock,
					[this] {
						return (startupState == startupStateTarget);
					});
		}
		
		void server::notify_thread_started() {
			start_lock_t startLock(startMutex);
			
			if(++startupState == startupStateTarget) {
				startLock.unlock();
				startCV.notify_all();
			}
		}
	}
}
