#include "../repo/item.h"

Item::Item(uint id, string name, double price, int quantity)
    : id(id), name(name), price(price), quantity(quantity) {}

uint Item::getId() const
{
   return id;
}

string Item::getName() const
{
   return name;
}

double Item::getPrice() const
{
   return price;
}

int Item::getQuantity() const
{
   return quantity;
}

void Item::setName(const string &newName) { name = newName; }
void Item::setPrice(double newPrice) { price = newPrice; }
void Item::setQuantity(int newQuantity) { quantity = newQuantity; }

void Item::increaseQuantity(int amount)
{
   quantity += amount;
}

void Item::decreaseQuantity(int amount)
{
   if (quantity >= amount)
      quantity -= amount;
   else
      throw std::runtime_error("Stock not enough");
}

void Item::showItem() const
{
   cout << "ID: " << id
        << ", Name: " << name
        << ", Price: " << price
        << ", Quantity: " << quantity
        << endl;
}
