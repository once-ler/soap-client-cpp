#ifndef SOAP_CLIENT_HPP
#define SOAP_CLIENT_HPP

#include <iostream>
// #include <locale>
// #include <boost/optional.hpp>
// #include <boost/date_time/gregorian/gregorian.hpp>

#include "api/api-client.hpp"
#include "plustache/include/plustache.hxx"

// #include "plustache/template.hpp"
// #include "plustache/plustache_types.hpp"
// #include "plustache/context.hpp"

// #define BOOST_SPIRIT_THREADSAFE
// #include <boost/property_tree/ptree.hpp>
// #include <boost/property_tree/xml_parser.hpp>

using namespace std;
// using namespace boost::property_tree;

// typedef SimpleWeb::Client<SimpleWeb::HTTP> HttpClient;

// typedef boost::gregorian::date Date;

using namespace std;

namespace SimpleSoap {

  namespace XmlElement {
    class Root;
    class Body;
    class Message;
  }

  template<typename xmlelement>
  class base {
  public:
    base(){}
    ~base(){}
    std::string tpl;

    template<typename T>
    string compile(const shared_ptr<T> o){

      // read entire template file 
      std::ifstream file(tpl.c_str());
      std::string tplx((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

      if (tplx.size() == 0){
        return "";
      }

      // compile template with context to create a soap message
      Plustache::template_t t;
      std::string result = t.render(tplx, *o);

      return result;
    }
  };

  // children must implement
  template<typename xmlelement>
  class generator_impl : public base<xmlelement>{};

  /*
    Root
  */
  template<>
  class generator_impl<XmlElement::Root> : public base<XmlElement::Root>{
  public:
    generator_impl(){ tpl = "tpl/root"; }

  };

  /*
    Body
  */
  template<>
  class generator_impl<XmlElement::Body> : public base<XmlElement::Body>{
  public:
    generator_impl(){ tpl = "tpl/body"; }

  };
  
  template<typename xmlelement>
  class generator{
  public:
    generator(){}
    ~generator(){}
  };
  
  template<>
  class generator<XmlElement::Root>{
  public:
    
    std::shared_ptr<std::string> compile(shared_ptr<PlustacheTypes::ObjectType> ctx, std::string& tplPath) {

      generator_impl<XmlElement::Root> elemGen;
      elemGen.tpl = tplPath;
      auto genTpl = elemGen.compile(ctx);
      auto r = make_shared<string>(genTpl);

      return r;
    }

  };

  template<>
  class generator<XmlElement::Body>{
  public:

    std::shared_ptr<std::string> compile(shared_ptr<Plustache::Context> ctx, std::string& tplPath) {

      generator_impl<XmlElement::Body> elemGen;
      elemGen.tpl = tplPath;
      auto genTpl = elemGen.compile(ctx);
      auto r = make_shared<string>(genTpl);

      return r;
    }    

  };

  template<>
  class generator<XmlElement::Message> {

  public:
    shared_ptr<string> render(shared_ptr<Plustache::Context> ctx, const string _path, const string _rootPath) {
      string tplPath(_path);
      durian::generator<durian::XmlElement::Body> gen;
      auto body = gen.compile(ctx, tplPath);

      // create envelope wrapper
      auto rootCtx = make_shared<PlustacheTypes::ObjectType>();
      (*rootCtx)["body"] = *body;
      
      string rootPath(_rootPath);
      durian::generator<durian::XmlElement::Root> rootGen;
      auto message = rootGen.compile(rootCtx, rootPath);

      return message;
    }
  };

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
    // PlustacheTypes::ObjectType user, password, token, response;
    std::map<string, string> customHeaders;
    string host, servicePath, userstr, passwordstr;    
  };

  /***************
   * Old version.
   ****************/
  struct Result {
    //shared_ptr<SimpleWeb::ClientBase<SimpleWeb::HTTP>::Response> httpResponse;
    shared_ptr<string> httpResponse;
    shared_ptr<string> httpStatus;
    shared_ptr<boost::property_tree::ptree> xmlTree;
  } SoapResult;

  class Client {
    string host;
    string querypath;
    string xml;
  public:
    Client(string _host, string _querypath) : host(_host), querypath(_querypath){}
    ~Client(){}
    template<class T>
    int compile(const string tplFile, const T& o){

      //read entire template file 
      std::ifstream file(tplFile.c_str());
      std::string tpl((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

      if (tpl.size() == 0){
        return 1;
      }

      //compile tempplate with context to create a soap message
      Plustache::template_t t;
      xml = t.render(tpl, o);

      return 0;
    }

    shared_ptr<Result> Client::send(){
      //send the compiled soap message to dest
      HttpClient client(host);

      //add appropriate header
      std::map<string, string> header;
      header["Content-Type"] = "text/xml";

      //post the request
      stringstream buf(xml);
      auto r2 = client.request("POST", querypath, buf, header);

      //get the result
      stringstream ss;
      ss << r2->content.rdbuf();
      
      //populate http response
      shared_ptr<string> res(new string(ss.str()));
      shared_ptr<string> sta(new string(r2->status_code));

      //convert the ostream to xml
      shared_ptr<ptree> pt(new ptree());
      try {
        read_xml(ss, *pt);
      }
      catch(exception& e){
        cout << e.what() << endl;
        cout << ss.str() << endl;
      }

      //prepare return result
      shared_ptr<Result> result(new Result());
      
      r2->content.seekg(0, r2->content.beg);
      //result->httpResponse = r2;
      result->httpResponse = res;
      result->httpStatus = sta;
      result->xmlTree = pt;

      return result;
    }
  private:
    
  };

}

/**
@reference
https://akrzemi1.wordpress.com/examples/parsing-xml/
// Copyright 2014 Andrzej Krzemienski.
**/

class DateTranslator {
  typedef boost::date_time::date_facet<Date, char> tOFacet;
  typedef boost::date_time::date_input_facet<Date, char> tIFacet;
  std::locale locale_;

  static std::locale isoDateLocale() {
    std::locale loc;
    loc = std::locale(loc, new tIFacet("%Y-%m-%d"));
    loc = std::locale(loc, new tOFacet("%Y-%m-%d"));
    return loc;
  }

public:
  typedef std::string internal_type;
  typedef Date external_type;

  DateTranslator() : locale_(isoDateLocale()) {}

  boost::optional<external_type> get_value(internal_type const& v) {
    std::istringstream stream(v);
    stream.imbue(locale_);
    external_type vAns;
    if (stream >> vAns) {
      return vAns;
    }
    else {
      return boost::none;
    }
  }

  boost::optional<internal_type> put_value(external_type const& v) {
    std::ostringstream ans;
    ans.imbue(locale_);
    ans << v;
    return ans.str();
  }
};

#endif