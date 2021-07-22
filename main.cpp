#include <fstream>
#include <iostream>
#include <string>

#include "OrderBook.hpp"

int main(int argc, char* argv[])
{
  if(argc < 2)
  {
    std::cerr << "Please enter file name as arg" << std::endl;
    return 1;
  }

  if(argc > 2)
  {
    std::cout << "Warning: ignoring additional args." << std::endl;
  }

  std::ifstream orders{ argv[1] };

  if(!orders)
  {
    std::cerr << "Please enter valid file name" << std::endl;
    return 1;
  }

  OrderBook orderBook;
  std::string orderStr;

  while(orders)
  {
    std::getline(orders, orderStr);
    orderBook.processOrder(orderStr);
  }

  orderBook.printBook();

  return 0;
}
  
