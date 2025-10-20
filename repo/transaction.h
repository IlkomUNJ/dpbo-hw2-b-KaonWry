#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <chrono>
#include <string>

using namespace std;
using namespace chrono;

enum OrderStatus
{
  PAID,
  COMPLETED,
  CANCELED
};

struct Transaction
{
  uint transactionId;
  uint buyerId;
  uint sellerId;
  uint itemId;

  string itemName;
  string sellerStoreName;

  int quantity;
  double totalPrice;

  system_clock::time_point transactionDate;
  OrderStatus status;

  Transaction(uint tId, uint bId, uint sId, uint iId, const string &iName, const string &sName, int qty, double price)
      : transactionId(tId), buyerId(bId), sellerId(sId), itemId(iId),
        itemName(iName), sellerStoreName(sName),
        quantity(qty), totalPrice(price),
        transactionDate(system_clock::now()),
        status(PAID)
  {
  }
};

#endif