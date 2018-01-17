#ifndef SOAP_CLIENT_HPP
#define SOAP_CLIENT_HPP

#include <iostream>
#include "plustache/include/plustache.hxx"

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
  class generator_impl<XmlElement::Body> : public base<XmlElement::Body> {
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
    
    std::string compile(shared_ptr<PlustacheTypes::ObjectType> ctx, std::string& tplPath) {
      generator_impl<XmlElement::Root> elemGen;
      elemGen.tpl = tplPath;
      auto genTpl = elemGen.compile(ctx);      
      return move(genTpl);
    }

  };

  template<>
  class generator<XmlElement::Body>{
  public:

    std::string compile(shared_ptr<Plustache::Context> ctx, std::string& tplPath) {
      generator_impl<XmlElement::Body> elemGen;
      elemGen.tpl = tplPath;
      auto genTpl = elemGen.compile(ctx);
      return move(genTpl);
    }    

  };

  template<>
  class generator<XmlElement::Message> {

  public:
    string render(shared_ptr<Plustache::Context> ctx, const string _path, const string _rootPath) {
      string tplPath(_path);
      SimpleSoap::generator<SimpleSoap::XmlElement::Body> gen;
      auto body = gen.compile(ctx, tplPath);

      // create envelope wrapper
      auto rootCtx = make_shared<PlustacheTypes::ObjectType>();
      (*rootCtx)["body"] = body;
      
      string rootPath(_rootPath);
      SimpleSoap::generator<SimpleSoap::XmlElement::Root> rootGen;
      auto message = rootGen.compile(rootCtx, rootPath);

      return move(message);
    }
  };

}

#endif
