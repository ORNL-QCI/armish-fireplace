#ifndef _NET_SIMULATION_RESPONSE_HPP
#define _NET_SIMULATION_RESPONSE_HPP

#include <common.hpp>
#include <rapidjson/document.h>
#include <rapidjson/memorystream.h>
#include <rapidjson/writer.h>

/**
 * \brief The JSON object name that holds the result value.
 */
#define NET_SIMULATION_RESPONSE_RESULT_STR "result"

/**
 * \brief The JSON object name that holds the error string value.
 */
#define NET_SIMULATION_RESPONSE_ERROR_STR "error"

namespace net {
	namespace simulation {
		/**
		 * \brief A reponse from the simulation server.
		 * 
		 * \warning Not threadsafe.
		 */
		class response {
		 public:
			/**
			 * \brief Decoding constructor takes in the JSON response as a C string.
			 * 
			 * \todo Parse insitu.
			 */
			response(const char* const input, const std::size_t size);
			
			/**
			 * \brief Copy constructor is disabled.
			 */
			response(const response&) = delete;
			
			/**
			 * \brief Move constructor.
			 */
			response(response&& old);
			
			/**
			 * \brief Assignment operator is disabled.
			 */
			response& operator=(const response&) = delete; 
			
			/**
			 * \brief Move assignment operator.
			 */
			response& operator=(response&& old);
			
			/**
			 * \brief Return whether or not there was an error.
			 */
			bool get_error() const;
			
			/**
			 * \brief Return a type T result.
			 */
			template <typename T> T get_result() const;
		
		 private:
		 	/**
			 * \brief The RapidJSON DOM object that holds our JSON.
			 */
			::rapidjson::Document dom;
		};
		
		
		
		
		
		
		/**
		 * \brief Return a cstring result.
		 */
		template <> const char*
				response::get_result<const char*>() const;
		
		/**
		 * \brief Return a cstring array result.
		 * 
		 * \note Pointer returned must be deallocated. The strings do not need to be
		 * deallocated.
		 */
		template <> const char* const*
				response::get_result<const char* const*>() const;
		
		
		
		
		
		
		
		/**
		 * \brief Return a bool result.
		 */
		template <> bool
				response::get_result<bool>() const;
		
		/**
		 * \brief Return a bool array result.
		 * 
		 * \note Pointer returned must be deallocated.
		 */
		template <> bool*
				response::get_result<bool*>() const;
		
		/**
		 * \brief Return a char result.
		 */
		template <> char
				response::get_result<char>() const;
		
		/**
		 * \brief Return an unsigned char result.
		 */
		template <> unsigned char
				response::get_result<unsigned char>() const;
		
		/**
		 * \brief Return a short result.
		 * 
		 * \warning Possible loss of precision.
		 */
		template <> short
				response::get_result<short>() const;
		
		/**
		 * \brief Return a short array result.
		 * 
		 * \warning Possible loss of precision.
		 * 
		 * \note Pointer returned must be deallocated.
		 */
		template <> short*
				response::get_result<short*>() const;
		
		/**
		 * \brief Return an unsigned short result
		 * 
		 * \warning Possible loss of precision.
		 */
		template <> unsigned short
				response::get_result<unsigned short>() const;
		
		/**
		 * \brief Return an unsigned short array result.
		 * 
		 * \warning Possible loss of precision.
		 * 
		 * \note Pointer returned must be deallocated.
		 */
		template <> unsigned short*
				response::get_result<unsigned short*>() const;
		
		/**
		 * \brief Return an int result.
		 */
		template <> int
				response::get_result<int>() const;
		
		/**
		 * \brief Return an int array result.
		 * 
		 * \note Pointer returned must be deallocated.
		 */
		template <> int*
				response::get_result<int*>() const;
		
		/**
		 * \brief Return an unsigned int result.
		 */
		template <> unsigned int
				response::get_result<unsigned int>() const;
		
		/**
		 * \brief Return an unsigned int array result.
		 * 
		 * \note Pointer returned must be deallocated.
		 */
		template <> unsigned int*
				response::get_result<unsigned int*>() const;
		
		/**
		 * \brief Return a long int result.
		 */
		template <> long int
				response::get_result<long int>() const;
		
		/**
		 * \brief Return a long int array result.
		 * 
		 * \note Pointer returned must be deallocated.
		 */
		template <> long int*
				response::get_result<long int*>() const;
		
		/**
		 * \brief Return an unsigned long int result.
		 */
		template <> unsigned long int
				response::get_result<unsigned long int>() const;
		
		/**
		 * \brief Return an unsigned long int array result.
		 * 
		 * \note Pointer returned must be deallocated.
		 */
		template <> unsigned long int*
				response::get_result<unsigned long int*>() const;
		
		/**
		 * \brief Return a float result.
		 */
		template <> float
				response::get_result<float>() const;
		
		/**
		 * \brief Return a float array result.
		 * 
		 * \note Pointer returned must be deallocated.
		 */
		template <> float*
				response::get_result<float*>() const;
		
		/**
		 * \brief Return a double result.
		 */
		template <> double
				response::get_result<double>() const;
		
		/**
		 * \brief Return a double array result.
		 * 
		 * \note Pointer returned must be deallocated.
		 */
		template <> double*
				response::get_result<double*>() const;
	}
}

#endif
