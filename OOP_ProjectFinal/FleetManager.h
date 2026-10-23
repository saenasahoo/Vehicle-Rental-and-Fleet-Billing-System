// FleetManager.h
// Keeps ALL the vehicles and ALL the rentals, and does the main work:
// add, remove, show, rent, return, history, save to file and load from file.
#ifndef FLEETMANAGER_H
#define FLEETMANAGER_H

#include <vector>
#include <fstream>
#include "Vehicle.h"
#include "Car.h"
#include "Bike.h"
#include "Truck.h"
#include "Customer.h"
#include "Rental.h"
#include "Helpers.h"

class FleetManager {
private:
    // A vector of Vehicle pointers can hold Cars, Bikes and Trucks together.
    // This is the reason polymorphism works in our project.
    vector<Vehicle*> fleet;
    vector<Rental> rentals;

    void printHeader() {
        cout << left << setw(7) << "ID" << setw(7) << "Type" << setw(18) << "Brand"
             << setw(10) << "Rate(Rs)" << setw(7) << "Per" << setw(12) << "Status"
             << setw(8) << "Rating" << "Details" << endl;
        cout << "-----------------------------------------------------------------------------------------\n";
    }

public:
    // Destructor: delete every vehicle we created with "new".
    ~FleetManager() {
        for (int i = 0; i < (int)fleet.size(); i++) {
            delete fleet[i];
        }
    }

    // Returns the vehicle with this ID, or NULL if there is none.
    Vehicle* findVehicle(string id) {
        for (int i = 0; i < (int)fleet.size(); i++) {
            if (fleet[i]->getId() == id) {
                return fleet[i];
            }
        }
        return NULL;
    }

    void addVehicle(Vehicle *v) {
        fleet.push_back(v);
    }

    // Removes a vehicle from the fleet. Returns false if the ID does not exist.
    // (main.cpp checks first that the vehicle is not rented right now.)
    bool removeVehicle(string id) {
        for (int i = 0; i < (int)fleet.size(); i++) {
            if (fleet[i]->getId() == id) {
                // Keep the old rental records for the history, but they must not
                // point to the vehicle any more, because we are going to delete it.
                for (int j = 0; j < (int)rentals.size(); j++) {
                    if (rentals[j].getVehicle() == fleet[i]) {
                        rentals[j].forgetVehicle();
                    }
                }
                delete fleet[i];                  // free the Car / Bike / Truck object
                fleet.erase(fleet.begin() + i);   // remove its pointer from the vector
                return true;
            }
        }
        return false;
    }

    void showFleet() {
        if (fleet.empty()) {
            cout << "\nThe fleet is empty.\n";
            return;
        }
        cout << "\n=== FLEET (" << fleet.size() << " vehicles) ===\n";
        printHeader();
        for (int i = 0; i < (int)fleet.size(); i++) {
            cout << *fleet[i] << endl;
        }
    }

    // Shows only the vehicles that can be rented now. Returns how many were shown.
    int showAvailable() {
        int count = 0;
        for (int i = 0; i < (int)fleet.size(); i++) {
            if (fleet[i]->canBeRented()) {
                if (count == 0) {
                    cout << "\n=== AVAILABLE VEHICLES ===\n";
                    printHeader();
                }
                cout << *fleet[i] << endl;
                count++;
            }
        }
        if (count == 0) {
            cout << "\nNo vehicles are available right now.\n";
        }
        return count;
    }

    // Rents the vehicle. rentVehicle() throws an exception if it cannot be rented.
    Rental rentVehicle(Vehicle *v, Customer c, int duration) {
        v->rentVehicle();
        rentals.push_back(Rental(c, v, duration));
        return rentals.back();
    }

    // true if this vehicle ID has a rental that is not returned yet
    bool hasActiveRental(string id) {
        for (int i = 0; i < (int)rentals.size(); i++) {
            if (rentals[i].isActive() && rentals[i].getVehicleId() == id) {
                return true;
            }
        }
        return false;
    }

    // Returns the vehicle. stars = rating given by the customer (0 = no rating).
    // Gives the finished rental back in "result". Result is false if there is no active rental.
    bool returnVehicle(string id, int stars, Rental &result) {
        for (int i = (int)rentals.size() - 1; i >= 0; i--) {
            if (rentals[i].isActive() && rentals[i].getVehicleId() == id) {
                rentals[i].getVehicle()->returnVehicle();
                rentals[i].closeRental();           // works out the late fee
                if (stars > 0) {
                    rentals[i].setRating(stars);
                    rentals[i].getVehicle()->addRating(stars);
                }
                result = rentals[i];
                return true;
            }
        }
        return false;
    }

    // Shows every rental (active and returned), oldest first.
    void showHistory() {
        if (rentals.empty()) {
            cout << "\nNo rentals yet.\n";
            return;
        }
        double earned = 0;
        cout << "\n=== RENTAL HISTORY (" << rentals.size() << " rentals) ===\n";
        cout << left << setw(7) << "ID" << setw(16) << "Customer" << setw(18) << "Rented on"
             << setw(18) << "Returned on" << right << setw(10) << "Total(Rs)" << left
             << "  Rating" << endl;
        cout << "--------------------------------------------------------------------------\n";
        for (int i = 0; i < (int)rentals.size(); i++) {
            rentals[i].historyRow(cout);
            if (rentals[i].isActive() == false) {
                earned = earned + rentals[i].getTotal();
            }
        }
        cout << "--------------------------------------------------------------------------\n";
        cout << "Money earned from returned rentals: Rs. " << fixed << setprecision(2) << earned << endl;
        cout.unsetf(ios::fixed);
        cout << setprecision(6);
    }

    // ---------------- FILE HANDLING ----------------

    // Writes everything to two text files.
    void save() {
        ofstream vf("vehicles.txt");
        for (int i = 0; i < (int)fleet.size(); i++) {
            fleet[i]->saveTo(vf);
        }
        vf.close();

        ofstream rf("rentals.txt");
        for (int i = 0; i < (int)rentals.size(); i++) {
            Rental &r = rentals[i];
            // Format: vehicleId type unit rate duration active start due returned
            //         lateFee bill rating phone brand|customerName
            rf << r.getVehicleId() << " " << r.getVehicleType() << " " << r.getUnit() << " "
               << r.getRate() << " " << r.getDuration() << " " << (r.isActive() ? 1 : 0) << " "
               << (long long)r.getStartTime() << " " << (long long)r.getDueTime() << " "
               << (long long)r.getReturnTime() << " ";
            rf << fixed << setprecision(2) << r.getLateFee() << " " << r.getBill() << " ";
            rf.unsetf(ios::fixed);
            rf << setprecision(6);
            rf << r.getRating() << " " << r.getCustomer().getPhone() << " "
               << r.getVehicleBrand() << "|" << r.getCustomer().getName() << endl;
        }
        rf.close();
    }

    // Reads the two files back (if they exist) and recreates the objects.
    void load() {
        ifstream vf("vehicles.txt");
        string type, id;
        double rate;
        int availableFlag, cond, ratingTotal, ratingCount;

        while (vf >> type >> id >> rate >> availableFlag >> cond >> ratingTotal >> ratingCount) {
            Vehicle *v = NULL;
            string brand;

            if (type == "Car") {
                int seats;
                string fuel;
                vf >> seats >> fuel;
                getline(vf, brand);
                v = new Car(id, trimText(brand), rate, seats, fuel);
            } else if (type == "Bike") {
                int gear;
                vf >> gear;
                getline(vf, brand);
                v = new Bike(id, trimText(brand), rate, gear == 1);
            } else if (type == "Truck") {
                double load;
                vf >> load;
                getline(vf, brand);
                v = new Truck(id, trimText(brand), rate, load);
            } else {
                string skip;
                getline(vf, skip);   // unknown line: ignore it
            }

            if (v != NULL) {
                v->restoreState(availableFlag == 1, cond, ratingTotal, ratingCount);
                fleet.push_back(v);
            }
        }
        vf.close();

        ifstream rf("rentals.txt");
        string vehicleId, vType, unit, phone;
        double rRate, lateFee, bill;
        int duration, activeFlag, stars;
        long long start, due, ret;
        while (rf >> vehicleId >> vType >> unit >> rRate >> duration >> activeFlag
                  >> start >> due >> ret >> lateFee >> bill >> stars >> phone) {
            string rest;
            getline(rf, rest);                       // "brand|customer name"
            rest = trimText(rest);
            int bar = (int)rest.find('|');
            if (bar < 0) {
                continue;                            // damaged line: ignore it
            }
            string brand = rest.substr(0, bar);
            string name = rest.substr(bar + 1);

            Vehicle *v = findVehicle(vehicleId);     // NULL if the vehicle was removed later
            if (v != NULL || activeFlag == 0) {      // an active rental must have its vehicle
                rentals.push_back(Rental(Customer(name, phone), v, vehicleId, vType, brand, unit,
                                         rRate, duration, activeFlag == 1, (time_t)start,
                                         (time_t)due, (time_t)ret, lateFee, bill, stars));
            }
        }
        rf.close();

        cout << "Loaded " << fleet.size() << " vehicle(s) and " << rentals.size()
             << " rental record(s) from file.\n";
    }
};

#endif
