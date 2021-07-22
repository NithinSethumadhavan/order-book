#pragma once

#include <cstdint>

enum class Side : uint8_t
{
  BID = 1,
  ASK = 0
};

struct Order
{
  using OrderId = uint64_t;
  using Qty = uint32_t;
  using Price = uint64_t;

  bool operator==(const Order& other)
  {
    return orderId == other.orderId;
  }

  OrderId orderId;
  Qty qty;
  Price price;
  Side side;
};
