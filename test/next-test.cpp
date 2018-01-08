#include "soap-client-api.hpp"

using SimpleSoapClientApi = SimpleSoap::api::client<SimpleWeb::HTTP>;

int main(int argc, char *argv[]) {
  //indicate what service to invoke
  const string testHost = "www.w3schools.com";
  const string testUri = "/webservices/tempconvert.asmx";
  const string rootTpl = "tpl/root";
  const string bodyTpl = "tpl/tempconvert-test";
  
  //your context
  PlustacheTypes::CollectionType c;
  PlustacheTypes::ObjectType o;
  o["temp"] = "99";

  // Create your client and pass your context
  SimpleSoapClientApi client(testHost, testUri);  

  // Pass the context.
  client.getContext()->add("temp", "99");

  auto requestBody = client.compile(bodyTpl, rootTpl);

  auto result = client.post(requestBody);

  cout << result << endl;

  // ------------------------------------------------------------------------
/*

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
*/
  return 0;
}
