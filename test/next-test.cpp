#include "soap-client-api.hpp"

using SimpleSoapClientApi = SimpleSoap::api::client<SimpleWeb::HTTPS>;

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
  SimpleSoapClientApi soapclient(testHost, testUri);  

  // Pass the context.
  soapclient.getContext()->add("temp", "99");

  auto requestBody = soapclient.compile(bodyTpl, rootTpl);

  auto result = soapclient.post(requestBody);

  if (result)
    cout << result->content.string() << endl;

  return 0;
}
