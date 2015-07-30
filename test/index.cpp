#include <iostream>
#include "plustache/plustache_types.hpp"
//#include "soap-client.hpp"

//using namespace SimpleSoap;

int main2(int argc, char *argv[]) {
  /**
  //indicate what service to invoke
  const string testHost = "www.w3schools.com";
  const string testUri = "/webservices/tempconvert.asmx";
  const string tplFile = "tpl/tempconvert-test";

  //your context
  PlustacheTypes::ObjectType o;
  o["temp"] = "99";

  //create your client and pass your context
  auto client = Client(testHost, testUri);
  client.compile(tplFile, o);

  //get the result
  auto result = client.send();
  cout << "body" << *result->httpResponse << "\n";
  cout << "status => " << *result->httpStatus << "\n";

  //traverse xml
  auto pt = result->xmlTree;
  auto converted = pt->get<double>("soap:Envelope.soap:Body.FahrenheitToCelsiusResponse.FahrenheitToCelsiusResult");

  cout << std::setprecision(9) << converted << "\n";
**/
  return 0;

}
