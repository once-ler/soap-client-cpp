#pragma once

#include <map>
#include <memory>
#include "soap-client.hpp"
#include "api/api-client.hpp"

using namespace std;
using namespace SimpleSoap;

namespace SimpleSoap::api {
  static string version = "0.2.0";
  /*
    client implenmentation
    LOB client modules should derive from this base class
  */
  template<typename socketType>
  class client {
  public:
    client<socketType>(const string& _host,
      const string& _servicePath,
      const string& _user = "",
      const string& _password = "",
      std::map<string, string>& _customHeaders = {}
      ) : 
        host(_host),
        servicePath(_servicePath), 
        userstr(_user), 
        passwordstr(_password), 
        customHeaders(_customHeaders)
      {

      // create api::client<socketType>
      apiClient = make_unique<api::client<socketType>>(host);

      // create the context that will be shared by all templates
      ctx = make_shared<Plustache::Context>();

      // store user & password in the context
      (*ctx).add("user", userstr);
      (*ctx).add("password", passwordstr);
      (*ctx).add("host", host);
      (*ctx).add("servicePath", servicePath);
      (*ctx).add("customHeaders", customHeaders);

      //store the template type in the context
      (*ctx).add("socketType", (std::is_same<socketType, SimpleWeb::HTTP>::value ? "HTTP" : "HTTPS"));
    }
    ~client<socketType>(){}
    client() = default;
    
    shared_ptr<Plustache::Context> getContext() {
      return this->ctx;
    }

  protected:
    shared_ptr<Plustache::Context> ctx;
    SimpleSoap::generator<XmlElement::Message> generator;
    unique_ptr<api::client<socketType>> apiClient;
    std::map<string, string> customHeaders;
    string host, servicePath, userstr, passwordstr;    
  };
}
