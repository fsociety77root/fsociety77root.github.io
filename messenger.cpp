#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cstring>

// Структура для сообщения
struct Message {
    int id;
    int senderId;
    int receiverId;
    std::string text;
    std::string timestamp;
    bool read;
    
    Message(int sid, int rid, const std::string& txt) 
        : id(0), senderId(sid), receiverId(rid), text(txt), read(false) {
        timestamp = getCurrentTime();
    }
    
    std::string toJson() const {
        std::stringstream ss;
        ss << "{";
        ss << "\"id\":" << id << ",";
        ss << "\"senderId\":" << senderId << ",";
        ss << "\"receiverId\":" << receiverId << ",";
        ss << "\"text\":\"" << escapeJson(text) << "\",";
        ss << "\"timestamp\":\"" << timestamp << "\",";
        ss << "\"read\":" << (read ? "true" : "false");
        ss << "}";
        return ss.str();
    }
    
private:
    std::string getCurrentTime() {
        std::time_t t = std::time(nullptr);
        std::tm* now = std::localtime(&t);
        
        std::stringstream ss;
        ss << std::setfill('0') << std::setw(2) << now->tm_hour << ":"
           << std::setfill('0') << std::setw(2) << now->tm_min;
        return ss.str();
    }
    
    std::string escapeJson(const std::string& str) {
        std::string result;
        for (char c : str) {
            switch (c) {
                case '\"': result += "\\\""; break;
                case '\\': result += "\\\\"; break;
                case '/': result += "\\/"; break;
                case '\b': result += "\\b"; break;
                case '\f': result += "\\f"; break;
                case '\n': result += "\\n"; break;
                case '\r': result += "\\r"; break;
                case '\t': result += "\\t"; break;
                default: result += c;
            }
        }
        return result;
    }
};

// Структура для пользователя
struct User {
    int id;
    std::string name;
    std::string avatar;
    std::string status;
    bool online;
    
    User(int i, const std::string& n, const std::string& a) 
        : id(i), name(n), avatar(a), status("В сети"), online(true) {}
    
    std::string toJson() const {
        std::stringstream ss;
        ss << "{";
        ss << "\"id\":" << id << ",";
        ss << "\"name\":\"" << escapeJson(name) << "\",";
        ss << "\"avatar\":\"" << avatar << "\",";
        ss << "\"status\":\"" << status << "\",";
        ss << "\"online\":" << (online ? "true" : "false");
        ss << "}";
        return ss.str();
    }
    
private:
    std::string escapeJson(const std::string& str) {
        std::string result;
        for (char c : str) {
            if (c == '\"') result += "\\\"";
            else if (c == '\\') result += "\\\\";
            else result += c;
        }
        return result;
    }
};

// Класс мессенджера
class Messenger {
private:
    std::map<int, User> users;
    std::map<std::string, std::vector<Message>> conversations;
    int nextMessageId = 1;
    
public:
    Messenger() {
        initializeUsers();
        initializeDemoMessages();
    }
    
    // Инициализация пользователей
    void initializeUsers() {
        users[1] = User(1, "Алексей", "👨‍💻");
        users[2] = User(2, "Мария", "👩‍💻");
        users[3] = User(3, "Дмитрий", "👨‍🔧");
        users[4] = User(4, "Анна", "👩‍🔬");
    }
    
    // Инициализация демо сообщений
    void initializeDemoMessages() {
        // Алексей -> Мария
        addMessage(1, 2, "Привет! Как продвигается веб-версия?");
        addMessage(2, 1, "Привет! Все отлично, React компоненты готовы");
        addMessage(1, 2, "Круто! C++ часть тоже почти готова");
        
        // Алексей -> Дмитрий
        addMessage(1, 3, "Дмитрий, сервер готов к деплою?");
        addMessage(3, 1, "Да, Docker контейнеры настроены");
        
        // Алексей -> Анна
        addMessage(1, 4, "Анна, есть данные для анализа?");
        addMessage(4, 1, "Да, собираю статистику по использованию");
    }
    
    // Добавление сообщения
    int addMessage(int senderId, int receiverId, const std::string& text) {
        Message msg(senderId, receiverId, text);
        msg.id = nextMessageId++;
        
        std::string convKey = getConversationKey(senderId, receiverId);
        conversations[convKey].push_back(msg);
        
        return msg.id;
    }
    
    // Получение сообщений беседы
    std::string getMessages(int user1, int user2) {
        std::string key1 = getConversationKey(user1, user2);
        std::string key2 = getConversationKey(user2, user1);
        
        std::vector<Message> allMessages;
        
        if (conversations.find(key1) != conversations.end()) {
            allMessages.insert(allMessages.end(), 
                              conversations[key1].begin(), 
                              conversations[key1].end());
        }
        
        if (conversations.find(key2) != conversations.end()) {
            allMessages.insert(allMessages.end(), 
                              conversations[key2].begin(), 
                              conversations[key2].end());
        }
        
        // Сортировка по времени
        std::sort(allMessages.begin(), allMessages.end(),
                 [](const Message& a, const Message& b) {
                     return a.id < b.id;
                 });
        
        // Преобразование в JSON
        std::stringstream ss;
        ss << "[";
        for (size_t i = 0; i < allMessages.size(); ++i) {
            ss << allMessages[i].toJson();
            if (i < allMessages.size() - 1) ss << ",";
        }
        ss << "]";
        
        return ss.str();
    }
    
    // Получение списка контактов
    std::string getContacts() {
        std::stringstream ss;
        ss << "[";
        bool first = true;
        for (const auto& pair : users) {
            if (!first) ss << ",";
            ss << pair.second.toJson();
            first = false;
        }
        ss << "]";
        return ss.str();
    }
    
    // Получение информации о пользователе
    std::string getUserInfo(int userId) {
        if (users.find(userId) != users.end()) {
            return users[userId].toJson();
        }
        return "{}";
    }
    
    // Очистка чата
    void clearChat(int user1, int user2) {
        std::string key1 = getConversationKey(user1, user2);
        std::string key2 = getConversationKey(user2, user1);
        
        conversations.erase(key1);
        conversations.erase(key2);
    }
    
    // Сохранение данных (заглушка)
    void saveData() {
        // В реальной реализации здесь было бы сохранение в файл
        std::cout << "C++: Данные сохранены" << std::endl;
    }
    
    // Загрузка данных (заглушка)
    void loadData() {
        // В реальной реализации здесь была бы загрузка из файла
        std::cout << "C++: Данные загружены" << std::endl;
    }
    
private:
    std::string getConversationKey(int user1, int user2) {
        return std::to_string(user1) + "-" + std::to_string(user2);
    }
};

// Глобальный экземпляр мессенджера
Messenger messenger;

// C функции для взаимодействия с JavaScript
extern "C" {
    
    // Инициализация мессенджера
    void EMSCRIPTEN_KEEPALIVE initMessenger() {
        std::cout << "C++ Messenger initialized" << std::endl;
    }
    
    // Добавление сообщения
    int EMSCRIPTEN_KEEPALIVE addMessage(int senderId, int receiverId, const char* text) {
        return messenger.addMessage(senderId, receiverId, text);
    }
    
    // Получение сообщений
    const char* EMSCRIPTEN_KEEPALIVE getMessages(int user1, int user2) {
        static std::string result;
        result = messenger.getMessages(user1, user2);
        return result.c_str();
    }
    
    // Получение контактов
    const char* EMSCRIPTEN_KEEPALIVE getContacts() {
        static std::string result;
        result = messenger.getContacts();
        return result.c_str();
    }
    
    // Получение информации о пользователе
    const char* EMSCRIPTEN_KEEPALIVE getUserInfo(int userId) {
        static std::string result;
        result = messenger.getUserInfo(userId);
        return result.c_str();
    }
    
    // Очистка чата
    void EMSCRIPTEN_KEEPALIVE clearChat(int user1, int user2) {
        messenger.clearChat(user1, user2);
    }
    
    // Сохранение данных
    void EMSCRIPTEN_KEEPALIVE saveData() {
        messenger.saveData();
    }
    
    // Загрузка данных
    void EMSCRIPTEN_KEEPALIVE loadData() {
        messenger.loadData();
    }
    
    // Вспомогательная функция для выделения памяти в JavaScript
    char* EMSCRIPTEN_KEEPALIVE allocateString(int length) {
        return (char*)malloc(length + 1);
    }
    
    // Вспомогательная функция для освобождения памяти
    void EMSCRIPTEN_KEEPALIVE freeString(char* str) {
        free(str);
    }
}
