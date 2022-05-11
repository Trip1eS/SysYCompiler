#include "Matcher.hpp"
#include <algorithm>

void StringMatcher::reset() {
    Matcher::reset();
    _index = 0;
}

void StringMatcher::read(char curChar, char nextChar) {
    if (getCurrentStatus() == MatchStatus::Reject) return;
    if (_index >= _str.size() || _str[_index] != curChar ||
        _index + 1 < _str.size() && _str[_index + 1] != nextChar) {
        setStatus(MatchStatus::Reject);
        return;
    }
    if (_index != _str.size() - 1) {
        setStatus(MatchStatus::Reading);
    } else {
        setStatus(MatchStatus::Accept);
    }
    _index++;
}

IntConstMatcher::IntConstMatcher(TokenType tokenType)
    : Matcher(tokenType) {
    _onError = [this]() -> auto { return getErrorMsg(); };
}

std::string IntConstMatcher::getErrorMsg() {
    char buf[256];
    if (_type == Type::Decimal) {
        snprintf(buf, sizeof(buf), "Illegal decimal number \"%s\"", _val.c_str());
    } else if (_type == Type::Hexadecimal) {
        snprintf(buf, sizeof(buf), "Illegal hexadecimal number \"%s\"", _val.c_str());
    } else {
        snprintf(buf, sizeof(buf), "Illegal octal number \"%s\"", _val.c_str());
    }
    return buf;
}

void IntConstMatcher::reset() {
    Matcher::reset();
    _val.clear();
}

bool IntConstMatcher::check() {
    switch (_type) {
        case Type::Decimal:
            return std::all_of(_val.begin(), _val.end(), [](auto& c) { return std::isdigit(c); });
        case Type::Hexadecimal:
            return _val.size() > 2 &&
                   std::all_of(_val.begin() + 2, _val.end(),
                               [](auto& c) {
                                   return std::isdigit(c) ||
                                          c >= 'a' && c <= 'f' ||
                                          c >= 'A' && c <= 'F';
                               });
        case Type::Octal:
            return _val.size() > 1 &&
                   std::all_of(_val.begin() + 1, _val.end(),
                               [](auto& c) {
                                   return c >= '0' && c <= '7';
                               });
        default:
            return false;
    }
}

void IntConstMatcher::read(char curChar, char nextChar) {
    if (getCurrentStatus() == MatchStatus::Reject) return;
    if (_val.empty()) {
        if (!std::isdigit(curChar)) {
            setStatus(MatchStatus::Reject);
            return;
        }
        if (curChar == '0' && (std::isdigit(nextChar) || std::isalpha(nextChar))) {
            if (nextChar == 'x' || nextChar == 'X') {
                _type = Type::Hexadecimal;
            } else {
                _type = Type::Octal;
            }
        } else {
            _type = Type::Decimal;
        }
    }
    if (!std::isdigit(curChar) && !std::isalpha(curChar)) {
        setStatus(MatchStatus::Reject);
        return;
    }
    _val += curChar;
    if (std::isdigit(nextChar) || isalpha(nextChar)) {
        setStatus(MatchStatus::Reading);
    } else {
        if (check()) {
            setStatus(MatchStatus::Accept);
        } else {
            setStatus(MatchStatus::Error);
        }
    }
}

std::string IntConstMatcher::getValue() {
    switch (_type) {
        case Type::Decimal:
            return _val;
        case Type::Hexadecimal: {
            int num = 0;
            int weight = 1;
            for (auto iter = _val.rbegin(); iter != _val.rend() - 2; ++iter) {
                int i;
                if (std::isdigit(*iter))
                    i = *iter - '0';
                else if (std::islower(*iter))
                    i = *iter - 'a' + 10;
                else
                    i = *iter - 'A' + 10;
                num += weight * i;
                weight *= 16;
            }
            return std::to_string(num);
        }
        case Type::Octal: {
            int num = 0;
            int weight = 1;
            for (auto iter = _val.rbegin(); iter != _val.rend() - 1; ++iter) {
                int i = *iter - '0';
                num += weight * i;
                weight *= 8;
            }
            return std::to_string(num);
        }
        default:
            return "";
    }
}

void IdMatcher::reset() {
    Matcher::reset();
    _val.clear();
}

void IdMatcher::read(char curChar, char nextChar) {
    if (getCurrentStatus() == MatchStatus::Reject) return;
    if (_val.empty()) {
        if (!(curChar == '_' || std::isalpha(curChar))) {
            setStatus(MatchStatus::Reject);
        } else if (nextChar == '_' || std::isalpha(nextChar) || std::isdigit(nextChar)) {
            setStatus(MatchStatus::Reading);
        } else {
            setStatus(MatchStatus::Accept);
        }
    } else {
        if (curChar == '_' || std::isalpha(curChar) || std::isdigit(curChar)) {
            if (nextChar == '_' || std::isalpha(nextChar) || std::isdigit(nextChar)) {
                setStatus(MatchStatus::Reading);
            } else {
                setStatus(MatchStatus::Accept);
            }
        } else {
            setStatus(MatchStatus::Reject);
        }
    }
    _val += curChar;
}