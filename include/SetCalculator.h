#pragma once

#include <vector>
#include <memory>
#include <string>
#include <iosfwd>
#include <optional>
#include <iostream>
#include <fstream>
#include "FileException.h"

const int SINGLE_ARG = 1;
const int BINARY_ARG = 2;
const int MIN_COMMANDS = 3;
const int MAX_COMMANDS = 100;

class Operation;

class SetCalculator
{
public:
    SetCalculator(std::istream& istr, std::ostream& ostr);
    void run();

private:
    void eval(std::istream& input);
    void del(std::istream& input);
    void help();
    void exit();

    template <typename FuncType>
    void binaryFunc(std::istream& input)
    {
        if (m_operations.size() == m_maxCommands) //if the user proceeded the max number of commands
            throw std::out_of_range("Reached the maximum number of operations\n");
        if (auto f0 = readOperationIndex(input), f1 = readOperationIndex(input); f0 && f1)
        {
            m_operations.push_back(std::make_shared<FuncType>(m_operations[*f0], m_operations[*f1]));
        }
    }

    void printOperations() const;

    enum class Action
    {
        Invalid,
        Eval,
        Union,
        Intersection,
        Difference,
        Product,
        Comp,
        Del,
        Read,
        Resize,
        Help,
        Exit,
    };

    struct ActionDetails
    {
        std::string command;
        std::string description;
        Action action;
    };

    using ActionMap = std::vector<ActionDetails>;
    using OperationList = std::vector<std::shared_ptr<Operation>>;

    const ActionMap m_actions;
    OperationList m_operations;
    bool m_running = true;
    int m_maxCommands;
    int m_numOfLine;
    bool m_readMode;
    bool m_continueReading;
    std::string m_line;
    std::istream& m_istr;
    std::ostream& m_ostr;

    std::optional<int> readOperationIndex(std::istream& input);
    Action readAction(std::istream& input) const;
    void runAction(Action action, std::istream& input);

    static ActionMap createActions();
    static OperationList createOperations();

    void getArguments(std::istream& inp, int& n1, int& n2, int args);
    void runCommands(std::istream& input);
    void checkToContinue();
    void initialPrint() const;
    void checkCommandRange();
    int readNewMax(std::istream& input);
    void openFile(std::ifstream& file);
    void Resize(std::istream& input);
    void Read();

};
