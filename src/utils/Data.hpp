#ifndef DATA_HPP
#define DATA_HPP

#include <string>
#include <vector>

class Data {
public:
    Data();
    ~Data();

    void loadData(const std::string& filename);
    void saveData(const std::string& filename);

private:
    std::vector<std::string> data_;
};

#endif 