#include <iostream>
#include "plustache/plustache_types.hpp"
#include "soap-client.hpp"

using namespace SimpleSoap;

namespace allentown{

  /**
    dummy classes to make code look readable
  **/
  namespace SoapAction { class GetCsv;  class GetXml; };
  typedef SoapAction::GetCsv GetCsv;
  typedef SoapAction::GetXml GetXml;

  template<typename method_type>
  class Base {
    public: 
      Base(){}
      ~Base(){}
      string host = "clcwcdcdvm009";
      string uri = "/EAM.Outbound.Webservices/OutboundService.asmx";
      string tpl;
      
      int compileTemplate(){
        //create your client and pass your context
        client = unique_ptr<SimpleSoap::Client>(new SimpleSoap::Client(host, uri));
        auto rv = client->compile(tpl, o);
        return rv;
      }
    
      void send() {
        //get the result
        result = client->send();
        pt = result->xmlTree;

        cout << "body" << *result->httpResponse << "\n";
        cout << "status => " << *result->httpStatus << "\n";
      }

      shared_ptr<SimpleSoap::Result> getResult() {
        return result;
      }

      shared_ptr<boost::property_tree::ptree> getTree() {
        return pt;
      }

    private:
      unique_ptr<SimpleSoap::Client> client;
      PlustacheTypes::ObjectType o;
      shared_ptr<SimpleSoap::Result> result;
      shared_ptr<boost::property_tree::ptree> pt;

  };

  template<typename method_type>
  class Client : public Base<method_type> {};

  template<>
  class Client<GetCsv> : public Base<GetCsv>{
      
  public:
    //context custom to GetCsv method
    string fromDate;
    string toDate;

    //2 constructors
    //note tpl is inherited from Base
    Client(){ tpl = "tpl/get-csv"; }
    Client(string _fromDate, string _toDate):fromDate(_fromDate), toDate(_toDate) {
      tpl = "tpl/get-csv";
    }
  };

};

int main(int argc, char *argv[]) {

  allentown::Client<allentown::GetCsv> client("01/01/2015","07/01/2015");

  auto error = client.compileTemplate();

  if (!error){
    client.send();

    auto pt = client.getTree();

    auto traversed = pt->get<string>("soap:Envelope.soap:Body.GetCsvResponse.GetCsvResult");
  
    ofstream ofs("out.csv");
    ofs << traversed << "\n";
    ofs.close();
  }
  else {
    cout << "file not found\n";
  }

  return 0;

}
