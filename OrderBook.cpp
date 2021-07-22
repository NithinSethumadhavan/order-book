#include "OrderBook.hpp"

#include <algorithm>
#include <iostream>
#include <sstream>

void OrderBook::addOrder(Order& newOrder)
{
  switch(newOrder.side)
  {
    case Side::BID: addOrderHelper(newOrder, bookBid, bookAsk, std::greater_equal<Order::Price>());
                    break;
    case Side::ASK: addOrderHelper(newOrder, bookAsk, bookBid, std::less_equal<Order::Price>());
                    break;
  }
}

template <typename OwnBook, typename OppBook>
void OrderBook::addOrderHelper(Order& newOrder, OwnBook& ownBook, OppBook& oppBook, 
    std::function<bool(Order::Price, Order::Price)> comp)
{
  if(!oppBook.empty() && comp(newOrder.price, oppBook.begin()->first))
  {
    trade(newOrder, oppBook, comp);
  }
  addOrderToBook(newOrder, ownBook);
}

template<typename BookSide>
void OrderBook::addOrderToBook(Order& newOrder, BookSide& book)
{
  if(newOrder.qty > 0)
  {
    book[newOrder.price].push_back(newOrder);
    auto it = book[newOrder.price].end();
    --it;
    index[newOrder.orderId] = it;
  }
}

void OrderBook::deleteOrder(const Order& newOrder)
{
  switch(newOrder.side)
  {
    case Side::BID : deleteHelper(newOrder, bookBid);
                     break;
    case Side::ASK : deleteHelper(newOrder, bookAsk); 
                     break;
  }
}

template <typename BookSide>
void OrderBook::deleteHelper(const Order& newOrder, BookSide& book)
{
  auto orderIt = index.find(newOrder.orderId);
  if(orderIt == index.end())
    return;

  auto levelIt = book.find(newOrder.price);
  if(levelIt == book.end())
    return;
  
  levelIt->second.erase(orderIt->second);
  if(levelIt->second.empty())
    book.erase(levelIt);
}

void OrderBook::printBook()
{
  std::cout << "=================\nASK\n";
  printHelper(bookAsk.rbegin(), bookAsk.rend());
  std::cout << "------------\n";
  printHelper(bookBid.begin(), bookBid.end());
  std::cout << "BID\n=================\n";
}

template<typename Iter>
void OrderBook::printHelper(Iter begin, Iter end)
{
  for(auto level = begin; level != end; ++level)
  {
    std::cout << level->first << ":";
    for(const auto order: level->second)
    {
      std::cout <<  " " << order.qty;
    }
    std::cout << "\n";
  }
}

template<typename BookSide>
void OrderBook::trade(Order& newOrder, BookSide& bookSide, 
    std::function<bool(Order::Price, Order::Price)> comp)
{
  auto it=bookSide.begin();
  while(newOrder.qty > 0 && comp(newOrder.price, it->first))
  {
    auto& level = it->second;
    while( newOrder.qty > 0 && !level.empty())
    {
      tradeHelper(newOrder, level);
    }

    if(level.empty())
    {
      it = bookSide.erase(it);
    }
  }
}

void OrderBook::tradeHelper(Order& newOrder, OrderBook::Level& level)
{
  auto it = level.begin();
  if(newOrder.qty >= it->qty)
  {
    tradePrinter(it->qty, it->price);
    newOrder.qty -= it->qty;
    index.erase(it->orderId);
    level.erase(it);
  }
  else
  {
    tradePrinter(newOrder.qty, it->price);
    it->qty -= newOrder.qty;
    newOrder.qty = 0;
  }
}

void OrderBook::tradePrinter(Order::Qty qty, Order::Price price)
{
  std::cout << qty << (qty == 1 ? " share" : " shares") << " of XYZ were sold at " << price << " USD\n";
}  

void OrderBook::processOrder(std::string& line)
{
  if(line == "") 
    return;

  if(std::count(line.begin(), line.end(), ',') != 4)
    return;

  std::replace(line.begin(), line.end(), ',', ' ');

  char action;
  char sideC;
  Order newOrder;
  std::istringstream ss(line);

  ss >> action >> newOrder.orderId >> sideC >> newOrder.qty >> newOrder.price;

  if((action != 'A' && action != 'X') || (sideC != 'B' && sideC != 'S'))
    return;

  newOrder.side = sideC == 'B' ? Side::BID : Side::ASK;

  switch(action)
  {
    case 'A': addOrder(newOrder);
              break;
    case 'X': deleteOrder(newOrder);
              break;
  }

}
