#pragma once

#ifndef BOOST_THREAD_PROVIDES_FUTURE
#define BOOST_THREAD_PROVIDES_FUTURE
#endif

#ifndef BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION
#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION
#endif 

#ifndef BOOST_THREAD_PROVIDES_FUTURE_WHEN_ALL_WHEN_ANY
#define BOOST_THREAD_PROVIDES_FUTURE_WHEN_ALL_WHEN_ANY
#endif

// typeinfo required by asio
#include <type_traits>
#include "Simple-Web-Server/client_http.hpp"
#include "Simple-Web-Server/client_https.hpp"
#include <boost/thread/future.hpp>
#include <iostream>
#include <list>
#include <algorithm>
#include <unordered_map>
#include <functional>
#include <map>

using namespace std;

static const std::list<string> methods{ "GET", "POST", "PUT", "PATCH", "DEL" };

namespace web {
  template<typename T>
  class client {    
  public:
    using HTTP_Response = shared_ptr<typename SimpleWeb::ClientBase<T>::Response>;
      
    client(const string& _host) : host(_host) {
      // for each http/s verb, create a method factory that returns a future
      std::for_each(methods.begin(), methods.end(), [this](const string verb)->void {
        // promise pattern
        this->methodPromise[verb] = [verb, this](
          const string& params,
          const string& data,
          const std::map<string, string>& header
        )->boost::future<HTTP_Response> {
          /*
            implementation of generic REST client
          */
          auto apiCall = [this](
            const string& host,
            const string& verb,
            const string& params,
            const string& data,
            const std::map<string, string>& header
          )->HTTP_Response {
            SimpleWeb::CaseInsensitiveMultimap mm;
            
            for_each(header.begin(), header.end(), [&mm](auto& e) { mm.insert(move(e)); });
            
            HTTP_Response r1; // = make_shared<typename SimpleWeb::ClientBase<T>::Response>();

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
            } catch (const std::exception& e) {              
            }

            return r1;
          };

          return boost::async([&, this]()->shared_ptr<typename SimpleWeb::ClientBase<T>::Response> {
            return apiCall(host, verb, params, data, header);
          });

        };
        
      }); // le fin for_each
    }
    /*
    map of <key, fn(string, string, map<string,string>) => future<HTTP_Response>>
    */
    std::unordered_map <string, std::function<boost::future<HTTP_Response>(const string&, const string&, const std::map<string, string>&)>> methodPromise;
  private:
    string host;
  };
}
