#include <regex>
#include <string>
using std::regex;
using std::string;

class StringMatch {
public:
    bool is_reg;
    regex *reg;
    string match;
public:
    StringMatch(const string &str, bool is_reg) {
        this->match = str;
        this->is_reg = is_reg;
        if (is_reg) {
            this->reg = new regex(str);
        }
    }

    bool is_same(const string &str) const {
        if (is_reg) {
            return regex_match(str, *reg);
        } else {
            return str == match;
        }
    }

    bool is_in(const string &str) const {
        if (is_reg) {
            return regex_search(str, *reg);
        } else {
            return str.find(match) != -1;
        }
    }
};