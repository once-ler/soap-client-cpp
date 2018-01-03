#ifndef SOAP_CLIENT_HPP
#define SOAP_CLIENT_HPP

#include <iostream>
// #include <locale>
// #include <boost/optional.hpp>
// #include <boost/date_time/gregorian/gregorian.hpp>

// #include "api/api-client.hpp"
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
      SimpleSoap::generator<SimpleSoap::XmlElement::Body> gen;
      auto body = gen.compile(ctx, tplPath);

      // create envelope wrapper
      auto rootCtx = make_shared<PlustacheTypes::ObjectType>();
      (*rootCtx)["body"] = *body;
      
      string rootPath(_rootPath);
      SimpleSoap::generator<SimpleSoap::XmlElement::Root> rootGen;
      auto message = rootGen.compile(rootCtx, rootPath);

      return message;
    }
  };

  /***************
   * Old version.
   ****************/
  /*
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
  */
}

#endif
