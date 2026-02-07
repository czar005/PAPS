using System;
using System.Collections.Generic;

// ============ SINGLETON ДЛЯ ВОДИТЕЛЯ ============
public abstract class Driver
{
    private static Dictionary<Type, Driver> _instances = new Dictionary<Type, Driver>();
    
    public string Name { get; protected set; } = "";
    public string LicenseCategory { get; protected set; } = "";
    public bool IsBusy { get; set; } = false;
    
    protected Driver(string name, string licenseCategory)
    {
        Name = name;
        LicenseCategory = licenseCategory;
    }
    
    public static T GetInstance<T>(string name, string licenseCategory) where T : Driver, new()
    {
        Type type = typeof(T);
        
        if (!_instances.ContainsKey(type))
        {
            // Используем стандартный конструктор и затем инициализируем свойства
            T instance = new T();
            instance.Initialize(name, licenseCategory);
            _instances[type] = instance;
        }
        
        return (T)_instances[type];
    }
    
    protected virtual void Initialize(string name, string licenseCategory)
    {
        Name = name;
        LicenseCategory = licenseCategory;
    }
    
    public abstract bool CanDrive(Vehicle vehicle);
}

public class TaxiDriver : Driver
{
    public TaxiDriver() : base("", "") { }
    
    protected override void Initialize(string name, string licenseCategory)
    {
        base.Initialize(name, licenseCategory);
    }
    
    public override bool CanDrive(Vehicle vehicle)
    {
        return vehicle is Taxi;
    }
}

public class BusDriver : Driver
{
    public BusDriver() : base("", "") { }
    
    protected override void Initialize(string name, string licenseCategory)
    {
        base.Initialize(name, licenseCategory);
    }
    
    public override bool CanDrive(Vehicle vehicle)
    {
        return vehicle is Bus;
    }
}

// ============ АБСТРАКТНЫЕ И КОНКРЕТНЫЕ ПРОДУКТЫ ============
public abstract class Vehicle
{
    public string Model { get; protected set; } = "";
    public int Capacity { get; protected set; }
    public Driver? Driver { get; protected set; }
    public List<string> Passengers { get; private set; } = new List<string>();
    public bool IsReadyToDepart { get; protected set; }
    
    public bool AssignDriver(Driver driver)
    {
        if (Driver != null)
        {
            Console.WriteLine($"Машина {Model} уже имеет водителя: {Driver.Name}");
            return false;
        }
        
        if (driver.IsBusy)
        {
            Console.WriteLine($"Водитель {driver.Name} уже занят");
            return false;
        }
        
        if (driver.CanDrive(this))
        {
            Driver = driver;
            driver.IsBusy = true;
            Console.WriteLine($"Водитель {driver.Name} назначен на {Model}");
            return true;
        }
        
        Console.WriteLine($"Водитель {driver.Name} не может управлять {Model}");
        return false;
    }
    
    public bool AddPassenger(string passengerName)
    {
        if (Passengers.Count >= Capacity)
        {
            Console.WriteLine($"Машина {Model} переполнена! Лимит: {Capacity} пассажиров");
            return false;
        }
        
        Passengers.Add(passengerName);
        Console.WriteLine($"Пассажир {passengerName} сел в {Model}");
        return true;
    }
    
    public void CheckReadiness()
    {
        if (Driver == null)
        {
            Console.WriteLine($"Машина {Model} не может отправиться: нет водителя");
            IsReadyToDepart = false;
            return;
        }
        
        if (Passengers.Count == 0)
        {
            Console.WriteLine($"Машина {Model} не может отправиться: нет пассажиров");
            IsReadyToDepart = false;
            return;
        }
        
        IsReadyToDepart = true;
        Console.WriteLine($"Машина {Model} готова к отправлению! Водитель: {Driver.Name}, Пассажиров: {Passengers.Count}/{Capacity}");
    }
    
    public void Depart()
    {
        if (IsReadyToDepart && Driver != null)
        {
            Console.WriteLine($"Машина {Model} отправляется! 🚗💨");
            // Освобождаем водителя
            Driver.IsBusy = false;
        }
        else
        {
            Console.WriteLine($"Машина {Model} не готова к отправлению!");
        }
    }
}

public class Taxi : Vehicle
{
    public Taxi(string model)
    {
        Model = model;
        Capacity = 4;
    }
}

public class Bus : Vehicle
{
    public Bus(string model)
    {
        Model = model;
        Capacity = 30;
    }
}

// ============ АБСТРАКТНАЯ ФАБРИКА ============
public abstract class TransportFactory
{
    public abstract Vehicle CreateVehicle(string model);
    public abstract Driver CreateDriver(string name, string licenseCategory);
}

public class TaxiFactory : TransportFactory
{
    public override Vehicle CreateVehicle(string model)
    {
        return new Taxi(model);
    }
    
    public override Driver CreateDriver(string name, string licenseCategory)
    {
        return Driver.GetInstance<TaxiDriver>(name, licenseCategory);
    }
}

public class BusFactory : TransportFactory
{
    public override Vehicle CreateVehicle(string model)
    {
        return new Bus(model);
    }
    
    public override Driver CreateDriver(string name, string licenseCategory)
    {
        return Driver.GetInstance<BusDriver>(name, licenseCategory);
    }
}

// ============ ГЛАВНЫЙ КЛАСС С МЕТОДОМ MAIN ============
class Program
{
    static void Main(string[] args)
    {
        Console.WriteLine("=== Система управления транспортом ===");
        Console.WriteLine("Демонстрация паттернов Singleton и Abstract Factory\n");
        
        // Создаем фабрики
        TransportFactory taxiFactory = new TaxiFactory();
        TransportFactory busFactory = new BusFactory();
        
        // Демонстрация Singleton
        Console.WriteLine("1. Демонстрация Singleton для водителей:");
        Console.WriteLine("----------------------------------------");
        
        // Создаем водителей (Singleton гарантирует один экземпляр)
        Driver taxiDriver1 = taxiFactory.CreateDriver("Иван Таксистов", "B");
        Driver taxiDriver2 = taxiFactory.CreateDriver("Петр Таксистов", "B"); // Этот вызов вернет того же Ивана!
        Driver busDriver = busFactory.CreateDriver("Сергей Автобусов", "D");
        
        Console.WriteLine($"Таксист 1: {taxiDriver1.Name}");
        Console.WriteLine($"Таксист 2: {taxiDriver2.Name} (Singleton: один и тот же объект)");
        Console.WriteLine($"Автобусник: {busDriver.Name}\n");
        
        // Проверяем что это действительно один объект
        Console.WriteLine($"Проверка ссылок: taxiDriver1 == taxiDriver2: {object.ReferenceEquals(taxiDriver1, taxiDriver2)}");
        
        // Создаем транспорт
        Console.WriteLine("\n2. Создание транспортных средств:");
        Console.WriteLine("----------------------------------");
        
        Vehicle taxi1 = taxiFactory.CreateVehicle("Такси-001");
        Vehicle taxi2 = taxiFactory.CreateVehicle("Такси-002");
        Vehicle bus1 = busFactory.CreateVehicle("Автобус-101");
        
        Console.WriteLine($"Создано такси: {taxi1.Model}, {taxi2.Model}");
        Console.WriteLine($"Создан автобус: {bus1.Model}\n");
        
        // Назначаем водителей
        Console.WriteLine("3. Назначение водителей:");
        Console.WriteLine("------------------------");
        taxi1.AssignDriver(taxiDriver1);
        bus1.AssignDriver(busDriver);
        
        // Пытаемся назначить того же водителя в другую машину (не получится)
        Console.WriteLine("\nПопытка назначить занятого водителя в другую машину:");
        taxi2.AssignDriver(taxiDriver1);
        
        // Добавляем пассажиров
        Console.WriteLine("\n4. Посадка пассажиров:");
        Console.WriteLine("----------------------");
        
        Console.WriteLine("В такси-001:");
        for (int i = 1; i <= 4; i++)
        {
            taxi1.AddPassenger($"Пассажир-Такси{i}");
        }
        
        // Пытаемся добавить лишнего (не получится)
        Console.WriteLine("\nПопытка добавить лишнего пассажира:");
        taxi1.AddPassenger("ЛишнийПассажир");
        
        Console.WriteLine("\nВ автобус-101:");
        for (int i = 1; i <= 30; i++)
        {
            bus1.AddPassenger($"Пассажир-Автобус{i}");
        }
        
        // Проверяем готовность
        Console.WriteLine("\n5. Проверка готовности к отправлению:");
        Console.WriteLine("--------------------------------------");
        taxi1.CheckReadiness();
        taxi2.CheckReadiness();
        bus1.CheckReadiness();
        
        // Отправляем
        Console.WriteLine("\n6. Отправление транспорта:");
        Console.WriteLine("---------------------------");
        taxi1.Depart();
        bus1.Depart();
        
        // Демонстрация: после отправления водитель освободился
        Console.WriteLine("\n7. Демонстрация освобождения водителя:");
        Console.WriteLine("----------------------------------------");
        Console.WriteLine($"Водитель {taxiDriver1.Name} освободился и теперь может взять другую машину");
        taxi2.AssignDriver(taxiDriver1); // Теперь можно!
        
        // Добавляем пассажиров в такси-002
        Console.WriteLine("\nДобавляем пассажиров в Такси-002:");
        taxi2.AddPassenger("Пассажир1");
        taxi2.AddPassenger("Пассажир2");
        
        // Проверяем готовность и отправляем
        taxi2.CheckReadiness();
        taxi2.Depart();
        
        Console.WriteLine("\n=== Программа завершена успешно! ===");
    }
}