#ifndef BANK_H
#define BANK_H

#include "bankCustomer.h"
#include "transaction.h"
#include <vector>

using namespace std;

class Bank
{
private:
   vector<BankCustomer> customers;

public:
   void addCustomer(const BankCustomer &customer);
   BankCustomer *findCustomerById(uint id) const;
   const vector<BankCustomer> &getAllCustomers() const;
   void showAllCustomers();

   void listRecentTransactions(const vector<Transaction> &log, int k_days) const;
   void listDormantAccounts(const vector<Transaction> &log) const;
   void listTopUsersToday(const vector<Transaction> &log, int n) const;

   void listMostActiveBuyersToday(const vector<Transaction> &log, int n) const;
   void listMostActiveSellersToday(const vector<Transaction> &log, int n) const;
};

#endif