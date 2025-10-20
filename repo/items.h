#ifndef ITEMS_H
#define ITEMS_H

#include "item.h"
#include <iostream>
#include <vector>
using namespace std;

class Items
{
private:
   vector<Item> items;

public:
   void addItem(const Item &item);
   vector<Item> &getItems();
   Item *findItemById(uint itemId);
   void showAllItems();
   void removeItem(uint itemId);
   void updateItem(uint itemId, const Item &newItem);
};

#endif
