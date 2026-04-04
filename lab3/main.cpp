#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>

using namespace std;

// ==================== Component (абстрактный класс) ====================
class FlightComponent {
protected:
    string name;
    int maxPassengers;      // максимальное количество (для классов)
    int currentCount;       // текущее количество
public:
    FlightComponent(string n) : name(n), maxPassengers(0), currentCount(0) {}
    FlightComponent(string n, int max) : name(n), maxPassengers(max), currentCount(0) {}
    
    virtual ~FlightComponent() {}
    
    // Виртуальные методы — будут переопределены
    virtual int getTotalBaggageWeight() { return 0; }
    virtual int getPassengerCount() { return 0; }
    virtual void removeExcessBaggage(int& excessToRemove, vector<string>& removedLog) {}
    virtual void add(FlightComponent* component) { /* по умолчанию ничего не делает */ }
    virtual void info(int indent = 0) {}
    
    string getName() { return name; }
    int getMaxPassengers() { return maxPassengers; }
    int getCurrentCount() { return currentCount; }
    void setCurrentCount(int count) { currentCount = count; }
};

// ==================== Leaf (примитивы) ====================

// Базовый класс для всех, у кого нет потомков
class Person : public FlightComponent {
protected:
    int baggageWeight;
public:
    Person(string n) : FlightComponent(n), baggageWeight(0) {}
    
    int getBaggageWeight() { return baggageWeight; }
    
    int getTotalBaggageWeight() override {
        return baggageWeight;
    }
    
    int getPassengerCount() override {
        return 1;  // один человек
    }
    
    void info(int indent = 0) override {
        string spaces(indent, ' ');
        cout << spaces << "├─ " << name;
        if (baggageWeight > 0) {
            cout << " (багаж: " << baggageWeight << " кг)";
        }
        cout << endl;
    }
};

// Пассажир (может иметь багаж)
class Passenger : public Person {
private:
    int freeBaggageAllowance;  // бесплатный лимит багажа
public:
    Passenger(string n, int baggage, int allowance) 
        : Person(n), freeBaggageAllowance(allowance) {
        baggageWeight = baggage;
    }
    
    int getFreeAllowance() { return freeBaggageAllowance; }
    
    // Снять багаж (только для эконом-класса)
    int removeBaggage(int amount) {
        int removed = min(amount, baggageWeight);
        baggageWeight -= removed;
        return removed;
    }
    
    void info(int indent = 0) override {
        string spaces(indent, ' ');
        cout << spaces << "├─ " << name 
             << " (багаж: " << baggageWeight << "/" << freeBaggageAllowance 
             << " кг, класс: " << freeBaggageAllowance;
        if (freeBaggageAllowance == 20) cout << " (эконом)";
        else if (freeBaggageAllowance == 35) cout << " (бизнес)";
        else if (freeBaggageAllowance == 999) cout << " (первый, без лимита)";
        cout << ")" << endl;
    }
};

// Пилот (не имеет багажа)
class Pilot : public Person {
public:
    Pilot(string n) : Person(n) {
        baggageWeight = 0;
    }
    
    void info(int indent = 0) override {
        string spaces(indent, ' ');
        cout << spaces << "├─ " << name << " (ПИЛОТ, без багажа)" << endl;
    }
};

// Стюардесса (не имеет багажа)
class Stewardess : public Person {
public:
    Stewardess(string n) : Person(n) {
        baggageWeight = 0;
    }
    
    void info(int indent = 0) override {
        string spaces(indent, ' ');
        cout << spaces << "├─ " << name << " (СТЮАРДЕССА, без багажа)" << endl;
    }
};

// ==================== Composite (составные объекты) ====================

// Базовый класс для составных объектов (классов пассажиров)
class PassengerClass : public FlightComponent {
protected:
    vector<Passenger*> passengers;
public:
    PassengerClass(string n, int max) : FlightComponent(n, max) {}
    
    ~PassengerClass() {
        for (auto p : passengers) {
            delete p;
        }
    }
    
    void add(FlightComponent* component) override {
        Passenger* passenger = dynamic_cast<Passenger*>(component);
        if (passenger && currentCount < maxPassengers) {
            passengers.push_back(passenger);
            currentCount++;
        }
    }
    
    int getTotalBaggageWeight() override {
        int total = 0;
        for (auto p : passengers) {
            total += p->getBaggageWeight();
        }
        return total;
    }
    
    int getPassengerCount() override {
        return currentCount;
    }
    
    void removeExcessBaggage(int& excessToRemove, vector<string>& removedLog) override {
        // По умолчанию — ничего не делаем (только эконом переопределит)
    }
    
    void info(int indent = 0) override {
        string spaces(indent, ' ');
        cout << spaces << "┌─ " << name << " (вместимость: " << maxPassengers 
             << ", пассажиров: " << currentCount 
             << ", общий вес багажа: " << getTotalBaggageWeight() << " кг)" << endl;
        for (auto p : passengers) {
            p->info(indent + 2);
        }
    }
    
    vector<Passenger*>& getPassengers() { return passengers; }
};

// Первый класс (без лимита багажа)
class FirstClass : public PassengerClass {
public:
    FirstClass() : PassengerClass("Первый класс", 10) {}
};

// Бизнес-класс (лимит багажа 35 кг)
class BusinessClass : public PassengerClass {
public:
    BusinessClass() : PassengerClass("Бизнес-класс", 20) {}
};

// Эконом-класс (лимит багажа 20 кг, можно снимать багаж)
class EconomyClass : public PassengerClass {
public:
    EconomyClass() : PassengerClass("Эконом-класс", 150) {}
    
    void removeExcessBaggage(int& excessToRemove, vector<string>& removedLog) override {
        // Снимаем багаж ТОЛЬКО у эконом-класса
        for (auto passenger : passengers) {
            if (excessToRemove <= 0) break;
            
            int passengerBaggage = passenger->getBaggageWeight();
            int freeAllowance = passenger->getFreeAllowance();
            int excessFromPassenger = max(0, passengerBaggage - freeAllowance);
            
            if (excessFromPassenger > 0) {
                int toRemove = min(excessToRemove, excessFromPassenger);
                int removed = passenger->removeBaggage(toRemove);
                excessToRemove -= removed;
                
                if (removed > 0) {
                    removedLog.push_back("  Снято " + to_string(removed) + " кг багажа у пассажира " + passenger->getName());
                }
            }
        }
    }
};

// ==================== Корневой Composite (Самолёт) ====================
class Airplane : public FlightComponent {
private:
    vector<FlightComponent*> components;
    int maxBaggageLimit;  // максимальная загрузка багажом
    
public:
    Airplane(string n, int maxBaggage) : FlightComponent(n), maxBaggageLimit(maxBaggage) {}
    
    ~Airplane() {
        for (auto c : components) {
            delete c;
        }
    }
    
    void add(FlightComponent* component) override {
        components.push_back(component);
    }
    
    int getTotalBaggageWeight() override {
        int total = 0;
        for (auto c : components) {
            total += c->getTotalBaggageWeight();
        }
        return total;
    }
    
    int getPassengerCount() override {
        int total = 0;
        for (auto c : components) {
            total += c->getPassengerCount();
        }
        return total;
    }
    
    void info(int indent = 0) override {
        string spaces(indent, ' ');
        cout << "\n" << spaces << "══════════════════════════════════════" << endl;
        cout << spaces << "  ✈️  " << name << endl;
        cout << spaces << "══════════════════════════════════════" << endl;
        cout << spaces << "Лимит багажа: " << maxBaggageLimit << " кг" << endl;
        cout << spaces << "Всего пассажиров: " << getPassengerCount() << endl;
        cout << spaces << "Общий вес багажа: " << getTotalBaggageWeight() << " кг" << endl;
        
        int excess = getTotalBaggageWeight() - maxBaggageLimit;
        if (excess > 0) {
            cout << spaces << "⚠️  ПЕРЕВЕС: " << excess << " кг ⚠️" << endl;
        } else {
            cout << spaces << "✅ Перевеса нет" << endl;
        }
        cout << spaces << "──────────────────────────────────────" << endl;
        
        for (auto c : components) {
            c->info(indent + 2);
        }
    }
    
    void checkAndRemoveExcessBaggage() {
        int totalWeight = getTotalBaggageWeight();
        int excess = totalWeight - maxBaggageLimit;
        
        if (excess <= 0) {
            cout << "\n✅ Перевеса нет, багаж не снимается." << endl;
            return;
        }
        
        cout << "\n⚠️  ОБНАРУЖЕН ПЕРЕВЕС: " << excess << " кг" << endl;
        cout << "Начинаем снятие багажа (только у эконом-класса)...\n" << endl;
        
        vector<string> removedLog;
        
        // Ищем эконом-класс в составе самолёта
        for (auto c : components) {
            EconomyClass* economy = dynamic_cast<EconomyClass*>(c);
            if (economy) {
                economy->removeExcessBaggage(excess, removedLog);
            }
        }
        
        // Выводим информацию о снятом багаже
        cout << "📋 ОТЧЁТ О СНЯТИИ БАГАЖА:" << endl;
        if (removedLog.empty()) {
            cout << "  Багаж не был снят (возможно, недостаточно веса у эконом-класса)" << endl;
        } else {
            for (const auto& log : removedLog) {
                cout << log << endl;
            }
        }
        
        int remainingExcess = getTotalBaggageWeight() - maxBaggageLimit;
        if (remainingExcess > 0) {
            cout << "\n⚠️  После снятия остался перевес: " << remainingExcess << " кг" << endl;
            cout << "   (Недостаточно багажа в эконом-классе для полной компенсации)" << endl;
        } else {
            cout << "\n✅ Перевес полностью устранён!" << endl;
        }
    }
    
    int getMaxBaggageLimit() { return maxBaggageLimit; }
};

// ==================== Функция для генерации случайного веса багажа ====================
int randomBaggageWeight(int min, int max) {
    static random_device rd;
    static mt19937 gen(rd());
    uniform_int_distribution<> dis(min, max);
    return dis(gen);
}

// ==================== main() ====================
int main() {
    cout << "\n";
    cout << "╔══════════════════════════════════════════════════════════════╗" << endl;
    cout << "║     СИСТЕМА КОНТРОЛЯ ЗАГРУЗКИ САМОЛЁТА (ПАТТЕРН COMPOSITE)   ║" << endl;
    cout << "╚══════════════════════════════════════════════════════════════╝" << endl;
    
    // Создаём самолёт с лимитом багажа 4000 кг
    Airplane* airplane = new Airplane("Боинг-747", 4000);
    
    // ===== Создаём классы =====
    FirstClass* firstClass = new FirstClass();
    BusinessClass* businessClass = new BusinessClass();
    EconomyClass* economyClass = new EconomyClass();
    
    // ===== Добавляем пилотов =====
    airplane->add(new Pilot("Капитан Алексей Ветров"));
    airplane->add(new Pilot("Второй пилот Дмитрий Соколов"));
    
    // ===== Добавляем стюардесс =====
    airplane->add(new Stewardess("Анна"));
    airplane->add(new Stewardess("Мария"));
    airplane->add(new Stewardess("Екатерина"));
    airplane->add(new Stewardess("Ольга"));
    airplane->add(new Stewardess("Татьяна"));
    airplane->add(new Stewardess("Ирина"));
    
    // ===== Заполняем первый класс (10 человек, багаж без лимита) =====
    cout << "\n📋 ЗАПОЛНЕНИЕ САМОЛЁТА..." << endl;
    
    for (int i = 1; i <= 10; i++) {
        // Богатые пассажиры — багаж от 20 до 60 кг
        int baggage = randomBaggageWeight(20, 60);
        firstClass->add(new Passenger("Пассажир 1-" + to_string(i), baggage, 999));
    }
    
    // ===== Заполняем бизнес-класс (20 человек, бесплатно до 35 кг) =====
    for (int i = 1; i <= 20; i++) {
        int baggage = randomBaggageWeight(10, 60);
        businessClass->add(new Passenger("Пассажир B-" + to_string(i), baggage, 35));
    }
    
    // ===== Заполняем эконом-класс (150 человек, бесплатно до 20 кг) =====
    for (int i = 1; i <= 150; i++) {
        int baggage = randomBaggageWeight(5, 50);
        economyClass->add(new Passenger("Пассажир E-" + to_string(i), baggage, 20));
    }
    
    // ===== Добавляем классы в самолёт =====
    airplane->add(firstClass);
    airplane->add(businessClass);
    airplane->add(economyClass);
    
    // ===== Выводим информацию о загрузке =====
    airplane->info();
    
    // ===== Проверяем перевес и снимаем багаж =====
    airplane->checkAndRemoveExcessBaggage();
    
    // ===== Финальная информация после снятия багажа =====
    cout << "\n─────────────────────────────────────────────────────────────" << endl;
    cout << "📊 ИТОГОВАЯ КАРТА ЗАГРУЗКИ ПОСЛЕ СНЯТИЯ БАГАЖА:" << endl;
    cout << "─────────────────────────────────────────────────────────────" << endl;
    cout << "✈️  " << airplane->getName() << endl;
    cout << "   Лимит багажа: " << airplane->getMaxBaggageLimit() << " кг" << endl;
    cout << "   Общий вес багажа: " << airplane->getTotalBaggageWeight() << " кг" << endl;
    
    int finalExcess = airplane->getTotalBaggageWeight() - airplane->getMaxBaggageLimit();
    if (finalExcess > 0) {
        cout << "   ⚠️  ОСТАТОЧНЫЙ ПЕРЕВЕС: " << finalExcess << " кг" << endl;
    } else {
        cout << "   ✅ Перевеса нет" << endl;
    }
    
    cout << "\n🛫 Самолёт готов к отправлению!" << endl;
    
    delete airplane;
    return 0;
}