// Truck.h
// Truck is a child of Vehicle (INHERITANCE). A truck is charged PER KILOMETRE.
#ifndef TRUCK_H
#define TRUCK_H

#include "Vehicle.h"

class Truck : public Vehicle {
private:
    double loadCapacity;   // in tons

public:
    Truck(string i, string b, double r, double load) : Vehicle(i, b, r) {
        loadCapacity = load;
    }

    // POLYMORPHISM: Truck has its own way to calculate the rent.
    double calculateRent(int km) {
        return km * getRate();
    }

    // We assume a truck drives 50 km every hour, so the time allowed is km / 50 (rounded up).
    int allowedHours(int km) {
        return (km + 49) / 50;
    }

    // Late fee for a truck: a fixed Rs. 200 for every late hour.
    double calculateLateFee(int lateHours) {
        return lateHours * 200;
    }

    void display(ostream &out) {
        printBase(out);
        out << loadCapacity << " ton load";
    }

    // Format: Truck id rate available condition ratingTotal ratingCount load brand
    void saveTo(ostream &out) {
        saveBase(out);
        out << loadCapacity << " " << getBrand() << endl;
    }

    string getType() { return "Truck"; }
    string getUnit() { return "km"; }
};

#endif
