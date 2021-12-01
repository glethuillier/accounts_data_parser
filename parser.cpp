#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>

enum StepType { CREATION_STEP, APPROVAL_STEP, WHITELIST_STEP, THRESHOLD };

class Account {
    std::string _accountId, _accountType;

public:
    Account() {};
    Account(std::string accountId, std::string accountType) :
        _accountId(accountId),
        _accountType(accountType)
        {};
    
    std::string getAccountId() {
        return _accountId;
    }
    
    std::string getAccountType() {
        return _accountType;
    }
    
    std::string get() const {
        return "(" + _accountId + ") " + _accountType;
    }
    
    bool operator <(const Account& rhs) const {
        return std::stoi(_accountId) <
            std::stoi(rhs._accountId);
    }
};

class Step {
    StepType _stepType;
    std::string _data;
    
public:
    Step(StepType stepType, std::string data) : _stepType(stepType), _data(data) {};
    
    std::string getData() const {
        return _data;
    }
};

void tokenize(const std::string& str, std::vector<std::string>& tokens) {
    std::string delimiter = ",";
    std::string::size_type lastPos = str.find_first_not_of(delimiter, 0);
    std::string::size_type pos = str.find_first_of(delimiter, lastPos);

    while (std::string::npos != pos || std::string::npos != lastPos) {
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        lastPos = str.find_first_not_of(",", pos);
        pos = str.find_first_of(delimiter, lastPos);
  }
}

int main(int argc, const char * argv[]) {
    std::string workingDir{"/raw_data/"};
    
    // file containing the accounts' data
    std::string filename{"accounts.csv"};
    
    std::ifstream infile(workingDir + filename);

    std::vector<std::string> lines;
    std::string line;
    while (std::getline(infile, line)) {
        lines.emplace_back(line);
    }
    
    // parse
    std::map<std::string, std::map<Account, std::vector<Step>>> workspacesData;
    std::string currentWorkspace = "";
    Account currentAccount;
    
    for(auto const& line : lines) {
        if (line.rfind("Creation step", 0) == 0) {
            Step step(CREATION_STEP, line);
            workspacesData[currentWorkspace][currentAccount].emplace_back(step);
        }
        else if (line.rfind("Approval step", 0) == 0) {
            Step step(APPROVAL_STEP, line);
            workspacesData[currentWorkspace][currentAccount].emplace_back(step);
        }
        else if (line.rfind("Whitelist", 0) == 0) {
            Step step(WHITELIST_STEP, line);
            workspacesData[currentWorkspace][currentAccount].emplace_back(step);
        }
        else if (line.rfind("Threshold", 0) == 0) {
            Step step(THRESHOLD, line);
            workspacesData[currentWorkspace][currentAccount].emplace_back(step);
        }
        else {
            std::vector<std::string> tokens;
            tokenize(line, tokens);
            currentWorkspace = tokens[0];
            Account account = Account(tokens[1], tokens[5]);
            currentAccount = account;
        }
    }

    // show
    for(auto const& workspace : workspacesData) {
        std::cout << workspace.first << std::endl;
        std::cout << "----" << std::endl;
        for(auto const& account : workspace.second) {
            std::cout << std::endl << "\t" << account.first.get() << std::endl << std::endl;
            for(auto const& data : account.second) {
                std::cout << "\t\t - " << data.getData() << std::endl;
            }
            std::cout << std::endl;
        }
        std::cout << std::endl << std::endl;
    }

    // thresholds
    std::map<std::string, unsigned int> thresholds;
    for(auto const& workspace : workspacesData) {
        for(auto const& account : workspace.second) {
            for(auto const& step : account.second) {
                if (step.getType() == StepType::THRESHOLD) {
                    ++thresholds[workspace.first];
                }
            }
        }
    }
    
}
