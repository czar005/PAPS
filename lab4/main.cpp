#include <iostream>
#include <string>
using namespace std;

// ==================== Subject ====================
class Graphic {
public:
    virtual void draw(int x, int y) = 0;
    virtual void handleMouse(int x, int y, bool doubleClickRight) = 0;
    virtual ~Graphic() {}
};

// ==================== RealSubject ====================
class RealImage : public Graphic {
private:
    string filename;
    bool loaded;
public:
    RealImage(string file) : filename(file), loaded(false) {
        cout << "[RealImage] Создан объект (картинка не загружена)" << endl;
    }
    
    void load() {
        if (!loaded) {
            cout << "[RealImage] Загружаем " << filename << " с диска..." << endl;
            loaded = true;
        }
    }
    
    void draw(int x, int y) override {
        if (loaded) {
            cout << "[RealImage] Рисуем картинку в позиции (" << x << "," << y << ")" << endl;
        } else {
            cout << "[RealImage] ОШИБКА: картинка не загружена!" << endl;
        }
    }
    
    void handleMouse(int x, int y, bool doubleClickRight) override {}
};

// ==================== Proxy ====================
class ImageProxy : public Graphic {
private:
    RealImage* realImage;
    string filename;
    int posX, posY;
    bool loaded;
public:
    ImageProxy(string file, int startX, int startY) 
        : filename(file), posX(startX), posY(startY), loaded(false), realImage(nullptr) {}
    
    ~ImageProxy() { delete realImage; }
    
    void draw(int x, int y) override {
        if (loaded && realImage) {
            realImage->draw(posX, posY);
        } else {
            cout << "[Proxy] Рисуем прямоугольник-заместитель в позиции (" << posX << "," << posY << ")" << endl;
            cout << "       (Размеры: 300x200, текст: 'Double-click right button to load image')" << endl;
        }
    }
    
    void handleMouse(int x, int y, bool doubleClickRight) override {
        bool inside = (x >= posX && x <= posX + 300 && y >= posY && y <= posY + 200);
        
        if (doubleClickRight && inside) {
            if (!loaded) {
                cout << "[Proxy] Двойной клик по боксу! Загружаем реальное изображение..." << endl;
                realImage = new RealImage(filename);
                realImage->load();
                loaded = true;
                draw(posX, posY);
            } else {
                cout << "[Proxy] Изображение уже загружено" << endl;
            }
        }
    }
    
    void move(int dx, int dy) {
        posX += dx;
        posY += dy;
        cout << "[Proxy] Бокс перемещён на новую позицию (" << posX << "," << posY << ")" << endl;
    }
};

int main() {
    cout << "=== ГРАФИЧЕСКИЙ РЕДАКТОР (ЭМУЛЯЦИЯ) ===" << endl;
    cout << "Команды: m <dx> <dy> - переместить бокс" << endl;
    cout << "         d            - двойной клик правой кнопкой" << endl;
    cout << "         q            - выход" << endl;
    cout << "========================================" << endl;
    
    ImageProxy proxy("TestImage.png", 100, 100);
    proxy.draw(0, 0);
    
    string cmd;
    while (true) {
        cout << "> ";
        cin >> cmd;
        
        if (cmd == "m") {
            int dx, dy;
            cin >> dx >> dy;
            proxy.move(dx, dy);
            proxy.draw(0, 0);
        } else if (cmd == "d") {
            proxy.handleMouse(250, 200, true);
            proxy.draw(0, 0);
        } else if (cmd == "q") {
            break;
        } else {
            cout << "Неизвестная команда" << endl;
        }
    }
    
    return 0;
}
