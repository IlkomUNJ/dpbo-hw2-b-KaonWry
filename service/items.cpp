#include "../repo/items.h"

void Items::addItem(const Item &item) { items.push_back(item); }

vector<Item> &Items::getItems() { return items; }

Item *Items::findItemById(uint itemId)
{
   for (auto &item : items)
   {
      if (item.getId() == itemId)
      {
         return &item;
      }
   }
   return nullptr;
}

void Items::showAllItems()
{
   cout << "=== Items List ===" << endl;
   for (const auto &item : items)
   {
      item.showItem();
   }
}

void Items::removeItem(uint itemId)
{
   for (auto it = items.begin(); it != items.end(); ++it)
   {
      if (it->getId() == itemId)
      {
         items.erase(it);
         return;
      }
   }
   throw std::runtime_error("Item not found");
}

void Items::updateItem(uint itemId, const Item &newItem)
{
   for (auto &item : items)
   {
      if (item.getId() == itemId)
      {
         item.setName(newItem.getName());
         item.setPrice(newItem.getPrice());
         item.setQuantity(newItem.getQuantity());
         return;
      }
   }
   throw std::runtime_error("Item not found");
}