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

namespace web {
  static const list<string> methods{ "GET", "POST", "PUT", "PATCH", "DEL" };

  template<typename T>
  struct web_response {
    shared_ptr<typename SimpleWeb::ClientBase<T>::Response> response;
    shared_ptr<SimpleWeb::system_error> error;
  };

  template<typename T>
  class client {    
  public:
    using HTTP_Response_ = shared_ptr<typename SimpleWeb::ClientBase<T>::Response>;
    using HTTP_Error_ = shared_ptr<SimpleWeb::system_error>;

    client(const string& _host) : host(_host) {
      for_each(methods.begin(), methods.end(), [this](const string& verb) {
        this->method[verb] = [verb, this](
          const string& params,
          const string& data,
          const map<string, string>& header
        )->web_response<T> {
          
          auto apiCall = [this](
            const string& host,
            const string& verb,
            const string& params,
            const string& data,
            const map<string, string>& header
          )->web_response<T> {
            SimpleWeb::CaseInsensitiveMultimap mm;
            
            for_each(header.begin(), header.end(), [&mm](auto& e) { mm.insert(move(e)); });
            
            HTTP_Response_ r1;
            HTTP_Error_ e1;
            
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
              e1 = make_shared<SimpleWeb::system_error>(e);              
            }

            return web_response<T>{ r1, e1 };
          };

          return apiCall(host, verb, params, data, header);

        };
        
      }); // le fin for_each
    }
    /*
    map of <key, fn(string, string, map<string,string>) => http_response>
    */
    unordered_map <string, function<web_response<T>(const string&, const string&, const map<string, string>&)>> method;
  private:
    string host;
  };
}
