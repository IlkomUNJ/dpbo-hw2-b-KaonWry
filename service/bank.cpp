#include "../repo/bank.h"
#include "../repo/bankCustomer.h"
#include "../utils.cpp"

#include <iostream>
#include <map>
#include <algorithm>
#include <vector>

using namespace std;

void Bank::addCustomer(const BankCustomer &customer)
{
   customers.push_back(customer);
}

BankCustomer *Bank::findCustomerById(uint id) const
{
   for (auto &c : const_cast<vector<BankCustomer> &>(customers))
   {
      if (c.getId() == id)
         return &c;
   }
   return nullptr;
}

const vector<BankCustomer> &Bank::getAllCustomers() const
{
   return customers;
}

void Bank::showAllCustomers()
{
   printHeader("Daftar Semua Nasabah Bank");

   for (const auto &c : customers)
   {
      cout << "Customer ID: " << c.getId()
           << ", Nama: " << c.getName()
           << ", BankID: " << maskBankId(c.getBankAccountId())
           << ", Saldo: Rp " << c.getBalance()
           << endl;
   }
}

void Bank::listRecentTransactions(const vector<Transaction> &log, int k_days) const
{
   printHeader("Transaksi Dalam " + to_string(k_days) + " Hari Terakhir");

   auto now = system_clock::now();
   auto k_days_ago = now - hours(24 * k_days);
   bool found = false;

   for (const auto &record : log)
   {
      if (record.transactionDate >= k_days_ago)
      {
         BankCustomer *buyer = findCustomerById(record.buyerId);
         BankCustomer *seller = findCustomerById(record.sellerId);

         if (buyer && seller)
         {
            cout << "Pembeli: " << buyer->getName()
                 << " (Bank ID: " << maskBankId(buyer->getBankAccountId()) << ")\n";

            cout << "Penjual: " << seller->getName()
                 << " (Toko: " << record.sellerStoreName
                 << ", Bank ID: " << maskBankId(seller->getBankAccountId()) << ")\n";

            cout << "Detail : " << record.itemName << " (x" << record.quantity << ") seharga Rp " << record.totalPrice << "\n";
            cout << "Status : " << statusToString(record.status) << "\n";
            cout << "----------------------------------------\n";
            found = true;
         }
      }
   }

   if (!found)
   {
      cout << "Tidak ada transaksi dalam " << k_days << " hari terakhir.\n";
   }
}

void Bank::listDormantAccounts(const vector<Transaction> &log) const
{
   printHeader("Akun Dormant (Tidak Aktif > 30 Hari)");

   map<uint, system_clock::time_point> lastActivity;

   for (const auto &record : log)
   {
      if (lastActivity.find(record.buyerId) == lastActivity.end() || record.transactionDate > lastActivity[record.buyerId])
      {
         lastActivity[record.buyerId] = record.transactionDate;
      }
      if (lastActivity.find(record.sellerId) == lastActivity.end() || record.transactionDate > lastActivity[record.sellerId])
      {
         lastActivity[record.sellerId] = record.transactionDate;
      }
   }

   auto now = system_clock::now();
   auto one_month_ago = now - hours(24 * 30);
   bool found = false;

   for (const auto &customer : customers)
   {
      bool isDormant = false;
      if (lastActivity.find(customer.getId()) == lastActivity.end())
      {
         isDormant = true;
      }
      else
      {
         if (lastActivity.at(customer.getId()) < one_month_ago)
         {
            isDormant = true;
         }
      }

      if (isDormant)
      {
         cout << "ID: " << customer.getId()
              << ", Nama: " << customer.getName()
              << ", Email: " << customer.getEmail()
              << ", BankID: " << maskBankId(customer.getBankAccountId())
              << ", Status: Dormant" << "\n";
         found = true;
      }
   }

   if (!found)
   {
      cout << "Tidak ada akun yang dormant saat ini.\n";
   }
}

void Bank::listTopUsersToday(const vector<Transaction> &log, int n) const
{
   printHeader("Top " + to_string(n) + " Pengguna Aktif Hari Ini");

   map<uint, int> userTransactionCount;

   for (const auto &record : log)
   {
      if (isToday(record.transactionDate))
      {
         userTransactionCount[record.buyerId]++;
         userTransactionCount[record.sellerId]++;
      }
   }

   if (userTransactionCount.empty())
   {
      cout << "Tidak ada aktivitas transaksi hari ini.\n";
      return;
   }

   vector<pair<uint, int>> sortedUsers(userTransactionCount.begin(), userTransactionCount.end());
   sort(sortedUsers.begin(), sortedUsers.end(), [](const auto &a, const auto &b)
        { return a.second > b.second; });

   int count = 0;
   for (const auto &pair : sortedUsers)
   {
      if (count++ >= n)
         break;
      BankCustomer *customer = const_cast<Bank *>(this)->findCustomerById(pair.first);
      if (customer)
      {
         cout << count << ". " << customer->getName() << " (ID: " << customer->getId()
              << ") - Transaksi: " << pair.second << "\n";
      }
   }
}

void Bank::listMostActiveBuyersToday(const vector<Transaction> &log, int n) const
{
   printHeader("Top " + to_string(n) + " Pembeli Aktif Hari Ini (by Transaksi)");

   map<uint, int> buyerActivityCount;

   for (const auto &record : log)
   {
      if (isToday(record.transactionDate))
      {
         buyerActivityCount[record.buyerId]++;
      }
   }

   if (buyerActivityCount.empty())
   {
      cout << "Tidak ada aktivitas pembelian hari ini.\n";
      return;
   }

   vector<pair<uint, int>> sortedBuyers(buyerActivityCount.begin(), buyerActivityCount.end());
   sort(sortedBuyers.begin(), sortedBuyers.end(), [](const auto &a, const auto &b)
        { return a.second > b.second; });

   int count = 0;
   for (const auto &pair : sortedBuyers)
   {
      if (count++ >= n)
         break;

      BankCustomer *customer = findCustomerById(pair.first);
      if (customer)
      {
         cout << count << ". " << customer->getName() << " (ID: " << customer->getId()
              << ") - Jumlah Pembelian: " << pair.second << "\n";
      }
   }
}
void Bank::listMostActiveSellersToday(const vector<Transaction> &log, int n) const
{
   printHeader("Top " + to_string(n) + " Penjual Aktif Hari Ini (by Transaksi)");

   map<uint, int> sellerActivityCount;

   for (const auto &record : log)
   {
      if (isToday(record.transactionDate))
      {
         sellerActivityCount[record.sellerId]++;
      }
   }

   if (sellerActivityCount.empty())
   {
      cout << "Tidak ada aktivitas penjualan hari ini.\n";
      return;
   }

   vector<pair<uint, int>> sortedSellers(sellerActivityCount.begin(), sellerActivityCount.end());
   sort(sortedSellers.begin(), sortedSellers.end(), [](const auto &a, const auto &b)
        { return a.second > b.second; });

   int count = 0;
   for (const auto &pair : sortedSellers)
   {
      if (count++ >= n)
         break;

      BankCustomer *customer = findCustomerById(pair.first);
      if (customer)
      {
         cout << count << ". " << customer->getName() << " (ID: " << customer->getId()
              << ") - Jumlah Penjualan: " << pair.second << "\n";
      }
   }
}