#ifndef ITEM_H
#define ITEM_H

#include <iostream>
#include <string>
using namespace std;

class Item
{
private:
   uint id;
   string name;
   double price;
   int quantity;

public:
   Item(uint id, string name, double price, int quantity = 1);

   uint getId() const;

   string getName() const;
   double getPrice() const;
   int getQuantity() const;

   void setName(const string &newName);
   void setPrice(double newPrice);
   void setQuantity(int newQuantity);

   void increaseQuantity(int amount);
   void decreaseQuantity(int amount);

   void showItem() const;
};

#endif
