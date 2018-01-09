#pragma once

#include <map>
#include <memory>
#include "soap-client.hpp"
#include "web/web-client.hpp"

using namespace std;
using namespace SimpleSoap;

namespace SimpleSoap::api {
  static string version = "0.5.0";
  /*
    client implenmentation
    LOB client modules should derive from this base class
  */
  template<typename socketType>
  class client {
  public:
    client<socketType>(const string& _host,
      const string& _servicePath,
      const std::map<string, string>& _customHeaders = {{"Content-Type", "text/xml"}},
      const string& _user = "",
      const string& _password = ""      
      ) : 
        host(_host),
        servicePath(_servicePath),
        customHeaders(_customHeaders),
        userstr(_user), 
        passwordstr(_password)        
      {

      // create web::client<socketType>
      webClient = make_unique<web::client<socketType>>(host);

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

    auto compile(const string& bodyTpl, const string& rootTpl) {
      auto compiledTemplate = this->generator.render(this->ctx, bodyTpl, rootTpl);
      return move(compiledTemplate);
    }

    auto post(const string& compiledTemplate) {
      auto future = this->webClient->methodPromise["POST"];
      auto pms = future(servicePath, compiledTemplate, customHeaders);
      return pms.get();
    }

    auto get() {
      auto future = this->webClient->methodPromise["GET"];
      auto pms = future(servicePath, "", customHeaders);
      return pms.get();
    }

  protected:
    shared_ptr<Plustache::Context> ctx;
    SimpleSoap::generator<XmlElement::Message> generator;
    unique_ptr<web::client<socketType>> webClient;
    std::map<string, string> customHeaders;
    string host, servicePath, userstr, passwordstr;    
  };
}
