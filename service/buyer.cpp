#include "../repo/buyer.h"
#include "../repo/seller.h"
#include "../database.cpp"
#include "../utils.cpp"

#include <stdexcept>
#include <iostream>
#include <limits>

using namespace std;
using namespace chrono;

Buyer::Buyer(uint id, const string &name, const string &email, double initialDeposit)
    : id(id), name(name), email(email),
      customer(id, name, email, id + 1000, initialDeposit)
{
}

uint Buyer::getId() const { return id; }
string Buyer::getName() const { return name; }
string Buyer::getEmail() const { return email; }

BankCustomer *Buyer::getCustomer() const { return const_cast<BankCustomer *>(&customer); }

void Buyer::buyItem(uint transactionId, Seller *seller, Items &items, uint itemId, int qty, vector<Transaction> &log)
{
   Item *item = items.findItemById(itemId);
   if (!item)
   {
      throw runtime_error("Item not found");
   }

   if (item->getQuantity() < qty)
   {
      throw runtime_error("Stock not enough");
   }

   double totalPrice = item->getPrice() * qty;
   if (customer.getBalance() < totalPrice)
   {
      throw runtime_error("Not enough balance");
   }

   auto transactionTimestamp = system_clock::now();

   customer.withdraw(totalPrice, transactionTimestamp);
   seller->getCustomerAccount()->deposit(totalPrice, transactionTimestamp);

   item->decreaseQuantity(qty);

   log.emplace_back(
       transactionId,
       this->id,
       seller->getBuyer()->getId(),
       itemId,
       item->getName(),
       seller->getStoreName(),
       qty,
       totalPrice);
   log.back().transactionDate = transactionTimestamp;
}

void Buyer::listOrders(const vector<Transaction> &allTransactions) const
{
   printHeader("Riwayat Pesanan Anda");

   cout << "Menampilkan semua pesanan Anda:\n\n";

   bool hasOrders = false;
   for (const auto &record : allTransactions)
   {
      if (record.buyerId == id)
      {
         cout << "Toko     : " << record.sellerStoreName << "\n";
         cout << "Item     : " << record.itemName << " (x" << record.quantity << ")\n";
         cout << "Total    : Rp " << record.totalPrice << "\n";
         cout << "Status   : " << statusToString(record.status) << "\n";
         cout << "----------------------------------------\n";
         hasOrders = true;
      }
   }
   if (!hasOrders)
   {
      cout << "Anda belum memiliki riwayat pesanan.\n";
   }

   cout << "\nTekan [Enter] untuk kembali...";
   cin.get();
}

void Buyer::checkSpending() const
{
   printHeader("Analisis Pengeluaran");

   int k_days;
   cout << "Cek total pengeluaran dalam (k) hari terakhir.\n";
   cout << "Masukkan jumlah hari (k) (ketik 0 untuk batal): ";
   cin >> k_days;
   if (k_days == 0)
   {
      Database::globalMessage = "Pencarian dibatalkan.";
      cin.ignore(numeric_limits<streamsize>::max(), '\n');
      return;
   }

   double totalSpending = 0;
   auto now = system_clock::now();
   auto k_days_ago = now - hours(24 * k_days);

   for (const auto &record : Database::transactionLog)
   {
      if (record.buyerId == id && record.transactionDate >= k_days_ago)
      {
         totalSpending += record.totalPrice;
      }
   }
   cout << "\nTotal pengeluaran Anda dalam " << k_days << " hari terakhir adalah: Rp " << totalSpending << "\n";

   cout << "\nTekan [Enter] untuk kembali...";
   cin.ignore(numeric_limits<streamsize>::max(), '\n');
   cin.get();
}

void Buyer::confirmReceipt(vector<Transaction> &allTransactions)
{
   printHeader("Konfirmasi Penerimaan Barang");

   bool hasPaidOrders = false;
   cout << "Pesanan Anda yang Menunggu Konfirmasi (Status PAID):\n";
   cout << "--------------------------------------------------------\n";
   for (const auto &record : allTransactions)
   {
      if (record.buyerId == id && record.status == PAID)
      {
         cout << "ID Transaksi: " << record.transactionId << "\n";
         cout << "Item        : " << record.itemName << " (x" << record.quantity << ")\n";
         cout << "Dari Toko   : " << record.sellerStoreName << "\n\n";
         hasPaidOrders = true;
      }
   }

   if (!hasPaidOrders)
   {
      cout << "Anda tidak memiliki pesanan yang menunggu konfirmasi.\n";
      return;
   }
   cout << "--------------------------------------------------------\n";

   uint idToConfirm;
   cout << "Masukkan ID Transaksi yang ingin dikonfirmasi (ketik 0 untuk batal): ";
   cin >> idToConfirm;
   cin.ignore(numeric_limits<streamsize>::max(), '\n');

   if (idToConfirm == 0)
   {
      Database::globalMessage = "Aksi dibatalkan.";
      return;
   }

   for (auto &record : allTransactions)
   {
      if (record.transactionId == idToConfirm && record.buyerId == id && record.status == PAID)
      {
         record.status = COMPLETED;
         Database::globalMessage = "Terima kasih! Transaksi #" + to_string(idToConfirm) + " telah ditandai selesai.";
         return;
      }
   }
   Database::globalMessage = "Error: ID Transaksi tidak ditemukan atau sudah dikonfirmasi.";
}

void Buyer::cancelOrder(vector<Transaction> &allTransactions, const vector<Seller> &allSellers)
{
   printHeader("Batalkan Pesanan");

   bool hasPaidOrders = false;
   cout << "Pesanan Anda yang bisa dibatalkan (Status PAID):\n";
   cout << "--------------------------------------------------------\n";
   for (const auto &record : allTransactions)
   {
      if (record.buyerId == this->id && record.status == PAID)
      {
         cout << "ID Transaksi: " << record.transactionId << "\n";
         cout << "Item        : " << record.itemName << " (x" << record.quantity << ")\n";
         cout << "Dari Toko   : " << record.sellerStoreName << "\n\n";
         hasPaidOrders = true;
      }
   }

   if (!hasPaidOrders)
   {
      cout << "Anda tidak memiliki pesanan yang bisa dibatalkan.\n";
      return;
   }
   cout << "--------------------------------------------------------\n";

   uint idToCancel;
   cout << "Masukkan ID Transaksi yang ingin dibatalkan (ketik 0 untuk batal): ";
   cin >> idToCancel;
   cin.ignore(numeric_limits<streamsize>::max(), '\n');

   if (idToCancel == 0)
   {
      Database::globalMessage = "Aksi dibatalkan.";
      return;
   }

   for (auto &record : allTransactions)
   {
      if (record.transactionId == idToCancel && record.buyerId == id && record.status == PAID)
      {
         Seller *sellerOfItem = nullptr;
         for (const auto &s : allSellers)
         {
            if (s.getBuyer()->getId() == record.sellerId)
            {
               sellerOfItem = const_cast<Seller *>(&s);
               break;
            }
         }

         if (sellerOfItem)
         {
            Item *itemToRestock = sellerOfItem->getStoreItems()->findItemById(record.itemId);
            if (itemToRestock)
            {
               itemToRestock->increaseQuantity(record.quantity);
            }
         }

         auto cancelTimestamp = system_clock::now();
         BankCustomer *buyerAccount = getCustomer();
         if (sellerOfItem)
         {
            BankCustomer *sellerAccount = sellerOfItem->getCustomerAccount();
            sellerAccount->withdraw(record.totalPrice, cancelTimestamp);
            buyerAccount->deposit(record.totalPrice, cancelTimestamp);
         }

         record.status = CANCELED;
         record.transactionDate = cancelTimestamp;

         Database::globalMessage = "Transaksi #" + to_string(idToCancel) + " berhasil dibatalkan dan dana telah dikembalikan.";
         return;
      }
   }

   Database::globalMessage = "Error: ID Transaksi tidak ditemukan atau statusnya bukan PAID.";
}

void Buyer::displayBasicInfo() const
{
   cout << "ID       : " << id << "\n";
   cout << "Nama     : " << name << "\n";
   cout << "Email    : " << email << "\n";
   cout << "Saldo    : Rp " << customer.getBalance() << "\n";
}

void Buyer::showCashFlow() const
{
   int choice;
   while (true)
   {
      clearScreen();
      printHeader("Lihat Cash Flow Akun Bank");
      Database::displayGlobalMessage();

      cout << "Pilih periode waktu:\n";
      cout << "1. Hari Ini\n";
      cout << "2. 30 Hari Terakhir\n";
      cout << "0. Kembali ke Menu Pembeli\n";
      cout << "----------------------------------------\n";
      cout << "Pilihan Anda: ";

      cin >> choice;

      if (cin.fail())
      {
         cin.clear();
         Database::globalMessage = "Input tidak valid. Harap masukkan angka.";
         choice = -1;
      }
      cin.ignore(numeric_limits<streamsize>::max(), '\n');

      BankCashFlowFilter filter;
      bool shouldDisplay = false;

      switch (choice)
      {
      case 1:
         filter = BankCashFlowFilter::TODAY;
         shouldDisplay = true;
         break;
      case 2:
         filter = BankCashFlowFilter::LAST_MONTH;
         shouldDisplay = true;
         break;
      case 0:
         return;
      default:
         if (choice != -1)
         {
            Database::globalMessage = "Pilihan periode tidak valid.";
         }
         continue;
      }

      if (shouldDisplay)
      {
         clearScreen();
         customer.displayCashFlowHistory(filter);

         cout << "\nTekan [Enter] untuk kembali ke pilihan periode...";
         cin.get();
      }
   }
}