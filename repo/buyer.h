#ifndef BUYER_H
#define BUYER_H

#include "bankCustomer.h"
#include "items.h"

#include <string>
#include <vector>

using namespace std;

class Seller;
struct Transaction;

class Buyer
{
private:
   uint id;
   string name;
   string email;
   BankCustomer customer;

public:
   Buyer(uint id, const string &name, const string &email, double initialDeposit);

   uint getId() const;
   string getName() const;
   string getEmail() const;
   BankCustomer *getCustomer() const;

   void displayBasicInfo() const;
   void buyItem(uint transactionId, Seller *seller, Items &items, uint itemId, int qty, vector<Transaction> &log);
   void listOrders(const vector<Transaction> &allTransactions) const;
   void confirmReceipt(vector<Transaction> &allTransactions);
   void cancelOrder(vector<Transaction> &allTransactions, const vector<Seller> &allSellers);
   void showCashFlow() const;
   void checkSpending() const;
};

#endif