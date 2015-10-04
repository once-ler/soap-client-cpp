#ifndef SOAP_CLIENT_HPP
#define SOAP_CLIENT_HPP

#include <iostream>
#include <locale>
#include <boost/optional.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

#include "client_http.hpp"

#include "plustache/template.hpp"
#include "plustache/plustache_types.hpp"
#include "plustache/context.hpp"

#define BOOST_SPIRIT_THREADSAFE
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

using namespace std;
using namespace boost::property_tree;

typedef SimpleWeb::Client<SimpleWeb::HTTP> HttpClient;

typedef boost::gregorian::date Date;

using namespace std;

namespace SimpleSoap{

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