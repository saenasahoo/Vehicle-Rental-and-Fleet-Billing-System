// ============================================================================
//  Vehicle Rental & Fleet Billing System
//  OOP project in C++      Saena Sahoo | B125107 | CSE B | IIIT Bhubaneswar
//
//  main.cpp - shows the menus and calls the classes.
//
//  How to compile:   g++ main.cpp -o VehicleRental
//  How to run:       VehicleRental        (Windows)   or   ./VehicleRental
// ============================================================================
#include <iostream>
#include "FleetManager.h"
#include "Helpers.h"
using namespace std;

// ---------------------------------------------------------------------------
//  ADMIN: add a new vehicle to the fleet
// ---------------------------------------------------------------------------
void addVehicleMenu(FleetManager &fm) {
    cout << "\nVehicle type:  1. Car   2. Bike   3. Truck\n";
    int type = readInt("Choose type: ", 1, 3);

    // The ID must be new.
    string id;
    while (true) {
        id = toUpperCase(readText("Vehicle ID (example C101): "));
        if (fm.findVehicle(id) == NULL) {
            break;
        }
        cout << "  That ID already exists. Try another one.\n";
    }

    string brand = readText("Brand / model: ");
    Vehicle *v = NULL;      // a Vehicle pointer can point to a Car, Bike or Truck

    if (type == 1) {
        double rate = readPositiveDouble("Rate per day (Rs.): ");
        int seats = readInt("Number of seats: ", 1, 60);
        string fuel = readText("Fuel type (one word, like Petrol): ");
        v = new Car(id, brand, rate, seats, fuel);
    } else if (type == 2) {
        double rate = readPositiveDouble("Rate per hour (Rs.): ");
        bool gear = readYesNo("Has gears? (y/n): ");
        v = new Bike(id, brand, rate, gear);
    } else {
        double rate = readPositiveDouble("Rate per km (Rs.): ");
        double load = readPositiveDouble("Load capacity (tons): ");
        v = new Truck(id, brand, rate, load);
    }

    fm.addVehicle(v);
    fm.save();
    cout << "\nVehicle " << id << " added to the fleet.\n";
}

// ---------------------------------------------------------------------------
//  ADMIN: remove a vehicle from the fleet
// ---------------------------------------------------------------------------
void removeVehicleMenu(FleetManager &fm) {
    fm.showFleet();
    string id = toUpperCase(readText("\nEnter Vehicle ID to remove: "));
    Vehicle *v = fm.findVehicle(id);

    if (v == NULL) {
        cout << "\n[Error] Vehicle not found.\n";
        return;
    }
    if (v->isAvailable() == false) {
        cout << "\n[Error] " << id << " is rented right now. Return it first, then remove it.\n";
        return;
    }
    if (readYesNo("Remove " + id + " (" + v->getBrand() + ") permanently? (y/n): ") == false) {
        cout << "\nCancelled. The vehicle was not removed.\n";
        return;
    }

    fm.removeVehicle(id);
    fm.save();
    cout << "\nVehicle " << id << " removed from the fleet.\n";
    cout << "(Its old rentals stay in the rental history.)\n";
}

// ---------------------------------------------------------------------------
//  ADMIN: change the condition of a vehicle (Good / Damaged / Servicing)
// ---------------------------------------------------------------------------
void conditionMenu(FleetManager &fm) {
    fm.showFleet();
    string id = toUpperCase(readText("\nEnter Vehicle ID to update: "));
    Vehicle *v = fm.findVehicle(id);

    if (v == NULL) {
        cout << "\n[Error] Vehicle not found.\n";
        return;
    }
    if (v->isAvailable() == false) {
        cout << "\n[Error] " << id << " is rented right now. Change its condition after it is returned.\n";
        return;
    }

    cout << "\nCurrent condition of " << id << ": " << v->getConditionName() << "\n";
    cout << "1. Good (can be rented)\n";
    cout << "2. Damaged (cannot be rented)\n";
    cout << "3. Servicing (under maintenance, cannot be rented)\n";
    int choice = readInt("New condition: ", 1, 3);

    if (choice == 1) {
        v->setCondition(GOOD);
    } else if (choice == 2) {
        v->setCondition(DAMAGED);
    } else {
        v->setCondition(SERVICING);
    }
    fm.save();
    cout << "\n" << id << " is now marked: " << v->getConditionName() << ".\n";
}

// ---------------------------------------------------------------------------
//  ADMIN: move the demo clock forward (to show a late return in class)
// ---------------------------------------------------------------------------
void clockMenu() {
    cout << "\nTime now: " << formatTime(nowTime()) << "\n";
    cout << "A rental is late when the time passes its \"Due back\" time.\n";
    int hours = readInt("Move the clock forward by how many hours? (0 = cancel): ", 0, 10000);
    if (hours > 0) {
        clockOffsetHours = clockOffsetHours + hours;
        cout << "Time now: " << formatTime(nowTime()) << "\n";
        cout << "(This demo clock goes back to normal when the program is closed.)\n";
    }
}

void adminMenu(FleetManager &fm) {
    while (true) {
        cout << "\n------ ADMIN MENU ------\n";
        cout << "1. Add vehicle\n";
        cout << "2. View fleet\n";
        cout << "3. Remove vehicle\n";
        cout << "4. Update vehicle condition\n";
        cout << "5. View rental history\n";
        cout << "6. Move clock forward (for demo)\n";
        cout << "7. Back to Main Menu\n";
        int choice = readInt("Choice: ", 1, 7);

        switch (choice) {
            case 1: addVehicleMenu(fm); break;
            case 2: fm.showFleet(); break;
            case 3: removeVehicleMenu(fm); break;
            case 4: conditionMenu(fm); break;
            case 5: fm.showHistory(); break;
            case 6: clockMenu(); break;
            case 7: return;
        }
    }
}

// ---------------------------------------------------------------------------
//  CUSTOMER: rent a vehicle
// ---------------------------------------------------------------------------
void rentMenu(FleetManager &fm) {
    if (fm.showAvailable() == 0) {
        return;
    }

    string id = toUpperCase(readText("\nEnter Vehicle ID to rent: "));
    Vehicle *v = fm.findVehicle(id);
    if (v == NULL) {
        cout << "\nVehicle not found. Rental cancelled.\n";
        return;
    }

    // try / catch = EXCEPTION HANDLING
    try {
        // Check first, so the customer does not type all details for nothing.
        // It throws an exception if the vehicle is rented, damaged or being serviced.
        v->checkRentable();

        string name = readText("Customer name: ");
        string phone = readPhone("Phone number (10 digits): ");
        int duration = readInt("Duration in " + v->getUnit() + "(s): ", 1, 100000);

        Rental r = fm.rentVehicle(v, Customer(name, phone), duration);
        fm.save();
        cout << r;                       // prints the receipt (operator<<)
    }
    catch (VehicleUnavailableException &e) {
        cout << "\n[Rental refused] " << e.message << "\n";
    }
}

// ---------------------------------------------------------------------------
//  CUSTOMER: return a vehicle
// ---------------------------------------------------------------------------
void returnMenu(FleetManager &fm) {
    string id = toUpperCase(readText("\nEnter Vehicle ID to return: "));

    Vehicle *v = fm.findVehicle(id);
    if (v == NULL) {
        cout << "\n[Error] Vehicle not found.\n";
        return;
    }
    if (fm.hasActiveRental(id) == false) {
        cout << "\n[Error] No active rental found for this vehicle.\n";
        return;
    }

    int stars = readInt("Rate this vehicle from 1 to 5 stars (0 = skip): ", 0, 5);

    Rental r(Customer("", ""), v, 1);       // empty rental, replaced by returnVehicle
    fm.returnVehicle(id, stars, r);
    fm.save();

    cout << "\nVehicle returned successfully.";
    if (r.isLate()) {
        cout << "\nIt was returned LATE, so a late fee was added.";
    }
    cout << r;
}

void customerMenu(FleetManager &fm) {
    while (true) {
        cout << "\n----- CUSTOMER MENU -----\n";
        cout << "1. View available vehicles\n";
        cout << "2. Rent a vehicle\n";
        cout << "3. Return a vehicle\n";
        cout << "4. Back to Main Menu\n";
        int choice = readInt("Choice: ", 1, 4);

        switch (choice) {
            case 1: fm.showAvailable(); break;
            case 2: rentMenu(fm); break;
            case 3: returnMenu(fm); break;
            case 4: return;
        }
    }
}

// ---------------------------------------------------------------------------
//  main
// ---------------------------------------------------------------------------
int main() {
    cout << "====================================================\n";
    cout << "      VEHICLE RENTAL & FLEET BILLING SYSTEM\n";
    cout << "      IIIT Bhubaneswar - OOP Project (C++)\n";
    cout << "====================================================\n";

    FleetManager fm;
    fm.load();                 // read saved data (if any)

    int choice = 0;
    while (choice != 3) {
        cout << "\n========== MAIN MENU ==========\n";
        cout << "1. Admin\n";
        cout << "2. Customer\n";
        cout << "3. Exit\n";
        choice = readInt("Choice: ", 1, 3);

        if (choice == 1) {
            adminMenu(fm);
        } else if (choice == 2) {
            customerMenu(fm);
        }
    }

    fm.save();                 // save everything before closing
    cout << "\nData saved. Goodbye!\n";
    cout << "Press Enter to exit...";
    cin.get();
    return 0;
}
