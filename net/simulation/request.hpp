#ifndef _NET_SIMULATION_REQUEST_HPP
#define _NET_SIMULATION_REQUEST_HPP

#include <common.hpp>
#include <functional>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

/**
 * \brief The JSON object name that holds the method string value.
 */
#define NET_MIDDLEWARE_SIMULATION_METHOD_STR "method"

/**
 * \brief The JSON object name that holds the parameters array values.
 */
#define NET_MIDDLEWARE_SIMULATION_PARAMS_STR "parameters"

namespace net {
	namespace simulation {
		/**
		 * \brief A request for the simulation server.
		 */
		class request {
		 public:
			/**
			 * \brief Direct object constructor with cstring.
			 * 
			 * If reAllocate is true, we copy the method string.
			 */
			request(const char* const method, const bool reAllocate);
			
			/**
			 * \brief Copy constructor is disabled.
			 */
			request(const request&) = delete;
			
			/**
			 * \brief Move constructor.
			 */
			request(request&& old);
			
			/**
			 * \brief Assignment operator is disabled.
			 */
			request& operator=(const request&) = delete; 
			
			/**
			 * \brief Move assignment operator.
			 */
			request& operator=(request&& old);
			
			/**
			 * \brief Add a parameter to the request parameter array.
			 * 
			 * This returns a reference to the current object as to implement a fluent
			 * interface.
			 * 
			 * The reallocate template parameter signifies whether or not we are to
			 * reallocate the data when adding it to our request. This is by default true,
			 * which doesn't make a difference when dealing with numeric types. However
			 * for cstring this means we can skip a copy of the string.
			 */
			template <typename T, bool reallocate = true>
					inline request& add(const T data) {
				static_assert(!(
						(std::is_same<T, char*>::value ||
						 std::is_same<T, const char*>::value ||
						 std::is_same<T, char* const>::value ||
						 std::is_same<T, const char* const>::value ||
						 std::is_same<T, unsigned char*>::value ||
						 std::is_same<T, const unsigned char*>::value ||
						 std::is_same<T, unsigned char* const>::value ||
						 std::is_same<T, const unsigned char* const>::value)
						&& reallocate),
						"Function only takes arrays/buffers as references");
				
				dom[NET_MIDDLEWARE_SIMULATION_PARAMS_STR].PushBack(data, domAllctr);
				
				return *this;
			}
			
			/**
			 * \brief Generate the json string from the request.
			 */
			void generate_json();
			
			/**
			 * \brief Return a char array that is a JSON encoding of the object.
			 * 
			 * \warning You must call generate_json() before calling.
			 */
			const char* get_json() const {
				return jBuffer.GetString();
			}
			
			/**
			 * \brief Return a the length of the JSON string.
			 * 
			 * \warning This includes the null terminator.
			 */
			std::size_t get_json_str_size() const {
				return jBuffer.GetSize()+1;
			}
		
		 private:
			/**
			 * \brief The RapidJSON document that stores the request in object form.
			 */
			::rapidjson::Document dom;
			
			/**
			 * \brief The allocator for the RapidJSON document, used for faster access.
			 */
			std::reference_wrapper<::rapidjson::Document::AllocatorType> domAllctr;
			
			/**
			 * \brief The RapidJSON buffer for the json string.
			 */
			::rapidjson::StringBuffer jBuffer;
		};
		
		/**
		 * \brief Add a cstring by reference to the parameter array.
		 */
		template <>
				inline request& request::add<const char*, false>(const char* const data) {
			dom[NET_MIDDLEWARE_SIMULATION_PARAMS_STR].PushBack(
					::rapidjson::Value().SetString(::rapidjson::StringRef(data)),
					domAllctr);
			
			return *this;
		}
	}
}

#endif
