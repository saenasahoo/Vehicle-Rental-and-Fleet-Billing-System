// Car.h
// Car is a child of Vehicle (INHERITANCE). A car is charged PER DAY.
#ifndef CAR_H
#define CAR_H

#include "Vehicle.h"

class Car : public Vehicle {
private:
    int seats;
    string fuel;

public:
    Car(string i, string b, double r, int s, string f) : Vehicle(i, b, r) {
        seats = s;
        fuel = f;
    }

    // POLYMORPHISM: Car has its own way to calculate the rent.
    double calculateRent(int days) {
        return days * getRate();
    }

    // A car is kept for whole days: 1 day = 24 hours.
    int allowedHours(int days) {
        return days * 24;
    }

    // Late fee for a car: every late day (or part of a day) costs 1.5 times the daily rate.
    double calculateLateFee(int lateHours) {
        int lateDays = (lateHours + 23) / 24;      // round up to whole days
        return lateDays * getRate() * 1.5;
    }

    void display(ostream &out) {
        printBase(out);
        out << seats << " seats, " << fuel;
    }

    // Writes one line into the data file.
    // Format: Car id rate available condition ratingTotal ratingCount seats fuel brand
    void saveTo(ostream &out) {
        saveBase(out);
        out << seats << " " << fuel << " " << getBrand() << endl;
    }

    string getType() { return "Car"; }
    string getUnit() { return "day"; }
};

#endif
