#ifndef _NET_MIDDLEWARE_REQUEST_HPP
#define _NET_MIDDLEWARE_REQUEST_HPP

#include <common.hpp>
#include <actions.hpp>
#include <rapidjson/document.h>

/**
 * \brief The JSON object name that holds the action value.
 */
#define NET_MIDDLEWARE_REQUEST_ACTION_STR "action"

/**
 * \brief The JSON object name that holds the method string value.
 */
#define NET_MIDDLEWARE_REQUEST_METHOD_STR "method"

/**
 * \brief The JSON object name that holds the parameters array values.
 */
#define NET_MIDDLEWARE_REQUEST_PARAMS_STR "parameters"

namespace net {
	namespace middleware {
		/**
		 * \brief A request from the client.
		 */
		struct request {
		 public:
			/**
			 * \brief Decoding constructor takes in json in mutable cstring.
			 */
			request(char* const input)
					: _dom() {
				_dom.ParseInsitu(input);
				
				#ifdef THROW
				if(UNLIKELY(!_dom.HasMember(NET_MIDDLEWARE_REQUEST_ACTION_STR))) {
					throw std::runtime_error(err_msg::_malinpt);
				}
				#endif
				
				_action = ::actions::str_map(_dom[NET_MIDDLEWARE_REQUEST_ACTION_STR].GetString());
			}
			
			/**
			 * \brief Constructor for directly feeding data from a zmq message.
			 * 
			 * The input string is still a cstring, but this saves us from casting at the
			 * source.
			 */
			request(void* const input)
				: request((char*)input) {
			}
			
			/**
			 * \brief Copy constructor is disabled.
			 */
			request(const request&) = delete;
			
			 /**
			 * \brief Move constructor.
			 */
			request(request&& old)
					: _dom(std::move(old._dom)) {
			}
			
			 /**
			 * \brief Assignment operator is disabled.
			 */
			request& operator=(const request&) = delete; 
			
			 /**
			 * \brief Move assignment operator.
			 */
			request& operator=(request&& old) {
				_dom = std::move(old._dom);
				
				return *this;
			}
			
			/**
			 * \brief Return the method.
			 */
			inline const char* method() const {
				return _dom[NET_MIDDLEWARE_REQUEST_METHOD_STR].GetString();
			}
			
			/**
			 * \brief Return a type T parameter by index.
			 * 
			 * \warning You must use a template specialized function.
			 */
			template <typename T> inline T parameter(const std::size_t idx) const;
			
			/**
			 * \brief Return the size of a parameter string by index.
			 */
			inline std::size_t parameter_str_size(const std::size_t idx) const {
				return _dom[NET_MIDDLEWARE_REQUEST_PARAMS_STR][idx].GetStringLength();
			}
			
			/**
			 * \brief Return the size of a parameter array by index.
			 */
			inline std::size_t parameter_array_size(const std::size_t idx) const {
				return _dom[NET_MIDDLEWARE_REQUEST_PARAMS_STR][idx].Size();
			}
			
			/**
			 * \brief Return the action type of the request.
			 */
			inline ::actions::actions_t action() const {
				return _action;
			}
		
		 private:
			/**
			 * \brief The rapidjson DOM object that holds our JSON.
			 */
			::rapidjson::Document _dom;
			
			/**
			 * \brief The action type of the request.
			 */
			::actions::actions_t _action;
		};
		
		/**
		 * \brief Return a cstring parameter by index.
		 */
		template <> inline const char*
				request::parameter<const char*>(const std::size_t idx) const {
			return _dom[NET_MIDDLEWARE_REQUEST_PARAMS_STR][idx].GetString();
		}
		
		/**
		 * \brief Return a cstring array parameter by index.
		 * 
		 * \note Pointer returned must be deallocated. The strings do not need to be
		 * deallocated.
		 */
		template <> inline const char* const*
				request::parameter<const char* const*>(const std::size_t idx) const {
			auto size = _dom[NET_MIDDLEWARE_REQUEST_PARAMS_STR][idx].Size();
			auto array = new const char*[size];
			for(std::size_t i = 0; i < size; i++) {
				array[i] = _dom[NET_MIDDLEWARE_REQUEST_PARAMS_STR][idx][i].GetString();
			}
			
			return array;
		}
		
		/**
		 * \brief Return a bool parameter by index.
		 */
		template <> inline bool
				request::parameter<bool>(const std::size_t idx) const {
			return _dom[NET_MIDDLEWARE_REQUEST_PARAMS_STR][idx].GetBool();
		}
		
		/**
		 * \brief Return a bool array parameter by index.
		 * 
		 * \note Pointer returned must be deallocated.
		 */
		template <> inline bool*
				request::parameter<bool*>(const std::size_t idx) const {
			auto size = _dom[NET_MIDDLEWARE_REQUEST_PARAMS_STR][idx].Size();
			auto array = new bool[size];
			for(std::size_t i = 0; i < size; i++) {
				array[i] = _dom[NET_MIDDLEWARE_REQUEST_PARAMS_STR][idx][i].GetBool();
			}
			
			return array;
		}
		
		/**
		 * \brief Return a char parameter by index.
		 */
		template <> inline char
				request::parameter<char>(const std::size_t idx) const {
			return _dom[NET_MIDDLEWARE_REQUEST_PARAMS_STR][idx].GetString()[0];
		}
		
		/**
		 * \brief Return an unsigned short parameter by index.
		 * 
		 * \warning Possible loss of precision.
		 */
		template <> inline unsigned short
				request::parameter<unsigned short>(const std::size_t idx) const {
			return static_cast<unsigned short>(_dom[NET_MIDDLEWARE_REQUEST_PARAMS_STR][idx].GetUint());
		}
		
		/**
		 * \brief Return an unsigned short array parameter by index.
		 * 
		 * \warning Possible loss of precision.
		 * 
		 * \note Pointer returned must be deallocated.
		 */
		template <> inline unsigned short*
				request::parameter<unsigned short*>(const std::size_t idx) const {
			auto size = _dom[NET_MIDDLEWARE_REQUEST_PARAMS_STR][idx].Size();
			auto array = new unsigned short[size];
			for(std::size_t i = 0; i < size; i++) {
				array[i] = static_cast<unsigned short>(_dom[NET_MIDDLEWARE_REQUEST_PARAMS_STR][idx][i].GetUint());
			}
			
			return array;
		}
		
		/**
		 * \brief Return a short parameter by index.
		 * 
		 * \warning Possible loss of precision.
		 */
		template <> inline short
				request::parameter<short>(const std::size_t idx) const {
			return static_cast<short>(_dom[NET_MIDDLEWARE_REQUEST_PARAMS_STR][idx].GetInt());
		}
		
		/**
		 * \brief Return a short array parameter by index.
		 * 
		 * \warning Possible loss of precision.
		 * 
		 * \note Pointer returned must be deallocated.
		 */
		template <> inline short*
				request::parameter<short*>(const std::size_t idx) const {
			auto size = _dom[NET_MIDDLEWARE_REQUEST_PARAMS_STR][idx].Size();
			auto array = new short[size];
			for(std::size_t i = 0; i < size; i++) {
				array[i] = static_cast<short>(_dom[NET_MIDDLEWARE_REQUEST_PARAMS_STR][idx][i].GetInt());
			}
			
			return array;
		}
		
		/**
		 * \brief Return an unsigned int parameter by index.
		 */
		template <> inline unsigned int
				request::parameter<unsigned int>(const std::size_t idx) const {
			return _dom[NET_MIDDLEWARE_REQUEST_PARAMS_STR][idx].GetUint();
		}
		
		/**
		 * \brief Return an unsigned int array parameter by index.
		 * 
		 * \note Pointer returned must be deallocated.
		 */
		template <> inline unsigned int*
				request::parameter<unsigned int*>(const std::size_t idx) const {
			auto size = _dom[NET_MIDDLEWARE_REQUEST_PARAMS_STR][idx].Size();
			auto array = new unsigned int[size];
			for(std::size_t i = 0; i < size; i++) {
				array[i] = _dom[NET_MIDDLEWARE_REQUEST_PARAMS_STR][idx][i].GetUint();
			}
			
			return array;
		}
		
		/**
		 * \brief Return an int parameter by index.
		 */
		template <> inline int
				request::parameter<int>(const std::size_t idx) const {
			return _dom[NET_MIDDLEWARE_REQUEST_PARAMS_STR][idx].GetInt();
		}
		
		/**
		 * \brief Return an int array parameter by index.
		 * 
		 * \note Pointer returned must be deallocated.
		 */
		template <> inline int*
				request::parameter<int*>(const std::size_t idx) const {
			auto size = _dom[NET_MIDDLEWARE_REQUEST_PARAMS_STR][idx].Size();
			auto array = new int[size];
			for(std::size_t i = 0; i < size; i++) {
				array[i] = _dom[NET_MIDDLEWARE_REQUEST_PARAMS_STR][idx][i].GetInt();
			}
			
			return array;
		}
		
		/**
		 * \brief Return an unsigned long int parameter by index.
		 */
		template <> inline unsigned long int
				request::parameter<unsigned long int>(const std::size_t idx) const {
			return _dom[NET_MIDDLEWARE_REQUEST_PARAMS_STR][idx].GetUint64();
		}
		
		/**
		 * \brief Return an unsigned long int array parameter by index.
		 * 
		 * \note Pointer returned must be deallocated.
		 */
		template <> inline unsigned long int*
				request::parameter<unsigned long int*>(const std::size_t idx) const {
			auto size = _dom[NET_MIDDLEWARE_REQUEST_PARAMS_STR][idx].Size();
			auto array = new unsigned long int[size];
			for(std::size_t i = 0; i < size; i++) {
				array[i] = _dom[NET_MIDDLEWARE_REQUEST_PARAMS_STR][idx][i].GetUint64();
			}
			
			return array;
		}
		
		/**
		 * \brief Return a long int parameter by index.
		 */
		template <> inline long int
				request::parameter<long int>(const std::size_t idx) const {
			return _dom[NET_MIDDLEWARE_REQUEST_PARAMS_STR][idx].GetInt64();
		}
		
		/**
		 * \brief Return a long int array parameter by index.
		 * 
		 * \note Pointer returned must be deallocated.
		 */
		template <> inline long int*
				request::parameter<long int*>(const std::size_t idx) const {
			auto size = _dom[NET_MIDDLEWARE_REQUEST_PARAMS_STR][idx].Size();
			auto array = new long int[size];
			for(std::size_t i = 0; i < size; i++) {
				array[i] = _dom[NET_MIDDLEWARE_REQUEST_PARAMS_STR][idx][i].GetInt64();
			}
			
			return array;
		}
		
		/**
		 * \brief Return a float parameter by index.
		 */
		template <> inline float
				request::parameter<float>(const std::size_t idx) const {
			return _dom[NET_MIDDLEWARE_REQUEST_PARAMS_STR][idx].GetDouble();
		}
		
		/**
		 * \brief Return a float array parameter by index.
		 * 
		 * \note Pointer returned must be deallocated.
		 */
		template <> inline float*
				request::parameter<float*>(const std::size_t idx) const {
			auto size = _dom[NET_MIDDLEWARE_REQUEST_PARAMS_STR][idx].Size();
			auto array = new float[size];
			for(std::size_t i = 0; i < size; i++) {
				array[i] = _dom[NET_MIDDLEWARE_REQUEST_PARAMS_STR][idx][i].GetDouble();
			}
			
			return array;
		}
		
		/**
		 * \brief Return a double parameter by index.
		 */
		template <> inline double
				request::parameter<double>(const std::size_t idx) const {
			return _dom[NET_MIDDLEWARE_REQUEST_PARAMS_STR][idx].GetDouble();
		}
		
		/**
		 * \brief Return a double array parameter by index.
		 * 
		 * \note Pointer returned must be deallocated.
		 */
		template <> inline double*
				request::parameter<double*>(const std::size_t idx) const {
			auto size = _dom[NET_MIDDLEWARE_REQUEST_PARAMS_STR][idx].Size();
			auto array = new double[size];
			for(std::size_t i = 0; i < size; i++) {
				array[i] = _dom[NET_MIDDLEWARE_REQUEST_PARAMS_STR][idx][i].GetDouble();
			}
			
			return array;
		}
	}
}

#endif
