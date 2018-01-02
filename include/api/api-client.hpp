#pragma once

#ifndef API_CLIENT_H
#define	API_CLIENT_H

#ifndef BOOST_THREAD_PROVIDES_FUTURE
#define BOOST_THREAD_PROVIDES_FUTURE
#endif

#ifndef BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION
#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION
#endif 

#ifndef BOOST_THREAD_PROVIDES_FUTURE_WHEN_ALL_WHEN_ANY
#define BOOST_THREAD_PROVIDES_FUTURE_WHEN_ALL_WHEN_ANY
#endif

#include "Simple-Web-Server/client_http.hpp"
#include "Simple-Web-Server/client_https.hpp"
#include <boost/thread/future.hpp>
#include <iostream>
#include <list>
#include <algorithm>
#include <unordered_map>
#include <functional>
#include <map>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

static const std::list<string> methods{ "GET", "POST", "PUT", "PATCH", "DEL" };

namespace api {
  
  struct IOIO {
    IOIO(string& _method, string& _host, string& _path, string& _data, std::map<string, string>& _headers)
      : method(_method), host(_host), path(_path), data(_data), headers(_headers) {}
    IOIO(const char* _method, const char* _host, const char* _path, const char* _data, std::map<string, string>& _headers)
      : method(_method), host(_host), path(_path), data(_data), headers(_headers) {}
    string method;
    string host;
    string path;
    string data;
    string response;
    std::map<string, string> headers;
  };

  template<typename T>
  class client {    
  public:
    client(string _host, unsigned int s_timeout = 30000, unsigned int r_timeout = 30000, bool _plain_response = true) : host(_host), send_timeout(s_timeout), recv_timeout(r_timeout), plain_response(_plain_response) {

      // for each http/s verb, create a method factory that returns a future
      std::for_each(methods.begin(), methods.end(), [this](const string verb)->void{

        // promise pattern
        this->methodPromise[verb] = [verb, this](string& params, string& data, std::map<string, string>& header)->boost::future<std::string> {
          /*
            implementation of generic REST client
          */
          auto apiCall = [this](const string host, const string verb, string& params, string& data, std::map<string, string>& header, unsigned int send_timeout, unsigned int recv_timeout, bool plain_response)->string {
            json j;
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

              if (plain_response)
                return resp;

              j["request"] = data;
              j["statusCode"] = r1->status_code;
              j["response"] = resp;
            } catch(const SimpleWeb::system_error& e) {
              j["response"] = e.what();
            } catch (const std::exception& e) {
              j["response"] = e.what();
            }
            return j.dump(2);
          };

          return boost::async([&, this]()->string{
            return apiCall(host, verb, params, data, header, send_timeout, recv_timeout, plain_response);
          });

        };
        
      }); // le fin for_each
    }
    client(){};
    ~client(){}
    /*
    map of <key, fn(string, string) => future<string>>
    */
    std::unordered_map <string, std::function<boost::future<std::string>(string&, string&, std::map<string, string>&)>> methodPromise;
  private:
    string host;
    unsigned int send_timeout;
    unsigned int recv_timeout;
    bool plain_response;   
  };

#endif