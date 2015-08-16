#include "test-bcp.hpp"

int main(int argc, char *argv[]) {
  int rt;
  
  //1. connect
  rt = bcpTest::connect();

  if (rt){
    bcpTest::doexit(1);
    return 1;
  }

  //2. create database + table for this example
  bcpTest::createDatabase();

  //3. some maintenance before bulkcopy
  rt = bcpTest::prepareBulkcopy();
  if (rt){
    bcpTest::doexit(1);
    return 1;
  }

  //4. bulkcopy!
  rt = bcpTest::bulkCopy();
  if (rt){
    bcpTest::doexit(1);
    return 1;
  }

  //5. drop database + table for this example
  bcpTest::dropDatabase();

  //success
  bcpTest::doexit(0);

  return 0;
}