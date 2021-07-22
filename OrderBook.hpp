#pragma once

#include <functional>
#include <list>
#include <map>
#include <unordered_map>

#include "Utils.hpp"

class OrderBook
{
  using Level = std::list<Order>;
  using BookAsk = std::map<Order::Price, Level>;
  using BookBid = std::map<Order::Price, Level, std::greater<int>>;
  using Index = std::unordered_map<Order::OrderId, Level::iterator>;

public:

  void processOrder(std::string&);
  void addOrder(Order&);
  void deleteOrder(const Order&);
  void printBook();

private:  
  template<typename SameSide, typename OppSide>
  void addOrderHelper(Order&, SameSide&, OppSide&, std::function<bool(Order::Price,Order::Price)>); 

  template<typename BookSide>
  void addOrderToBook(Order&, BookSide&);

  template<typename BookSide>
  void trade(Order&, BookSide&, std::function<bool(Order::Price, Order::Price)>);

  void tradeHelper(Order& newOrder, Level&);
  
  void tradePrinter(Order::Qty, Order::Price);

  template<typename BookSide>
  void deleteHelper(const Order& newOrder, BookSide&);

  template<typename Iter>
  void printHelper(Iter, Iter);

  Index index;
  BookAsk bookAsk;
  BookBid bookBid;

};
