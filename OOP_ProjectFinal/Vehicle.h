// Vehicle.h
// The BASE class of the project. It is ABSTRACT because it has pure virtual
// functions (= 0), so we can never create a plain "Vehicle" object.
// Only Car, Bike and Truck objects can be created.
#ifndef VEHICLE_H
#define VEHICLE_H

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
using namespace std;

// ----- Our own exception class (EXCEPTION HANDLING) -----
// We throw it when someone tries to rent a vehicle that cannot be rented.
class VehicleUnavailableException {
public:
    string message;
    VehicleUnavailableException(string m) {
        message = m;
    }
};

// ----- ENUM: the physical condition of a vehicle -----
// GOOD = 0, DAMAGED = 1, SERVICING = 2 (these numbers are saved in the file)
enum Condition { GOOD, DAMAGED, SERVICING };

class Vehicle {
private:                  // ENCAPSULATION: private data, nobody outside can change it directly
    string id;
    string brand;
    double rate;          // price per day / hour / km (depends on the type)
    bool available;       // true = free, false = rented
    Condition condition;  // GOOD, DAMAGED or SERVICING
    int ratingTotal;      // sum of all star ratings given by customers
    int ratingCount;      // how many customers rated this vehicle

protected:
    // Prints the columns that are common to every vehicle.
    void printBase(ostream &out) {
        out << left << setw(7) << id << setw(7) << getType() << setw(18) << brand
            << setw(10) << rate << setw(7) << getUnit()
            << setw(12) << getStatusText() << setw(8) << getRatingText();
    }

public:
    Vehicle(string i, string b, double r) {
        id = i;
        brand = b;
        rate = r;
        available = true;
        condition = GOOD;
        ratingTotal = 0;
        ratingCount = 0;
    }

    virtual ~Vehicle() {}   // virtual destructor so "delete" works for Car, Bike, Truck

    // Getters: we can READ the data, but not change it directly.
    string getId() { return id; }
    string getBrand() { return brand; }
    double getRate() { return rate; }
    bool isAvailable() { return available; }      // true = not rented right now
    Condition getCondition() { return condition; }
    int getRatingTotal() { return ratingTotal; }
    int getRatingCount() { return ratingCount; }

    string getConditionName() {
        if (condition == GOOD) return "Good";
        if (condition == DAMAGED) return "Damaged";
        return "Servicing";
    }

    // The text shown in the "Status" column.
    string getStatusText() {
        if (available == false) return "Rented";
        if (condition == GOOD) return "Available";
        return getConditionName();
    }

    // Average star rating, like "4.5". A dash if nobody has rated it yet.
    string getRatingText() {
        if (ratingCount == 0) return "-";
        double average = (double)ratingTotal / ratingCount;
        int tenths = (int)(average * 10 + 0.5);          // round to 1 decimal place
        return to_string(tenths / 10) + "." + to_string(tenths % 10);
    }

    // A vehicle can be rented only if it is free AND in good condition.
    bool canBeRented() {
        return available && condition == GOOD;
    }

    // Returns the reason why it cannot be rented, or "" if it can be rented.
    string rentProblem() {
        if (available == false) {
            return "Vehicle " + id + " is already rented.";
        }
        if (condition != GOOD) {
            return "Vehicle " + id + " is " + getConditionName() + " and cannot be rented now.";
        }
        return "";
    }

    // Throws an exception if the vehicle cannot be rented.
    void checkRentable() {
        string problem = rentProblem();
        if (problem != "") {
            throw VehicleUnavailableException(problem);
        }
    }

    // ----- These are the ONLY ways to change the private data -----
    void rentVehicle() {
        checkRentable();
        available = false;
    }

    void returnVehicle() {
        available = true;
    }

    void setCondition(Condition c) {
        condition = c;
    }

    // stars is 1 to 5
    void addRating(int stars) {
        ratingTotal = ratingTotal + stars;
        ratingCount = ratingCount + 1;
    }

    // Used only by load(): puts back the saved state of the vehicle.
    void restoreState(bool isFree, int cond, int total, int count) {
        available = isFree;
        if (cond == 1) condition = DAMAGED;
        else if (cond == 2) condition = SERVICING;
        else condition = GOOD;
        ratingTotal = total;
        ratingCount = count;
    }

    // ----- Pure virtual functions (ABSTRACTION) -----
    // Every child class MUST write its own version of these.
    virtual double calculateRent(int duration) = 0;
    virtual int allowedHours(int duration) = 0;      // how many hours the customer may keep it
    virtual double calculateLateFee(int lateHours) = 0;
    virtual void display(ostream &out) = 0;
    virtual void saveTo(ostream &out) = 0;
    virtual string getType() = 0;
    virtual string getUnit() = 0;

protected:
    // The first part of every saved line (same for all types):
    // Type id rate available condition ratingTotal ratingCount
    void saveBase(ostream &out) {
        out << getType() << " " << id << " " << rate << " " << (available ? 1 : 0) << " "
            << (int)condition << " " << ratingTotal << " " << ratingCount << " ";
    }
};

// ----- OPERATOR OVERLOADING -----
// This lets us write:  cout << vehicle;
ostream& operator<<(ostream &out, Vehicle &v) {
    v.display(out);
    return out;
}

#endif
