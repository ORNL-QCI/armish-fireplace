#ifndef _NET_MIDDLEWARE_RESPONSE_HPP
#define _NET_MIDDLEWARE_RESPONSE_HPP

#include <common.hpp>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

/**
 * \brief The JSON object name that holds the result value.
 */
#define NET_MIDDLEWARE_REQUEST_RESULT_STR "result"

/**
 * \brief The JSON object name that holds the error string value.
 */
#define NET_MIDDLEWARE_REQUEST_ERROR_STR "error"

namespace net {
	namespace middleware {
		/**
		 * \brief A JSON reponse to a client.
		 */
		struct response {
		 public:
			/**
			 * \brief Direct object initializer for C string type result.
			 */
			response(const char* const result, bool error = false) {
				#ifdef THROW
				if(UNLIKELY(result == 0)) {
					throw std::invalid_argument(err_msg::_nllpntr);
				}
				#endif
				
				auto dom(dom_setup());
				dom.AddMember(NET_MIDDLEWARE_REQUEST_RESULT_STR,
						::rapidjson::Value(result, dom.GetAllocator()),
						dom.GetAllocator());
				dom_error(dom, error);
				dom_json(dom);
			}
			
			/**
			 * \brief Direct object initializer for bool type result.
			 */
			response(const bool result, const bool error = false) {
				auto dom(dom_setup());
				dom.AddMember(NET_MIDDLEWARE_REQUEST_RESULT_STR, result, dom.GetAllocator());
				dom_error(dom, error);
				dom_json(dom);
			}
			
			/**
			 * \brief Direct object initializer for an int type result.
			 */
			response(const int result, const bool error = false) {
				auto dom(dom_setup());
				dom.AddMember(NET_MIDDLEWARE_REQUEST_RESULT_STR, result, dom.GetAllocator());
				dom_error(dom, error);
				dom_json(dom);
			}
			
			/**
			 * \brief Direct object initializer for an unsigned int type result.
			 */
			response(const unsigned int result, const bool error = false) {
				auto dom(dom_setup());
				dom.AddMember(NET_MIDDLEWARE_REQUEST_RESULT_STR, result, dom.GetAllocator());
				dom_error(dom, error);
				dom_json(dom);
			}
			
			/**
			 * \brief Direct object initializer for a long int type result.
			 */
			response(const long int result, const bool error = false) {
				auto dom(dom_setup());
				dom.AddMember(NET_MIDDLEWARE_REQUEST_RESULT_STR, result, dom.GetAllocator());
				dom_error(dom, error);
				dom_json(dom);
			}
			
			/**
			 * \brief Direct object initializer for an unsigned long int type result.
			 */
			response(const unsigned long int result, const bool error = false) {
				auto dom(dom_setup());
				dom.AddMember(NET_MIDDLEWARE_REQUEST_RESULT_STR, result, dom.GetAllocator());
				dom_error(dom, error);
				dom_json(dom);
			}
			
			/**
			 * \brief Copy constructor is disabled.
			 */
			response(const response&) = delete;
			
			/**
			 * \brief Move constructor.
			 */
			response(response&& old)
					: _jbuffer(std::move(old._jbuffer)) {
			}
			
			/**
			 * \brief Assignment operator is disabled.
			 */
			response& operator=(const response&) = delete; 
			
			/**
			 * \brief Move assignment operator.
			 */
			response& operator=(response&& old) {
				_jbuffer = std::move(old._jbuffer);
				
				return *this;
			}
			
			/**
			 * \brief Destructor.
			 */
			~response() {
			}
			
			/**
			 * \brief Return a JSON encoded string of the object.
			 */
			inline const char* json() const {
				return _jbuffer.GetString();
			}
			
			/**
			 * \brief Return the number of character of the JSON string.
			 * 
			 * \note Does not include null terminator.
			 */
			inline std::size_t size() const {
				return _jbuffer.GetSize();
			}
		
		 private:
			/**
			 * \brief This buffer stores our JSON string.
			 */
			::rapidjson::StringBuffer _jbuffer;
			
			/**
			 * \brief Setup a dom.
			 */
			inline ::rapidjson::Document dom_setup() const {
				::rapidjson::Document dom;
				dom.SetObject();
				return dom;
			}
			
			/**
			 * \brief Setup a dom for an error response.
			 */
			inline void dom_error(::rapidjson::Document& dom, const bool error) const {
				dom.AddMember(NET_MIDDLEWARE_REQUEST_ERROR_STR, error, dom.GetAllocator());
			}
			
			/**
			 * \brief Turn a dom into a string of json.
			 */
			inline void dom_json(::rapidjson::Document& dom) {
				::rapidjson::Writer<::rapidjson::StringBuffer> writer(_jbuffer);
				dom.Accept(writer);
			}
		};
	}
}

#endif
