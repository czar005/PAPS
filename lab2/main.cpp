#include <iostream>
#include <vector>
#include <string>

using namespace std;

// Типы водителей
enum class DriverType { BusDriver, TaxiDriver };

// Типы пассажиров
enum class PassengerType { Adult, Child, Discounted };

// Класс, представляющий готовый продукт - Транспортное средство (автобус или такси)
class Vehicle {
private:
    DriverType driver;
    vector<PassengerType> passengers;
    string vehicleType;
    int capacity;
public:
    Vehicle(string type, int cap) : vehicleType(type), capacity(cap) {}

    void addDriver(DriverType d) {
        driver = d;
    }

    bool addPassenger(PassengerType p) {
        if (passengers.size() < capacity) {
            passengers.push_back(p);
            return true; // Пассажир добавлен
        }
        return false; // Мест нет
    }

    void info() {
        cout << vehicleType << ":" << endl;
        cout << "  Водитель: " << (driver == DriverType::BusDriver ? "Автобуса (кат. D)" : "Такси (кат. B)") << endl;
        cout << "  Пассажиры (" << passengers.size() << "/" << capacity << "):" << endl;
        int adultCount = 0, childCount = 0, discountedCount = 0;
        for (PassengerType p : passengers) {
            switch(p) {
                case PassengerType::Adult: adultCount++; break;
                case PassengerType::Child: childCount++; break;
                case PassengerType::Discounted: discountedCount++; break;
            }
        }
        if (adultCount > 0) cout << "    Взрослых: " << adultCount << endl;
        if (childCount > 0) cout << "    Детей (с креслом): " << childCount << endl;
        if (discountedCount > 0) cout << "    Льготных: " << discountedCount << endl;
        if (passengers.empty()) cout << "    Нет пассажиров" << endl;
        cout << "  Статус: " << ( (passengers.size() > 0) ? "Готов к отправлению" : "Нет пассажиров" ) << endl;
    }
};

// Абстрактный класс Строителя
class VehicleBuilder {
protected:
    Vehicle* vehicle;
public:
    virtual void createVehicle() = 0;
    virtual void addDriver() = 0;
    virtual void addPassenger(PassengerType type) = 0;
    virtual Vehicle* getVehicle() {
        return vehicle;
    }
    virtual ~VehicleBuilder() {}
};

// Конкретный строитель для Автобуса
class BusBuilder : public VehicleBuilder {
private:
    const int BUS_LIMIT = 30;
public:
    void createVehicle() override {
        vehicle = new Vehicle("Автобус", BUS_LIMIT);
    }
    void addDriver() override {
        vehicle->addDriver(DriverType::BusDriver);
    }
    void addPassenger(PassengerType type) override {
        // Автобус может принимать любые типы пассажиров, но проверяем лимит
        if (!vehicle->addPassenger(type)) {
            cout << "!!! Автобус заполнен, пассажир не добавлен." << endl;
        }
    }
};

// Конкретный строитель для Такси
class TaxiBuilder : public VehicleBuilder {
private:
    const int TAXI_LIMIT = 4;
public:
    void createVehicle() override {
        vehicle = new Vehicle("Такси", TAXI_LIMIT);
    }
    void addDriver() override {
        vehicle->addDriver(DriverType::TaxiDriver);
    }
    void addPassenger(PassengerType type) override {
        // Для такси тип Discounted недопустим
        if (type == PassengerType::Discounted) {
            cout << "!!! Льготные пассажиры не допускаются в такси." << endl;
            return;
        }
        // Дополнительная проверка на лимит
        if (!vehicle->addPassenger(type)) {
            cout << "!!! Такси заполнено, пассажир не добавлен." << endl;
        }
    }
};

// Класс-Распорядитель
class Director {
public:
    Vehicle* construct(VehicleBuilder& builder) {
        builder.createVehicle();
        builder.addDriver();          // Шаг 1: Добавить водителя
        // Шаг 2: Добавить пассажиров по алгоритму
        // Для демонстрации добавим пассажиров, имитируя заполнение
        builder.addPassenger(PassengerType::Adult);
        builder.addPassenger(PassengerType::Child);
        builder.addPassenger(PassengerType::Adult);
        // Пытаемся добавить еще, чтобы проверить лимиты или недопустимые типы
        builder.addPassenger(PassengerType::Adult); // Это должно превысить лимит такси
        builder.addPassenger(PassengerType::Discounted); // Это не должно пройти в такси

        return builder.getVehicle();
    }
};

int main() {
    Director director;

    BusBuilder busBuilder;
    TaxiBuilder taxiBuilder;

    Vehicle* myBus = director.construct(busBuilder);
    Vehicle* myTaxi = director.construct(taxiBuilder);

    cout << "\n--- Состояние транспорта ---" << endl;
    myBus->info();
    cout << endl;
    myTaxi->info();

    delete myBus;
    delete myTaxi;

    return 0;
}