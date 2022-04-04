#pragma once

#include <vector>
#include <memory>
#include <string>
#include <iosfwd>
#include <optional>
#include <iostream>

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
    void eval();
    void del();
    void help();
    void exit();

    template <typename FuncType>
    void binaryFunc()
    {
        if (m_operations.size() == m_maxCommands)
            throw std::out_of_range("Reached the maximum number of operations\n");
        if (auto f0 = readOperationIndex(), f1 = readOperationIndex(); f0 && f1)
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
    std::istream& m_istr;
    std::ostream& m_ostr;

    std::optional<int> readOperationIndex();
    Action readAction() const;
    void runAction(Action action);

    static ActionMap createActions();
    static OperationList createOperations();

    void getArguments(std::istream& inp, int& n1, int& n2, int args);
    void checkCommandRange();
    int readNewMax();
    void Resize();
    void Read();

};
