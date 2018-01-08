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
#include "json11/json11.hpp"

using namespace std;
using namespace json11;

static const std::list<string> methods{ "GET", "POST", "PUT", "PATCH", "DEL" };

namespace web {
  template<typename T>
  class client {    
  public:
    client(const string& _host, bool _plain_response = true) : host(_host), plain_response(_plain_response) {

      // for each http/s verb, create a method factory that returns a future
      std::for_each(methods.begin(), methods.end(), [this](const string verb)->void{

        // promise pattern
        this->methodPromise[verb] = [verb, this](
          const string& params,
          const string& data,
          const std::map<string, string>& header,
          bool plain_response
        )->boost::future<std::string> {
          /*
            implementation of generic REST client
          */
          auto apiCall = [this](
            const string& host,
            const string& verb,
            const string& params,
            const string& data,
            const std::map<string, string>& header,
            bool plain_response
          )->string {
            SimpleWeb::CaseInsensitiveMultimap mm;
            
            for_each(header.begin(), header.end(), [&mm](auto& e) { mm.insert(move(e)); });
            
            try {
              SimpleWeb::Client<T> client(host);
              
              shared_ptr<typename SimpleWeb::ClientBase<T>::Response> r1;
              string resp;

              if (data.size() > 0){
                stringstream ss;
                ss << data;
                r1 = client.request(verb, params, ss, mm);
              } else {
                r1 = client.request(verb, params, "", mm);
              }
              
              ostringstream oss;
              oss << r1->content.rdbuf();
              resp = oss.str();

              // if (plain_response)
                return resp;

              // j["request"] = data;
              // j["statusCode"] = r1->status_code;
              // j["response"] = resp;
            } catch(const SimpleWeb::system_error& e) {
              // j["response"] = e.what();
            } catch (const std::exception& e) {
              // j["response"] = e.what();
            }

            return ""; 
            // Json js = j;
            // return js.dump();
          };

          return boost::async([&, this]()->string{
            return apiCall(host, verb, params, data, header, plain_response);
          });

        };
        
      }); // le fin for_each
    }
    // client(){};
    ~client(){}
    /*
    map of <key, fn(string, string) => future<string>>
    */
    std::unordered_map <string, std::function<boost::future<std::string>(const string&, const string&, const std::map<string, string>&, bool)>> methodPromise;
  private:
    string host;
    bool plain_response;   
  };
}
