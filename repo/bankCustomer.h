#ifndef BANK_CUSTOMER_H
#define BANK_CUSTOMER_H

#include <iostream>
#include <string>
#include <vector>
#include <chrono>

using namespace std;
using namespace chrono;

enum BankTransactionType
{
   DEPOSIT,
   WITHDRAWAL
};

enum class BankCashFlowFilter
{
   TODAY,
   LAST_MONTH
};

struct BankTransactionRecord
{
   BankTransactionType type;
   double amount;
   system_clock::time_point timestamp;

   BankTransactionRecord(BankTransactionType t, double a, system_clock::time_point ts)
       : type(t), amount(a), timestamp(ts) {}
};

class BankCustomer
{
private:
   uint id;
   string name;
   string email;
   uint bankAccountId;
   double balance;
   vector<BankTransactionRecord> history;

public:
   BankCustomer(uint id, const string &name, const string &email, uint bankAccountId, double initialDeposit);

   BankCustomer() = default;

   uint getId() const;
   string getName() const;
   string getEmail() const;
   uint getBankAccountId() const;
   double getBalance() const;

   const vector<BankTransactionRecord> &getHistory() const;
   void addBankHistoryRecord(BankTransactionType type, double amount, system_clock::time_point timestamp);

   void withdraw(double amount, system_clock::time_point timestamp = system_clock::now());
   void deposit(double amount, system_clock::time_point timestamp = system_clock::now());
   void displayCashFlowHistory(BankCashFlowFilter filter) const;
};

#endif