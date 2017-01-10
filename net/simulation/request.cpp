#include "request.hpp"

namespace net {
	namespace simulation {
		request::request(const char* const method,
				const bool reAllocate)
				: dom(),
				domAllctr(dom.GetAllocator()) {
			dom.SetObject();
			
			if(reAllocate) {
				dom.AddMember(NET_MIDDLEWARE_SIMULATION_METHOD_STR,
						::rapidjson::Value(method, domAllctr),
						domAllctr);
			} else {
				dom.AddMember(NET_MIDDLEWARE_SIMULATION_METHOD_STR,
						::rapidjson::Value().SetString(::rapidjson::StringRef(method)),
						domAllctr);
			}
			
			dom.AddMember(NET_MIDDLEWARE_SIMULATION_PARAMS_STR,
				::rapidjson::Value(::rapidjson::kArrayType),
				domAllctr);
		}
		
		request::request(request&& old)
				: dom(std::move(old.dom)),
				domAllctr(std::move(old.domAllctr)),
				jBuffer(std::move(old.jBuffer)) {
		}
		
		request& request::operator=(request&& old) {
			dom = std::move(old.dom);
			domAllctr = std::move(old.domAllctr);
			jBuffer = std::move(old.jBuffer);
			
			return *this;
		}
		
		void request::generate_json() {
			::rapidjson::Writer<::rapidjson::StringBuffer> writer(jBuffer);
			
			dom.Accept(writer);
		}
	}
}
