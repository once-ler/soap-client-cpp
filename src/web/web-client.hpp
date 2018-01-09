#pragma once

// typeinfo required by asio
#include <type_traits>
#include "Simple-Web-Server/client_http.hpp"
#include "Simple-Web-Server/client_https.hpp"
#include <iostream>
#include <list>
#include <algorithm>
#include <unordered_map>
#include <functional>
#include <map>

using namespace std;

static const list<string> methods{ "GET", "POST", "PUT", "PATCH", "DEL" };

namespace web {
  template<typename T>
  class client {    
  public:
    using HTTP_Response = shared_ptr<typename SimpleWeb::ClientBase<T>::Response>;
      
    client(const string& _host) : host(_host) {
      for_each(methods.begin(), methods.end(), [this](const string verb)->void {
        this->method[verb] = [verb, this](
          const string& params,
          const string& data,
          const map<string, string>& header
        )->HTTP_Response {
          
          auto apiCall = [this](
            const string& host,
            const string& verb,
            const string& params,
            const string& data,
            const map<string, string>& header
          )->HTTP_Response {
            SimpleWeb::CaseInsensitiveMultimap mm;
            
            for_each(header.begin(), header.end(), [&mm](auto& e) { mm.insert(move(e)); });
            
            HTTP_Response r1;
            
            try {
              SimpleWeb::Client<T> client(host);              
              
              if (data.size() > 0) {
                stringstream ss;
                ss << data;
                r1 = client.request(verb, params, ss, mm);
              } else {
                r1 = client.request(verb, params, "", mm);
              }
            } catch (const SimpleWeb::system_error& e) {
            } catch (const exception& e) {              
            }

            return r1;
          };

          return apiCall(host, verb, params, data, header);

        };
        
      }); // le fin for_each
    }
    /*
    map of <key, fn(string, string, map<string,string>) => HTTP_Response>
    */
    unordered_map <string, function<HTTP_Response(const string&, const string&, const map<string, string>&)>> method;
  private:
    string host;
  };
}
