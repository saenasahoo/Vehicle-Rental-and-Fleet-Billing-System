// Rental.h
// A Rental connects a Customer with a Vehicle.
// This is COMPOSITION ("has-a"): a Rental HAS a Customer and HAS a Vehicle.
#ifndef RENTAL_H
#define RENTAL_H

#include "Customer.h"
#include "Vehicle.h"
#include "Helpers.h"

class Rental {
private:
    Customer customer;     // the person who rented
    Vehicle *vehicle;      // pointer to the rented vehicle (NULL if it was removed later)

    // A copy of the vehicle details, so the history still works after the vehicle is removed.
    string vehicleId;
    string vehicleType;
    string vehicleBrand;
    string unit;           // day / hour / km
    double rate;

    int duration;          // days / hours / km
    double bill;           // normal rent
    double lateFee;        // extra money if the vehicle came back late
    int rating;            // stars given by the customer (0 = not rated)
    time_t startTime;      // when the vehicle was rented
    time_t dueTime;        // when it must be returned
    time_t returnTime;     // when it was returned (0 = not returned yet)
    bool active;           // true = still rented, false = returned

public:
    // CONSTRUCTOR 1: a brand new rental (starts now).
    Rental(Customer c, Vehicle *v, int d) : customer(c) {
        vehicle = v;
        vehicleId = v->getId();
        vehicleType = v->getType();
        vehicleBrand = v->getBrand();
        unit = v->getUnit();
        rate = v->getRate();
        duration = d;
        lateFee = 0;
        rating = 0;
        startTime = nowTime();
        dueTime = startTime + (time_t)v->allowedHours(d) * 3600;
        returnTime = 0;
        active = true;
        bill = generateBill();
    }

    // CONSTRUCTOR 2 (constructor overloading): a rental read back from the file.
    Rental(Customer c, Vehicle *v, string id, string type, string brand, string u, double r,
           int d, bool isActive, time_t start, time_t due, time_t ret,
           double late, double b, int stars) : customer(c) {
        vehicle = v;
        vehicleId = id;
        vehicleType = type;
        vehicleBrand = brand;
        unit = u;
        rate = r;
        duration = d;
        active = isActive;
        startTime = start;
        dueTime = due;
        returnTime = ret;
        lateFee = late;
        bill = b;
        rating = stars;
    }

    // POLYMORPHISM: the vehicle itself decides how the rent is calculated.
    double generateBill() {
        return vehicle->calculateRent(duration);
    }

    // Called when the vehicle comes back. Works out the late fee (if any).
    void closeRental() {
        returnTime = nowTime();
        if (returnTime > dueTime) {
            int lateHours = (int)((returnTime - dueTime + 3599) / 3600);   // round up to whole hours
            lateFee = vehicle->calculateLateFee(lateHours);                 // POLYMORPHISM again
        }
        active = false;
    }

    // The vehicle was removed from the fleet, so forget the pointer.
    void forgetVehicle() { vehicle = NULL; }

    void setRating(int stars) { rating = stars; }

    bool isActive() { return active; }
    bool isLate() { return lateFee > 0; }
    Vehicle* getVehicle() { return vehicle; }
    Customer getCustomer() { return customer; }
    string getVehicleId() { return vehicleId; }
    string getVehicleType() { return vehicleType; }
    string getVehicleBrand() { return vehicleBrand; }
    string getUnit() { return unit; }
    double getRate() { return rate; }
    int getDuration() { return duration; }
    double getBill() { return bill; }
    double getLateFee() { return lateFee; }
    double getTotal() { return bill + lateFee; }
    int getRating() { return rating; }
    time_t getStartTime() { return startTime; }
    time_t getDueTime() { return dueTime; }
    time_t getReturnTime() { return returnTime; }

    // One line of the rental history table.
    void historyRow(ostream &out) {
        out << left << setw(7) << vehicleId << setw(16) << customer.getName().substr(0, 15)
            << setw(18) << formatTime(startTime)
            << setw(18) << (active ? string("(not returned)") : formatTime(returnTime))
            << right << setw(10) << fixed << setprecision(2) << getTotal() << left << "  ";
        out.unsetf(ios::fixed);
        out << setprecision(6);
        if (rating > 0) {
            out << rating << "/5";
        } else {
            out << "-";
        }
        out << endl;
    }
};

// ----- OPERATOR OVERLOADING -----
// This lets us print a receipt with:  cout << rental;
ostream& operator<<(ostream &out, Rental &r) {
    out << "\n------------------ RENTAL RECEIPT ------------------\n";
    out << "Customer  : " << r.getCustomer().getName() << " (" << r.getCustomer().getPhone() << ")\n";
    out << "Vehicle   : " << r.getVehicleId() << " - " << r.getVehicleType() << " " << r.getVehicleBrand() << "\n";
    out << "Rented on : " << formatTime(r.getStartTime()) << "\n";
    out << "Due back  : " << formatTime(r.getDueTime()) << "\n";
    if (r.isActive() == false) {
        out << "Returned  : " << formatTime(r.getReturnTime()) << "\n";
    }
    out << "Duration  : " << r.getDuration() << " " << r.getUnit() << "(s)\n";
    out << "Rate      : Rs. " << r.getRate() << " per " << r.getUnit() << "\n";
    out << fixed << setprecision(2);
    out << "Rent      : Rs. " << r.getBill() << "\n";
    if (r.isActive() == false) {
        out << "Late fee  : Rs. " << r.getLateFee() << "\n";
    }
    out << "Total     : Rs. " << r.getTotal() << "\n";
    out.unsetf(ios::fixed);
    out << setprecision(6);
    if (r.isActive()) {
        out << "Status    : Active\n";
    } else {
        out << "Status    : Closed (vehicle returned)\n";
        if (r.getRating() > 0) {
            out << "Rating    : " << r.getRating() << "/5\n";
        }
    }
    out << "----------------------------------------------------\n";
    return out;
}

#endif
