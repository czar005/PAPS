#include <iostream>
#include <string>
#include <vector>
#include <regex>

using namespace std;

// ==================== Context ====================
class Context {
private:
    string text;
public:
    Context(string t) : text(t) {}
    string getText() { return text; }
    void setText(string t) { text = t; }
};

// ==================== AbstractExpression ====================
class AbstractExpression {
public:
    virtual void interpret(Context& ctx) = 0;
    virtual ~AbstractExpression() {}
};

// ==================== Terminal Expressions (правила исправления) ====================

// 1. Множественные пробелы → один пробел
class FixSpaces : public AbstractExpression {
public:
    void interpret(Context& ctx) override {
        string text = ctx.getText();
        regex multipleSpaces("\\s+");
        string result = regex_replace(text, multipleSpaces, " ");
        ctx.setText(result);
    }
};

// 2. Дефис с пробелами → тире
class FixDashToTire : public AbstractExpression {
public:
    void interpret(Context& ctx) override {
        string text = ctx.getText();
        regex dashWithSpaces("\\s+-\\s+");
        string result = regex_replace(text, dashWithSpaces, " — ");
        ctx.setText(result);
    }
};

// 3. Неправильные кавычки → « »
class FixQuotes : public AbstractExpression {
public:
    void interpret(Context& ctx) override {
        string text = ctx.getText();
        bool isOpen = true;
        string result;
        
        for (char c : text) {
            if (c == '"') {
                if (isOpen) {
                    result += '«';
                } else {
                    result += '»';
                }
                isOpen = !isOpen;
            } else {
                result += c;
            }
        }
        ctx.setText(result);
    }
};

// 4. Лишний пробел после открывающей скобки и перед закрывающей
class FixParentheses : public AbstractExpression {
public:
    void interpret(Context& ctx) override {
        string text = ctx.getText();
        
        // Удаляем пробел после "("
        regex openParenSpace("\\(\\s+");
        string result = regex_replace(text, openParenSpace, "(");
        
        // Удаляем пробел перед ")"
        regex spaceCloseParen("\\s+\\)");
        result = regex_replace(result, spaceCloseParen, ")");
        
        ctx.setText(result);
    }
};

// 5. Лишний пробел перед запятой
class FixCommaSpace : public AbstractExpression {
public:
    void interpret(Context& ctx) override {
        string text = ctx.getText();
        regex spaceBeforeComma("\\s+,");
        string result = regex_replace(text, spaceBeforeComma, ",");
        ctx.setText(result);
    }
};

// 6. Лишний пробел перед точкой
class FixPeriodSpace : public AbstractExpression {
public:
    void interpret(Context& ctx) override {
        string text = ctx.getText();
        regex spaceBeforePeriod("\\s+\\.");
        string result = regex_replace(text, spaceBeforePeriod, ".");
        ctx.setText(result);
    }
};

// 7. Множественные переводы строк (3+ → 2)
class FixNewlines : public AbstractExpression {
public:
    void interpret(Context& ctx) override {
        string text = ctx.getText();
        regex multipleNewlines("\n{3,}");
        string result = regex_replace(text, multipleNewlines, "\n\n");
        ctx.setText(result);
    }
};

// ==================== Interpreter (собирает все правила) ====================
class Interpreter {
private:
    vector<AbstractExpression*> rules;
    
public:
    Interpreter() {
        // Порядок важен! Сначала глобальные замены
        rules.push_back(new FixSpaces());
        rules.push_back(new FixDashToTire());
        rules.push_back(new FixQuotes());
        rules.push_back(new FixParentheses());
        rules.push_back(new FixCommaSpace());
        rules.push_back(new FixPeriodSpace());
        rules.push_back(new FixNewlines());
    }
    
    string interpret(string input) {
        Context ctx(input);
        
        for (auto rule : rules) {
            rule->interpret(ctx);
        }
        
        return ctx.getText();
    }
    
    ~Interpreter() {
        for (auto rule : rules) {
            delete rule;
        }
    }
};

// ==================== main ====================
int main() {
    Interpreter interpreter;
    
    // Тестовый текст со всеми типами ошибок
    string inputText = 
        "Это   пример   текста   с   множественными   пробелами.\n"
        "Он использует дефис - вместо тире.\n"
        "Здесь \"кавычки\" неправильные.\n"
        "Пример ( скобка с пробелом ) и пробел перед запятой , и перед точкой .\n"
        "А здесь    лишние    переводы    строки.\n\n\n\n"
        "Конец текста.";
    
    cout << "========================================" << endl;
    cout << "  ИНТЕРПРЕТАТОР ТЕКСТА (паттерн Interpreter)" << endl;
    cout << "========================================" << endl;
    
    cout << "\n📄 ИСХОДНЫЙ ТЕКСТ:\n" << endl;
    cout << inputText << endl;
    
    string fixedText = interpreter.interpret(inputText);
    
    cout << "\n✅ ИСПРАВЛЕННЫЙ ТЕКСТ:\n" << endl;
    cout << fixedText << endl;
    
    cout << "\n========================================" << endl;
    cout << "  ИСПРАВЛЕННЫЕ ОШИБКИ:" << endl;
    cout << "  • множественные пробелы → один пробел" << endl;
    cout << "  • дефис с пробелами → тире (—)" << endl;
    cout << "  • \"кавычки\" → «кавычки»" << endl;
    cout << "  • пробелы у скобок удалены" << endl;
    cout << "  • пробел перед , и . удалён" << endl;
    cout << "  • 3+ переводов строк → 2 перевода" << endl;
    cout << "========================================" << endl;
    
    return 0;
}

//./interpreter