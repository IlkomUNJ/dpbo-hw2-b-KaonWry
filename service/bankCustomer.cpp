#include "../repo/bankCustomer.h"
#include "../utils.cpp"

#include <string>
#include <iostream>
#include <iomanip>

BankCustomer::BankCustomer(uint id, const string &name, const string &email, uint bankAccountId, double initialDeposit)
    : id(id), name(name), email(email), bankAccountId(bankAccountId), balance(initialDeposit)
{
   if (initialDeposit > 0)
   {
      history.emplace_back(BankTransactionType::DEPOSIT, initialDeposit, system_clock::now());
   }
}

uint BankCustomer::getId() const { return id; }
std::string BankCustomer::getName() const { return name; }
std::string BankCustomer::getEmail() const { return email; }
uint BankCustomer::getBankAccountId() const { return bankAccountId; }
double BankCustomer::getBalance() const { return balance; }

const vector<BankTransactionRecord> &BankCustomer::getHistory() const { return history; }
void BankCustomer::addBankHistoryRecord(BankTransactionType type, double amount, system_clock::time_point timestamp)
{
   history.emplace_back(type, amount, timestamp);
}

void BankCustomer::withdraw(double amount, system_clock::time_point timestamp)
{
   if (amount <= 0)
   {
      throw runtime_error("Withdrawal amount must be positive.");
   }
   if (amount > balance)
   {
      throw runtime_error("Withdrawal amount exceeds balance.");
   }
   balance -= amount;
   addBankHistoryRecord(BankTransactionType::WITHDRAWAL, amount, timestamp);
}

void BankCustomer::deposit(double amount, system_clock::time_point timestamp)
{
   if (amount <= 0)
   {
      throw runtime_error("Deposit amount must be positive.");
   }
   balance += amount;
   addBankHistoryRecord(BankTransactionType::DEPOSIT, amount, timestamp);
}

void BankCustomer::displayCashFlowHistory(BankCashFlowFilter filter) const
{
   string headerText = "Riwayat Cash Flow ";
   auto now = system_clock::now();
   system_clock::time_point startTime;

   if (filter == BankCashFlowFilter::TODAY)
   {
      headerText += "Hari Ini";
   }
   else
   {
      headerText += "(30 Hari Terakhir)";
      startTime = now - hours(24 * 30);
   }

   printHeader(headerText);
   cout << fixed << setprecision(2);

   bool found = false;
   double totalDeposit = 0;
   double totalWithdrawal = 0;

   for (const auto &record : history)
   {
      bool timeMatch = false;
      if (filter == BankCashFlowFilter::TODAY)
      {
         timeMatch = isToday(record.timestamp);
      }
      else
      {
         timeMatch = (record.timestamp >= startTime);
      }

      if (timeMatch)
      {
         found = true;
         if (record.type == BankTransactionType::DEPOSIT)
         {
            cout << "[+] Pemasukan  : Rp " << record.amount << "\n";
            totalDeposit += record.amount;
         }
         else
         {
            cout << "[-] Pengeluaran: Rp " << record.amount << "\n";
            totalWithdrawal += record.amount;
         }
      }
   }

   if (!found)
   {
      cout << "Tidak ada aktivitas pada periode ini.\n";
   }
   else
   {
      cout << "----------------------------------------\n";
      cout << "Total Pemasukan  : Rp " << totalDeposit << "\n";
      cout << "Total Pengeluaran: Rp " << totalWithdrawal << "\n";
   }
   cout << defaultfloat;
}