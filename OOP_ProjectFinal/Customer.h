// Customer.h
// Stores the details of the person who rents a vehicle.
#ifndef CUSTOMER_H
#define CUSTOMER_H

#include <string>
using namespace std;

class Customer {
private:
    string name;
    string phone;

public:
    Customer(string n, string p) {
        name = n;
        phone = p;
    }

    string getName() { return name; }
    string getPhone() { return phone; }
};

#endif
