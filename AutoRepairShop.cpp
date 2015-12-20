//
// Created by Andre on 17/10/2015.
//


#include <time.h>
#include <stdlib.h>
#include <algorithm>

#include "AutoRepairShop.h"
#include "ConfigFile.h"

// ============================================ PERSON ============================================ //

Person::Person(string name) {
    this->name = name;
    this->id = -1;
}

Person::Person(istream &in, string name, vector<string> &licensePlates) {
    this->name = name;
    string testString = "-";
    in >> testString;
    while (testString.find('-') != string::npos) {
        if (testString == "-") {
            string file = "Clients file";
            ConfigFile::BadFileException e(file);
            throw (e);
        }
        licensePlates.push_back(testString);
        in >> testString;
    }
    this->id = atoi(testString.c_str());
}

void Person::saveObjectInfo(ostream &out) {
    out << this->name << endl;
    for (size_t i = 0; i < vehicles.size(); i++) {
        out << vehicles[i]->getLicensePlate() << endl;
    }
    out << this->id;
}

bool Person::addVehicle(Vehicle *vehicle) {
    return addsIfNotExist(vehicle, this->vehicles);
}

bool operator==(const Person &p1, const Person &p2) {
    return p1.getName() == p2.getName();
}

bool operator<(const Employee &e1, const Employee &e2) {
    if (e1.getVehicles().size() == e2.getVehicles().size())
        return e1.getID() < e2.getID();
    return e1.getVehicles().size() < e2.getVehicles().size();
}

bool operator>(const Client &c1, const Client &c2) {
    return c1.getID() > c2.getID();
}

bool operator>(const Employee &e1, const Employee &e2) {
    return e1.getID() > e2.getID();
}

// ============================================ CLIENT ============================================ //

// ============================================ EMPLOYEE ============================================ //

// ===================================== AUTOREPAIRSHOP ===================================== //

AutoRepairShop::AutoRepairShop(string name) {
    this->name = name;
}

Vehicle *AutoRepairShop::vehicleWithLicensePlate(string licensePlate) {
    for (size_t i = 0; i < vehicles.size(); i++) {
        if (vehicles[i]->getLicensePlate() == licensePlate)
            return vehicles[i];
    }
    throw (InexistentVehicle(licensePlate));
}

vector<Client *> AutoRepairShop::clientsWithName(string name) {
    vector<Client *> clientsWithName;
    for (size_t i = 0; i < clients.size(); i++) {
        if (clients[i]->getName() == name)
            clientsWithName.push_back(clients[i]);
    }
    return clientsWithName;
}

vector<Employee *> AutoRepairShop::employeesWithName(string name) {
    vector<Employee *> employeesWithName;
    for (size_t i = 0; i < employees.size(); i++) {
        if (employees[i]->getName() == name)
            employeesWithName.push_back(employees[i]);
    }
    return employeesWithName;
}

bool AutoRepairShop::isClient(Client *client1) {
    return exists(client1, clients);
}

bool AutoRepairShop::isEmployee(Employee *employee1) {
    return exists(employee1, employees);
}

bool AutoRepairShop::addClient(Client *client) {
    if (isClient(client))
        return false;
    clients.push_back(client);
    clients[clients.size() - 1]->setID(clients.size() - 1);
    return true;
}

bool AutoRepairShop::addEmployee(Employee *employee) {
    if (isEmployee(employee))
        return false;
    employees.push_back(employee);
    employees[employees.size() - 1]->setID(employees.size() - 1);
    if (employees.size() == 1)
        for (size_t i = 0; i < vehicles.size(); i++) {
            assignEmployee(vehicles[i]);
        }
    return true;
}

bool AutoRepairShop::addVehicle(Vehicle *vehicle) {
    if (!addsIfNotExist(vehicle, this->vehicles))
        return false;
    if (employees.size() != 0) {
        insertionSort(employees);
        employees[0]->addVehicle(vehicle);
    }
    return true;
}

bool AutoRepairShop::removeVehicle(Vehicle *vehicle) {
    int index = sequentialSearch(vehicles, vehicle);
    if (index == -1) {
        return false;
    }
    for (size_t i = 0; i < employees.size(); i++) {
        employees[i]->removeVehicle(vehicle); //only removes from the one who actually has it
    }
    for (size_t i = 0; i < clients.size(); i++) {
        clients[i]->removeVehicle(vehicle); //only removes from the one who actually has it
    }
    vehicles.erase(vehicles.begin() + index);
    return true;
}

const string &AutoRepairShop::getName() const {
    return this->name;
}

bool Person::removeVehicle(Vehicle *vehicle) {
    int index = sequentialSearch(vehicles, vehicle);
    if (index == -1)
        return false;
    vehicles.erase(vehicles.begin() + index);
    return true;
}

bool AutoRepairShop::assignEmployee(Vehicle *vehicle) {
    if (employees.size() == 0)
        return false;
    insertionSort(employees);
    employees[0]->addVehicle(vehicle);
    return true;
}

bool AutoRepairShop::removeEmployee(Employee *employee) {
    int index = sequentialSearch(employees, employee);
    if (index == -1)
        return false;
    vector<Vehicle *> employeeVehicles = employee->getVehicles(); // - working ?
    employees.erase(employees.begin() + index);
    for (size_t i = 0; i < employeeVehicles.size(); i++) {
        assignEmployee(employeeVehicles[i]);
    }
    return true;
}

bool AutoRepairShop::removeClient(Client *client) {
    int index = sequentialSearch(clients, client);
    if (index == -1)
        return false;
    for (size_t i = 0; i < clients[index]->getVehicles().size(); i++) {
        removeVehicle(clients[index]->getVehicles()[i]);
    }
    clients.erase(clients.begin() + index);
    return true;
}

void Person::printObjectInfo() const {
    cout << "Name: " << this->name << endl << "ID: " << this->id << endl;
}

void Client::printObjectInfo() const {
    Person::printObjectInfo();
    cout << "Owned vehicles: ";
    for (size_t i = 0; i < vehicles.size(); i++) {
        cout << endl << "Vehicle " << i << ":" << endl;
        vehicles[i]->printObjectInfo();
    }
}

void Employee::printObjectInfo() const {
    Person::printObjectInfo();
    cout << "Assigned vehicles: ";
    for (size_t i = 0; i < vehicles.size(); i++) {
        cout << endl << "Vehicle " << i << ":" << endl;
        vehicles[i]->printObjectInfo();
    }
}

void AutoRepairShop::printVehiclesInfo() const {
    if (vehicles.size() == 0)
        cout << "There are 0 vehicles here. Go ahead and add some!";
    for (size_t i = 0; i < vehicles.size(); i++) {
        cout << "Vehicle " << i << ":" << endl;
        vehicles[i]->printObjectInfo();
        if (i != vehicles.size() - 1)
            cout << endl;
    }
}

void AutoRepairShop::printEmployeesInfo() const {
    if (employees.size() == 0)
        cout << "There are 0 employees here. Go ahead and add some!";
    vector<Employee *> employeesById = employees;
    inverseInsertionSort(employeesById);
    reverse(employeesById.begin(), employeesById.end());
    for (size_t i = 0; i < employeesById.size(); i++) {
        employeesById[i]->printObjectInfo();
        if (i != employeesById.size() - 1)
            cout << endl;
    }
}

void AutoRepairShop::printClientsInfo() const {
    if (clients.size() == 0)
        cout << "There are 0 clients here. Go ahead and add some!";
    vector<Client *> clientsById = clients;
    inverseInsertionSort(clientsById);
    reverse(clientsById.begin(), clientsById.end());
    for (size_t i = 0; i < clientsById.size(); i++) {
        clientsById[i]->printObjectInfo();
        if (i != clientsById.size() - 1)
            cout << endl;
    }
}

void Client::printServices() const {
    for (size_t i = 0; i < vehicles.size(); i++) {
        vehicles[i]->printServices();
        if (i != vehicles.size() - 1)
            cout << endl;
    }
}

void AutoRepairShop::printServices() const {
    for (size_t i = 0; i < clients.size(); i++) {
        clients[i]->printServices();
        if (i != clients.size() - 1)
            cout << endl;
    }
}

void AutoRepairShop::printClientsWithFirstLetter(char firstLetter) const {
    vector<Client *> clientsById = clients;
    inverseInsertionSort(clientsById);
    reverse(clientsById.begin(), clientsById.end());
    for (size_t i = 0; i < clientsById.size(); i++) {
        if (clientsById[i]->getName()[0] == firstLetter) {
            if (i != 0)
                cout << endl;
            clientsById[i]->printObjectInfo();
        }
    }
}

void AutoRepairShop::printEmployeesWithFirstLetter(char firstLetter) const {
    vector<Employee *> employeesById = employees;
    inverseInsertionSort(employeesById);
    reverse(employeesById.begin(), employeesById.end());
    for (size_t i = 0; i < employeesById.size(); i++) {
        if (employeesById[i]->getName()[0] == firstLetter) {
            if (i != 0)
                cout << endl;
            employeesById[i]->printObjectInfo();
        }
    }
}

void AutoRepairShop::printAllInfo() const {
    cout << "Auto Repair Shop: " << this->name << endl;
    printClientsInfo();
    cout << endl;
    printEmployeesInfo();
    cout << endl;
    if (vehicles.size() == 0)
        cout << "There are 0 vehicles here. Go ahead and add some!" << endl <<
        "Note: you need to have at least 1 client and 1 employee to add vehicles";
}

bool AutoRepairShop::addVehicleToClient(Vehicle *vehicle, int clientIndex) {
    return clients[clientIndex]->addVehicle(vehicle);
}