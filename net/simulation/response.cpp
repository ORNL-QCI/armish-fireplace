#include "response.hpp"

namespace net {
	namespace simulation {
		response::response(const char* const input,
				const std::size_t size) {
			::rapidjson::MemoryStream ms(input, size);
			dom.ParseStream(ms);
		}
		
		response::response(response&& old)
				: dom(std::move(old.dom)) {
		}
		
		response& response::operator=(response&& old) {
			dom = std::move(old.dom);
			
			return *this;
		}
		
		bool response::get_error() const {
			return (dom.HasMember(NET_SIMULATION_RESPONSE_ERROR_STR) &&
				dom[NET_SIMULATION_RESPONSE_ERROR_STR].GetBool());
		}
		
		template <> const char*
				response::get_result<const char*>() const {
			return dom[NET_SIMULATION_RESPONSE_RESULT_STR].GetString();
		}
		
		template <> const char* const*
				response::get_result<const char* const*>() const {
			auto size = dom[NET_SIMULATION_RESPONSE_RESULT_STR].Size();
			auto array = new const char*[size];
			
			for(std::size_t i = 0; i < size; i++) {
				array[i] = dom[NET_SIMULATION_RESPONSE_RESULT_STR][i].GetString();
			}
			
			return array;
		}
		
		template <> bool
				response::get_result<bool>() const {
			return dom[NET_SIMULATION_RESPONSE_RESULT_STR].GetBool();
		}
		
		template <> bool*
				response::get_result<bool*>() const {
			auto size = dom[NET_SIMULATION_RESPONSE_RESULT_STR].Size();
			auto array = new bool[size];
			
			for(std::size_t i = 0; i < size; i++) {
				array[i] = dom[NET_SIMULATION_RESPONSE_RESULT_STR][i].GetBool();
			}
			
			return array;
		}
		
		template <> char
				response::get_result<char>() const {
			return dom[NET_SIMULATION_RESPONSE_RESULT_STR].GetString()[0];
		}
		
		template <> unsigned char
				response::get_result<unsigned char>() const {
			return dom[NET_SIMULATION_RESPONSE_RESULT_STR].GetString()[0];
		}
		
		template <> unsigned short
				response::get_result<unsigned short>() const {
			return static_cast<unsigned short>(dom[NET_SIMULATION_RESPONSE_RESULT_STR].GetUint());
		}
		
		template <> unsigned short*
				response::get_result<unsigned short*>() const {
			auto size = dom[NET_SIMULATION_RESPONSE_RESULT_STR].Size();
			auto array = new unsigned short[size];
			
			for(std::size_t i = 0; i < size; i++) {
				array[i] = static_cast<unsigned short>(dom[NET_SIMULATION_RESPONSE_RESULT_STR][i].GetUint());
			}
			
			return array;
		}
		
		template <> short
				response::get_result<short>() const {
			return static_cast<short>(dom[NET_SIMULATION_RESPONSE_RESULT_STR].GetInt());
		}
		
		template <> short*
				response::get_result<short*>() const {
			auto size = dom[NET_SIMULATION_RESPONSE_RESULT_STR].Size();
			auto array = new short[size];
			
			for(std::size_t i = 0; i < size; i++) {
				array[i] = static_cast<short>(dom[NET_SIMULATION_RESPONSE_RESULT_STR][i].GetInt());
			}
			
			return array;
		}
		
		template <> unsigned int
				response::get_result<unsigned int>() const {
			return dom[NET_SIMULATION_RESPONSE_RESULT_STR].GetUint();
		}
		
		template <> unsigned int*
				response::get_result<unsigned int*>() const {
			auto size = dom[NET_SIMULATION_RESPONSE_RESULT_STR].Size();
			auto array = new unsigned int[size];
			
			for(std::size_t i = 0; i < size; i++) {
				array[i] = dom[NET_SIMULATION_RESPONSE_RESULT_STR][i].GetUint();
			}
			
			return array;
		}
		
		template <> int
				response::get_result<int>() const {
			return dom[NET_SIMULATION_RESPONSE_RESULT_STR].GetInt();
		}
		
		template <> int*
				response::get_result<int*>() const {
			auto size = dom[NET_SIMULATION_RESPONSE_RESULT_STR].Size();
			auto array = new int[size];
			
			for(std::size_t i = 0; i < size; i++) {
				array[i] =dom[NET_SIMULATION_RESPONSE_RESULT_STR][i].GetInt();
			}
			
			return array;
		}
		
		template <> unsigned long int
				response::get_result<unsigned long int>() const {
			return dom[NET_SIMULATION_RESPONSE_RESULT_STR].GetUint64();
		}
		
		template <> long int
				response::get_result<long int>() const {
			return dom[NET_SIMULATION_RESPONSE_RESULT_STR].GetInt64();
		}
		
		template <> unsigned long int*
				response::get_result<unsigned long int*>() const {
			auto size = dom[NET_SIMULATION_RESPONSE_RESULT_STR].Size();
			auto array = new unsigned long int[size];
			
			for(std::size_t i = 0; i < size; i++) {
				array[i] = dom[NET_SIMULATION_RESPONSE_RESULT_STR][i].GetUint64();
			}
			
			return array;
		}
		
		template <> long int*
				response::get_result<long int*>() const {
			auto size = dom[NET_SIMULATION_RESPONSE_RESULT_STR].Size();
			auto array = new long int[size];
			
			for(std::size_t i = 0; i < size; i++) {
				array[i] = dom[NET_SIMULATION_RESPONSE_RESULT_STR][i].GetInt64();
			}
			
			return array;
		}
		
		template <> float
				response::get_result<float>() const {
			return dom[NET_SIMULATION_RESPONSE_RESULT_STR].GetDouble();
		}
		
		template <> float*
				response::get_result<float*>() const {
			auto size = dom[NET_SIMULATION_RESPONSE_RESULT_STR].Size();
			auto array = new float[size];
			
			for(std::size_t i = 0; i < size; i++) {
				array[i] = dom[NET_SIMULATION_RESPONSE_RESULT_STR][i].GetDouble();
			}
			
			return array;
		}
		
		template <> double
				response::get_result<double>() const {
			return dom[NET_SIMULATION_RESPONSE_RESULT_STR].GetDouble();
		}
		
		template <> double*
				response::get_result<double*>() const {
			auto size = dom[NET_SIMULATION_RESPONSE_RESULT_STR].Size();
			auto array = new double[size];
			
			for(std::size_t i = 0; i < size; i++) {
				array[i] = dom[NET_SIMULATION_RESPONSE_RESULT_STR][i].GetDouble();
			}
			
			return array;
		}
	}
}