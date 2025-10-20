#ifndef SELLER_H
#define SELLER_H

#include "items.h"
#include <string>

using namespace std;

class Buyer;
class BankCustomer;
struct Transaction;

class Seller
{
private:
   Buyer *buyer;
   string storeName;
   string storeAddress;
   string storeEmail;
   Items storeItems;

public:
   Seller(Buyer *buyer, const string &storeName, const string &storeAddress, const string &storeEmail);

   Buyer *getBuyer() const;
   string getStoreName() const;
   string getStoreAddress() const;
   string getStoreEmail() const;
   BankCustomer *getCustomerAccount() const;
   Items *getStoreItems();

   void registerNewItem();
   void updateExistingItem();
   void showTopKItems(const vector<Transaction> &allTransactions) const;
   void showLoyalCustomers(const vector<Transaction> &allTransactions) const;
   void listPendingOrders(const vector<Transaction> &allTransactions) const;
};

#endif