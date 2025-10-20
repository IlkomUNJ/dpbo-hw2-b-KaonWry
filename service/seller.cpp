#include "../repo/seller.h"
#include "../repo/buyer.h"
#include "../database.cpp"
#include "../utils.cpp"

#include <map>
#include <algorithm>

Seller::Seller(Buyer *buyerRef, const string &storeName, const string &storeAddress, const string &storeEmail)
    : buyer(buyerRef), storeName(storeName), storeAddress(storeAddress), storeEmail(storeEmail)
{
}

Buyer *Seller::getBuyer() const
{
    return buyer;
}

string Seller::getStoreName() const
{
    return storeName;
}

string Seller::getStoreAddress() const
{
    return storeAddress;
}

string Seller::getStoreEmail() const
{
    return storeEmail;
}

BankCustomer *Seller::getCustomerAccount() const
{
    return buyer->getCustomer();
}

Items *Seller::getStoreItems()
{
    return &storeItems;
}

void Seller::registerNewItem()
{
    printHeader("Daftarkan Item Baru");

    string name;
    double price;
    int quantity;

    cout << "Nama Item (ketik 0 untuk batal): ";
    getline(cin, name);
    if (name == "0")
    {
        Database::globalMessage = "Pendaftaran item baru dibatalkan.";
        return;
    }
    cout << "Harga per Item (ketik 0 untuk batal): Rp ";
    cin >> price;
    if (price == 0)
    {
        Database::globalMessage = "Pendaftaran item baru dibatalkan.";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }
    cout << "Jumlah Stok (ketik 0 untuk batal): ";
    cin >> quantity;
    if (quantity == 0)
    {
        Database::globalMessage = "Pendaftaran item baru dibatalkan.";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }

    Item newItem(Database::nextItemId++, name, price, quantity);
    storeItems.addItem(newItem);

    Database::globalMessage = "Item '" + name + "' berhasil didaftarkan!";
}

void Seller::updateExistingItem()
{
    printHeader("Update Item");

    if (storeItems.getItems().empty())
    {
        Database::globalMessage = "Anda belum memiliki item untuk diupdate.";
        return;
    }

    cout << "Item yang Anda miliki:\n";
    storeItems.showAllItems();
    cout << "----------------------------------------\n";

    uint itemId;
    cout << "Masukkan ID item yang ingin diupdate (ketik 0 untuk batal): ";
    cin >> itemId;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (itemId == 0)
    {
        Database::globalMessage = "Update item dibatalkan.";
        return;
    }

    Item *itemToUpdate = storeItems.findItemById(itemId);
    if (!itemToUpdate)
    {
        Database::globalMessage = "Item dengan ID " + to_string(itemId) + " tidak ditemukan.";
        return;
    }

    clearScreen();
    printHeader("Mengupdate: " + itemToUpdate->getName());
    cout << "1. Tambah Stok\n";
    cout << "2. Ubah Harga\n";
    cout << "3. Buang Item\n";
    cout << "Pilihan Anda: ";
    int choice;
    cin >> choice;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    switch (choice)
    {
    case 1:
    {
        int amount;
        cout << "Jumlah stok yang ingin ditambahkan: ";
        cin >> amount;
        itemToUpdate->increaseQuantity(amount);
        Database::globalMessage = "Stok berhasil ditambahkan.";
        break;
    }
    case 2:
    {
        double newPrice;
        cout << "Harga baru: Rp ";
        cin >> newPrice;
        itemToUpdate->setPrice(newPrice);
        Database::globalMessage = "Harga berhasil diubah.";
        break;
    }
    case 3:
    {
        storeItems.removeItem(itemId);
        Database::globalMessage = "Item berhasil dibuang.";
        break;
    }
    default:
        Database::globalMessage = "Pilihan update tidak valid.";
        break;
    }
}

void Seller::showTopKItems(const vector<Transaction> &allTransactions) const
{
    printHeader("Item Terpopuler per Bulan");

    int month, year, k;
    cout << "Masukkan Bulan (1-12) (ketik 0 untuk batal): ";
    cin >> month;
    if (month == 0)
    {
        Database::globalMessage = "Pencarian item dibatalkan.";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }
    cout << "Masukkan Tahun (cth: 2025): ";
    cin >> year;
    cout << "Berapa item teratas (K): ";
    cin >> k;

    map<uint, int> itemPopularity;
    for (const auto &record : allTransactions)
    {
        if (record.sellerId == buyer->getId())
        {
            pair<int, int> recordDate = getMonthYear(record.transactionDate);
            if (recordDate.first == month && recordDate.second == year)
            {
                itemPopularity[record.itemId] += record.quantity;
            }
        }
    }

    if (itemPopularity.empty())
    {
        Database::globalMessage = "Tidak ada data penjualan untuk periode tersebut.";
        return;
    }

    vector<pair<uint, int>> sortedItems(itemPopularity.begin(), itemPopularity.end());
    sort(sortedItems.begin(), sortedItems.end(), [](const auto &a, const auto &b)
         { return a.second > b.second; });

    clearScreen();
    printHeader("Top " + to_string(k) + " Item Populer");

    cout << "Periode: " << month << "/" << year << endl
         << endl;
    int count = 0;
    for (const auto &pair : sortedItems)
    {
        if (count++ >= k)
            break;
        Item *item = const_cast<Seller *>(this)->getStoreItems()->findItemById(pair.first);
        if (item)
        {
            cout << count << ". " << item->getName()
                 << " (ID: " << pair.first << ") - Terjual: "
                 << pair.second << " unit\n";
        }
    }
    cout << "\nTekan [Enter] untuk kembali...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

void Seller::showLoyalCustomers(const vector<Transaction> &allTransactions) const
{
    printHeader("Pelanggan Loyal per Bulan");

    int month, year;
    cout << "Masukkan Bulan (1-12) (ketik 0 untuk batal): ";
    cin >> month;
    if (month == 0)
    {
        Database::globalMessage = "Pencarian pelanggan dibatalkan.";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }
    cout << "Masukkan Tahun (cth: 2025): ";
    cin >> year;

    map<uint, int> customerFrequency;
    for (const auto &record : allTransactions)
    {
        if (record.sellerId == buyer->getId())
        {
            pair<int, int> recordDate = getMonthYear(record.transactionDate);
            if (recordDate.first == month && recordDate.second == year)
            {
                customerFrequency[record.buyerId]++;
            }
        }
    }

    if (customerFrequency.empty())
    {
        Database::globalMessage = "Tidak ada data pelanggan untuk periode tersebut.";
        return;
    }

    vector<pair<uint, int>> sortedCustomers(customerFrequency.begin(), customerFrequency.end());
    sort(sortedCustomers.begin(), sortedCustomers.end(), [](const auto &a, const auto &b)
         { return a.second > b.second; });

    clearScreen();
    printHeader("Top 3 Pelanggan Loyal");

    cout << "Periode: " << month << "/" << year << endl
         << endl;
    int count = 0;
    for (const auto &pair : sortedCustomers)
    {
        if (count++ >= 3)
            break;
        Buyer *b = nullptr;
        for (auto &buyer : Database::buyers)
        {
            if (buyer.getId() == pair.first)
                b = &buyer;
        }
        if (b)
        {
            cout << count << ". " << b->getName()
                 << " (ID: " << pair.first << ") - Jumlah Transaksi: "
                 << pair.second << "\n";
        }
    }
    cout << "\nTekan [Enter] untuk kembali...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

void Seller::listPendingOrders(const vector<Transaction> &allTransactions) const
{
    printHeader("Pesanan Menunggu Penyelesaian (Status PAID)");
    cout << "Pesanan berikut perlu Anda siapkan/kirim:\n\n";

    bool found = false;
    for (const auto &record : allTransactions)
    {
        if (record.sellerId == this->buyer->getId() && record.status == PAID)
        {
            Buyer *buyerCustomer = nullptr;
            for (auto &b : Database::buyers)
            {
                if (b.getId() == record.buyerId)
                {
                    buyerCustomer = &b;
                    break;
                }
            }

            cout << "ID Transaksi: " << record.transactionId << "\n";
            cout << "Pembeli     : " << (buyerCustomer ? buyerCustomer->getName() : "N/A") << " (ID: " << record.buyerId << ")\n";
            cout << "Item        : " << record.itemName << " (x" << record.quantity << ")\n";
            cout << "Total       : Rp " << record.totalPrice << "\n";
            cout << "----------------------------------------\n";
            found = true;
        }
    }

    if (!found)
    {
        cout << "Tidak ada pesanan yang menunggu penyelesaian saat ini.\n";
    }
}