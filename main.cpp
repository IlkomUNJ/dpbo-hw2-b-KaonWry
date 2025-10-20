#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include <map>
#include <algorithm>
#include <cctype>
#include "repo/bank.h"
#include "repo/buyer.h"
#include "repo/seller.h"
#include "repo/items.h"
#include "repo/transaction.h"
#include "utils.cpp"
#include "database.cpp"
#include "serialization.cpp"

using namespace std;
using namespace chrono;

void handlePurchaseItem()
{
   clearScreen();
   printHeader("Purchase Item");

   if (Database::sellers.empty())
   {
      Database::globalMessage = "Sorry, no sellers available.";
      return;
   }

   cout << "Available items:\n";
   cout << "--------------------------------------------------------\n";
   for (auto &seller : Database::sellers)
   {
      if (!seller.getStoreItems()->getItems().empty())
      {
         cout << "Store: " << seller.getStoreName() << "\n";
         seller.getStoreItems()->showAllItems();
         cout << "\n";
      }
   }
   cout << "--------------------------------------------------------\n";

   uint itemId;
   int quantity;

   cout << "Insert Item ID to purchase (type 0 to cancel): ";
   cin >> itemId;
   if (itemId == 0)
   {
      Database::globalMessage = "Purchase canceled.";
      cin.ignore(numeric_limits<streamsize>::max(), '\n');
      return;
   }
   cout << "Insert Quantity: ";
   cin >> quantity;

   Seller *targetSeller = nullptr;
   Items *targetItems = nullptr;

   for (auto &seller : Database::sellers)
   {
      Item *item = seller.getStoreItems()->findItemById(itemId);
      if (item)
      {
         targetSeller = &seller;
         targetItems = seller.getStoreItems();
         break;
      }
   }

   if (!targetSeller)
   {
      Database::globalMessage = "Item with ID " + to_string(itemId) + " not found in any store.";
      return;
   }

   try
   {
      uint newTransactionId = Database::nextTransactionId++;
      Database::loggedInBuyer->buyItem(newTransactionId, targetSeller, *targetItems, itemId, quantity, Database::transactionLog);

      Database::globalMessage = "Purchase successful! Your Transaction ID: " + to_string(newTransactionId);
   }
   catch (const runtime_error &e)
   {
      Database::globalMessage = "Error: " + string(e.what());
   }
}

// =======================================================
// Handler action
// =======================================================

void handleUpgradeToSeller()
{
   if (!Database::loggedInBuyer)
   {
      Database::globalMessage = "Error: No user is logged in.";
      return;
   }
   if (Database::loggedInSeller)
   {
      Database::globalMessage = "Your account is already a Seller!";
      return;
   }

   clearScreen();
   printHeader("Upgrade Account to Seller");

   string storeName, storeAddress, storeEmail;
   cout << "Data Buyer: " << Database::loggedInBuyer->getName() << "\n";
   cout << "Please complete your store information.\n\n";

   cout << "Store Name (type 0 to cancel): ";
   getline(cin, storeName);
   if (storeName == "0")
   {
      Database::globalMessage = "Registration canceled.";
      return;
   }

   cout << "Store Address  : ";
   getline(cin, storeAddress);

   cout << "Store Email    : ";
   getline(cin, storeEmail);

   Database::sellers.emplace_back(Database::loggedInBuyer, storeName, storeAddress, storeEmail);
   Database::loggedInSeller = &Database::sellers.back();

   Database::globalMessage = "Upgrade successful! You are now a Seller.";
}

void handleCheckStatus()
{
   clearScreen();
   printHeader("Account Status");

   Database::loggedInBuyer->displayBasicInfo();
   cout << "Status   : ";
   if (Database::loggedInSeller)
   {
      cout << "Seller & Buyer\n";
      cout << "Store Name: " << Database::loggedInSeller->getStoreName() << "\n";
   }
   else
   {
      cout << "Buyer\n";
   }

   cout << "\nPress [Enter] to return...";
   cin.get();
}

void handleListRecentBankTransactions()
{
   printHeader("List Recent Bank Transactions");

   int k_days;
   cout << "Show transactions in the last (k) days.\n";
   cout << "Enter the number of days (k) (type 0 to cancel): ";
   cin >> k_days;
   cin.ignore(numeric_limits<streamsize>::max(), '\n');

   if (k_days == 0)
   {
      Database::globalMessage = "Operation canceled.";
      return;
   }
   if (k_days < 0)
   {
      Database::globalMessage = "Number of days cannot be negative.";
      return;
   }

   clearScreen();
   Database::mainBank.listRecentTransactions(Database::transactionLog, k_days);
}

void handleListActiveBuyers()
{
   clearScreen();
   printHeader("Top Active Buyers Today");

   int n;
   cout << "How many top buyers would you like to display? (type 0 to cancel): ";
   cin >> n;
   cin.ignore(numeric_limits<streamsize>::max(), '\n');
   if (n == 0)
   {
      Database::globalMessage = "Operation canceled.";
      return;
   }

   Database::mainBank.listMostActiveBuyersToday(Database::transactionLog, n);
}

void handleListActiveSellers()
{
   clearScreen();
   printHeader("Top Active Sellers Today");

   int n;
   cout << "How many top sellers would you like to display? (type 0 to cancel): ";
   cin >> n;
   cin.ignore(numeric_limits<streamsize>::max(), '\n');
   if (n == 0)
   {
      Database::globalMessage = "Operation canceled.";
      return;
   }

   Database::mainBank.listMostActiveSellersToday(Database::transactionLog, n);
}

void handleRegisterBuyer()
{
   clearScreen();
   printHeader("Register New Buyer Account");

   string name, email;
   double initialDeposit = 0;

   cout << "Enter Full Name (type 0 to cancel): ";
   getline(cin, name);
   if (name == "0")
   {
      Database::globalMessage = "Operation canceled.";
      return;
   }

   cout << "Enter Email         : ";
   getline(cin, email);

   cout << "Enter Initial Deposit: Rp ";
   while (!(cin >> initialDeposit) || initialDeposit < 0)
   {
      cout << "Invalid input. Please enter a positive number: Rp ";
      cin.clear();
      cin.ignore(numeric_limits<streamsize>::max(), '\n');
   }

   uint newId = Database::buyers.size() + 1;
   Database::buyers.emplace_back(newId, name, email, initialDeposit);

   Database::mainBank.addCustomer(*(Database::buyers.back().getCustomer()));

   Database::globalMessage = "Registration successful! Your Buyer ID is " + to_string(newId);
}

void handleLogin()
{
   if (Database::buyers.empty())
   {
      Database::globalMessage = "No buyers registered yet. Please register first.";
      return;
   }

   clearScreen();
   printHeader("Login");

   uint id;
   cout << "Enter your Buyer ID (type 0 to cancel): ";
   cin >> id;

   if (id == 0)
   {
      Database::globalMessage = "Login cancelled.";
      cin.ignore(numeric_limits<streamsize>::max(), '\n');
      return;
   }

   for (auto &buyer : Database::buyers)
   {
      if (buyer.getId() == id)
      {
         Database::loggedInBuyer = &buyer;
         Database::loggedInSeller = nullptr;
         for (auto &seller : Database::sellers)
         {
            if (seller.getBuyer()->getId() == id)
            {
               Database::loggedInSeller = &seller;
               break;
            }
         }
         Database::globalMessage = "Login successful! Welcome, " + Database::loggedInBuyer->getName() + ".";
         return;
      }
   }
   Database::globalMessage = "Login failed. Buyer ID not found.";
}

void handleLogout()
{
   clearScreen();
   printHeader("Logout");

   char choice;
   while (true)
   {
      cout << "Are you sure you want to logout? (y/n): ";
      cin >> choice;
      cin.ignore(numeric_limits<streamsize>::max(), '\n');

      choice = tolower(choice);

      if (choice == 'y')
      {
         string name = Database::loggedInBuyer->getName();

         Database::loggedInBuyer = nullptr;
         Database::loggedInSeller = nullptr;

         Database::globalMessage = "You have logged out. Goodbye, " + name + "!";
         return;
      }
      else if (choice == 'n')
      {
         Database::globalMessage = "Logout cancelled.";
         return;
      }
      else
      {
         cout << "Invalid input. Please enter 'y' or 'n'.\n";
      }
   }
}

// =======================================================
// Handler untuk Menampilkan Menu-Menu
// =======================================================

void showBankMenu()
{
   int choice = 0;
   while (true)
   {
      clearScreen();
   printHeader("Bank Report Menu");
   Database::displayGlobalMessage();

   cout << "1. Show All Customers\n";
   cout << "2. Show Transactions in Recent Days\n";
   cout << "3. Show Dormant Accounts (>30 Days)\n";
   cout << "4. Show Top Users Today\n";
   cout << "5. Show Top Buyers Today\n";
   cout << "6. Show Top Sellers Today\n";
   cout << "7. Return to Main Menu\n";
   cout << "----------------------------------------\n";
   cout << "Your Choice: ";
      cin >> choice;

      if (cin.fail())
      {
         cin.clear();
         Database::globalMessage = "Input tidak valid.";
         choice = 0;
      }
      cin.ignore(numeric_limits<streamsize>::max(), '\n');

      if (choice == 7)
      {
         return;
      }

      if (choice >= 1 && choice <= 6)
      {
         clearScreen();

         switch (choice)
         {
         case 1:
            Database::mainBank.showAllCustomers();
            break;
         case 2:
            handleListRecentBankTransactions();
            break;
         case 3:
            Database::mainBank.listDormantAccounts(Database::transactionLog);
            break;
         case 4:
            printHeader("Top Active Users Today");
            int n;
            cout << "How many top users would you like to display? (type 0 to cancel): ";
            cin >> n;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            if (n == 0)
            {
               Database::globalMessage = "Operation cancelled.";
               return;
            }

            Database::mainBank.listTopUsersToday(Database::transactionLog, n);
            break;
         case 5:
            handleListActiveBuyers();
            break;
         case 6:
            handleListActiveSellers();
            break;
         }

         cout << "\nPress [Enter] to return...";
         cin.get();
      }
      else
      {
         if (choice != 0)
            Database::globalMessage = "Pilihan tidak valid.";
      }
   }
}

void showBuyerMenu()
{
   int choice = 0;
   while (true)
   {
      clearScreen();
   printHeader("Buyer Menu");
   Database::displayGlobalMessage();

   cout << "1. Purchase Item\n";
   cout << "2. View Order History\n";
   cout << "3. Check Spending in Recent Days\n";
   cout << "4. View Cash Flow\n";
   cout << "5. Confirm Item Receipt\n";
   cout << "6. Cancel Order\n";
   cout << "7. Return to Main Menu\n";
   cout << "----------------------------------------\n";
   cout << "Your Choice: ";
      cin >> choice;

      if (cin.fail())
      {
         cin.clear();
         Database::globalMessage = "Input tidak valid.";
         choice = 0;
      }
      cin.ignore(numeric_limits<streamsize>::max(), '\n');

      if (choice == 7)
      {
         return;
      }

      if (choice >= 1 && choice <= 6)
      {
         clearScreen();

         switch (choice)
         {
         case 1:
            handlePurchaseItem();
            break;
         case 2:
            Database::loggedInBuyer->listOrders(Database::transactionLog);
            break;
         case 3:
            Database::loggedInBuyer->checkSpending();
            break;
         case 4:
            Database::loggedInBuyer->showCashFlow();
            break;
         case 5:
            Database::loggedInBuyer->confirmReceipt(Database::transactionLog);
            break;
         case 6:
            Database::loggedInBuyer->cancelOrder(Database::transactionLog, Database::sellers);
            break;
         }
      }
      else
      {
         if (choice != 0)
            Database::globalMessage = "Pilihan tidak valid.";
      }
   }
}

void showManageStoreMenu()
{
   int choice = 0;
   while (true)
   {
      clearScreen();

   printHeader("Store Management: " + Database::loggedInSeller->getStoreName());
   Database::displayGlobalMessage();

   cout << "1. Register New Item\n";
   cout << "2. Update Item (Stock/Price/Remove)\n";
   cout << "3. View All Store Items\n";
   cout << "4. View Pending Orders\n";
   cout << "--- Store Analytics ---\n";
   cout << "5. View Most Popular Items\n";
   cout << "6. View Loyal Customers\n";
   cout << "7. Return\n";
   cout << "----------------------------------------\n";
   cout << "Your Choice: ";
      cin >> choice;

      if (cin.fail())
      {
         cin.clear();
         Database::globalMessage = "Input tidak valid.";
         choice = 0;
      }
      cin.ignore(numeric_limits<streamsize>::max(), '\n');

      if (choice == 7)
      {
         return;
      }

      if (choice >= 1 && choice <= 6)
      {
         clearScreen();

         switch (choice)
         {
         case 1:
            Database::loggedInSeller->registerNewItem();
            break;
         case 2:
            Database::loggedInSeller->updateExistingItem();
            break;
         case 3:
            printHeader("Items in " + Database::loggedInSeller->getStoreName());
            Database::loggedInSeller->getStoreItems()->showAllItems();
            cout << "\nPress [Enter] to return...";
            cin.get();
            break;
         case 4:
            Database::loggedInSeller->listPendingOrders(Database::transactionLog);
            cout << "\nPress [Enter] to return...";
            cin.get();
            break;
         case 5:
            Database::loggedInSeller->showTopKItems(Database::transactionLog);
            break;
         case 6:
            Database::loggedInSeller->showLoyalCustomers(Database::transactionLog);
            break;
         }
      }
      else
      {
         if (choice != 0)
         {
            Database::globalMessage = "Pilihan tidak valid.";
         }
      }
   }
}

void showRegisterMenu()
{
   int choice = 0;
   while (true)
   {
      clearScreen();
   printHeader("Registration Menu");
   Database::displayGlobalMessage();

   cout << "1. Create Buyer Account\n";
   cout << "2. Return to Main Menu\n";
   cout << "----------------------------------------\n";
   cout << "Your Choice: ";
      cin >> choice;

      if (cin.fail())
      {
         cin.clear();
         Database::globalMessage = "Input tidak valid. Harap masukkan angka.";
         choice = 0;
      }
      cin.ignore(numeric_limits<streamsize>::max(), '\n');

      switch (choice)
      {
      case 1:
         handleRegisterBuyer();
         break;
      case 2:
         return;
      default:
         if (choice != 0)
            Database::globalMessage = "Pilihan tidak valid.";
         break;
      }
   }
}

void showLoggedInMenu()
{
   int choice = 0;

   while (Database::loggedInBuyer)
   {
      clearScreen();
      printHeader("Welcome, " + Database::loggedInBuyer->getName());
      Database::displayGlobalMessage();

      cout << "1. Check Account Status\n";
      cout << "2. Buyer Menu\n";
      cout << "3. Bank Menu\n";
      cout << "4. Upgrade Account to Seller\n";

      int sellerMenuOption = 0;
      int logoutOption = 5;
      int exitOption = 6;

      if (Database::loggedInSeller)
      {
         sellerMenuOption = 5;
         cout << sellerMenuOption << ". Manage Store\n";
         logoutOption = 6;
         exitOption = 7;
      }

      cout << logoutOption << ". Logout\n";
      cout << exitOption << ". Exit Program\n";
      cout << "----------------------------------------\n";
      cout << "Your Choice: ";
      cin >> choice;

      if (cin.fail())
      {
         cin.clear();
         Database::globalMessage = "Input tidak valid.";
         choice = 0;
      }
      cin.ignore(numeric_limits<streamsize>::max(), '\n');

      if (choice == 2)
      {
         showBuyerMenu();
      }
      else if (choice == 3)
      {
         showBankMenu();
      }
      else if (Database::loggedInSeller && choice == sellerMenuOption)
      {
         showManageStoreMenu();
      }
      else if (choice == logoutOption)
      {
         handleLogout();
      }
      else if (choice == exitOption)
      {
         Serialization::saveAllData();
         cout << "Thank you!\n";
         exit(0);
      }
      else
      {
         switch (choice)
         {
         case 1:
            handleCheckStatus();
            break;
         case 4:
            handleUpgradeToSeller();
            break;
         default:
            if (choice != 0)
               Database::globalMessage = "Pilihan tidak valid.";
            break;
         }
      }
   }
}

// =======================================================
// Fungsi Utama Program
// =======================================================

int main()
{
   Serialization::loadAllData();

   if (Database::buyers.empty())
   {
      Database::seedDatabase();
   }

   int choice = 0;
   while (true)
   {
      if (Database::loggedInBuyer)
      {
         showLoggedInMenu();
      }
      else
      {
         clearScreen();
         printHeader("MINI E-COMMERCE");
         Database::displayGlobalMessage();

         cout << "1. Registration\n";
         cout << "2. Login\n";
         cout << "3. Exit\n";
         cout << "----------------------------------------\n";
         cout << "Your Choice: ";
         cin >> choice;

         if (cin.fail())
         {
            cin.clear();
            Database::globalMessage = "Input tidak valid. Harap masukkan angka.";
            choice = 0;
         }
         cin.ignore(numeric_limits<streamsize>::max(), '\n');

         switch (choice)
         {
         case 1:
            showRegisterMenu();
            break;
         case 2:
            handleLogin();
            break;
         case 3:
            Serialization::saveAllData();
            cout << "Terima kasih!\n";
            return 0;
         default:
            if (choice != 0)
               Database::globalMessage = "Pilihan tidak valid.";
            break;
         }
      }
   }
   return 0;
}