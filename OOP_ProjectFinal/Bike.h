// Bike.h
// Bike is a child of Vehicle (INHERITANCE). A bike is charged PER HOUR.
#ifndef BIKE_H
#define BIKE_H

#include "Vehicle.h"

class Bike : public Vehicle {
private:
    bool hasGear;

public:
    Bike(string i, string b, double r, bool g) : Vehicle(i, b, r) {
        hasGear = g;
    }

    // POLYMORPHISM: Bike has its own way to calculate the rent.
    double calculateRent(int hours) {
        return hours * getRate();
    }

    // A bike is kept for the number of hours that were paid for.
    int allowedHours(int hours) {
        return hours;
    }

    // Late fee for a bike: every late hour costs 2 times the hourly rate.
    double calculateLateFee(int lateHours) {
        return lateHours * getRate() * 2;
    }

    void display(ostream &out) {
        printBase(out);
        if (hasGear) {
            out << "Geared";
        } else {
            out << "Gearless";
        }
    }

    // Format: Bike id rate available condition ratingTotal ratingCount gear brand   (gear: 1 = yes, 0 = no)
    void saveTo(ostream &out) {
        saveBase(out);
        out << (hasGear ? 1 : 0) << " " << getBrand() << endl;
    }

    string getType() { return "Bike"; }
    string getUnit() { return "hour"; }
};

#endif
