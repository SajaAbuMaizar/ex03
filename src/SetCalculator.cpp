#include "SetCalculator.h"

#include "Union.h"
#include "Intersection.h"
#include "Difference.h"
#include "Product.h"
#include "Comp.h"
#include "Identity.h"

#include <istream>
#include <ostream>
#include <sstream>
#include <algorithm>
#include <limits>
#include <iostream>

namespace rng = std::ranges;



SetCalculator::SetCalculator(std::istream& istr, std::ostream& ostr)
    : m_actions(createActions()), m_operations(createOperations()), m_istr(istr), m_ostr(ostr)
{
    checkCommandRange();
}

void SetCalculator::checkCommandRange()
{
    m_ostr << "Please enter maximum number of functions:\n";
    m_maxCommands = readNewMax();
}

//asks to anter arguments
void SetCalculator::getArguments(std::istream& input, int& arg1, int& arg2, int num_of_args)
{
    std::string line;
    std::getline(input, line);
    std::stringstream ss(line);
    ss.exceptions(ss.failbit | ss.badbit);

    if (num_of_args == SINGLE_ARG)
        ss >> arg1;
    else
        ss >> arg1 >> arg2;
}

void SetCalculator::run()
{
    do
    {
        m_istr.clear(); //to clear the buffer
        m_istr.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        try
        {
            m_ostr << '\n';
            m_ostr << "Number of maximum commands: " << m_maxCommands << '\n';
            m_ostr << "Number of available commands: " << m_maxCommands - m_operations.size() << '\n';
            printOperations();
            m_ostr << "Enter command ('help' for the list of available commands): ";
            const auto action = readAction();
            runAction(action);
        }
        catch (std::invalid_argument e)
        {
            m_ostr << e.what();
        }
        catch (std::out_of_range e)
        {
            m_ostr << e.what();
        }
        catch (std::istream::failure e)
        {
            m_ostr << e.what();
        }
        catch (InvalidPath e)//if the file is invalid
        {
            std::cerr << e.what() << "please try again\n";
        }

    } while (m_running);
}


void SetCalculator::eval()
{
    if (auto index = readOperationIndex(); index)
    {
        const auto& operation = m_operations[*index];
        auto inputs = std::vector<Set>();
        for (auto i = 0; i < operation->inputCount(); ++i)
        {
            inputs.push_back(Set(m_istr));
        }

        operation->print(m_ostr, inputs);
        m_ostr << " = " << operation->compute(inputs) << '\n';
    }
}

void SetCalculator::del()
{
    if (m_operations.size() == MIN_COMMANDS)
        throw std::out_of_range("Reached the minimum number of operations\n");
    if (auto i = readOperationIndex(); i)
    {
        m_operations.erase(m_operations.begin() + *i);
    }
}

void SetCalculator::help()
{
    m_ostr << "The available commands are:\n";
    for (const auto& action : m_actions)
    {
        m_ostr << "* " << action.command << action.description << '\n';
    }
    m_ostr << '\n';
}

void SetCalculator::exit()
{
    m_ostr << "Goodbye!\n";
    m_running = false;
}

void SetCalculator::printOperations() const
{
    m_ostr << "List of available set operations:\n";
    for (decltype(m_operations.size()) i = 0; i < m_operations.size(); ++i)
    {
        m_ostr << i << ".\t";
        auto gen = NameGenerator();
        m_operations[i]->print(m_ostr, gen);
        m_ostr << '\n';
    }
    m_ostr << '\n';
}

std::optional<int> SetCalculator::readOperationIndex()
{
    int i = 0;
    try
    {
        m_istr >> i;
        if (bool failed = m_istr.fail())
        {
            throw std::istream::failure("");
        }
        if (i >= m_operations.size())
            throw (std::out_of_range(""));
    }
    catch (std::istream::failure e)
    {
        m_ostr << "Bad Operation Input\n";
        return {};
    }
    catch (std::out_of_range e)
    {
        m_ostr << "Operation #" << i << " doesn't exist\n";
        return {};
    }
    return i;
}

SetCalculator::Action SetCalculator::readAction() const
{
    auto action = std::string();
	m_istr >> action;
	const auto i = std::ranges::find(m_actions, action, &ActionDetails::command);
	if (i != m_actions.end())
	{
		return i->action;
	}
	else
		throw std::invalid_argument("received incorrect value\n");
    return Action::Invalid;
}

void SetCalculator::runAction(Action action)
{
    switch (action)
    {
        default:
            m_ostr << "Unknown enum entry used!\n";
            break;

        case Action::Invalid:
            m_ostr << "Command not found\n";
            break;

        case Action::Eval:         eval();                     break;
        case Action::Union:        binaryFunc<Union>();        break;
        case Action::Intersection: binaryFunc<Intersection>(); break;
        case Action::Difference:   binaryFunc<Difference>();   break;
        case Action::Product:      binaryFunc<Product>();      break;
        case Action::Comp:         binaryFunc<Comp>();         break;
        case Action::Del:          del();                      break;
        case Action::Resize:       Resize();                   break;
        case Action::Read:         Read();                     break;
        case Action::Help:         help();                     break;
        case Action::Exit:         exit();                     break;
    }
}

void SetCalculator::Read()
{
    std::ifstream file;
    openFile(file);


}

void SetCalculator::openFile(std::ifstream& file)
{
    std::string fileName;
    m_istr >> fileName;
    file.open(fileName);
    if (!file.is_open())
        throw InvalidPath();
}

void SetCalculator::Resize()
{
    int temp = readNewMax();

    if (temp < m_operations.size()) //if the new max is less than the existed commands
    {
        int answer;
        m_ostr << "Do you want to cancel the command or delete the extra commands?\n"
            << "0 = cancel ,1 = delete extra commands\n";
        m_istr >> answer;
        if (answer)
            m_operations.erase(m_operations.begin() + temp, m_operations.end());
    }
    else
        m_maxCommands = temp;
}

int SetCalculator::readNewMax()
{
    int temp;
    while(true)
    {
        try
        {
            m_istr >> temp;
            if (bool failed = m_istr.fail())
                throw std::istream::failure("");
            if (temp < MIN_COMMANDS || temp > MAX_COMMANDS)
                throw std::invalid_argument("received incorrect value\n");
            return temp;
        }
        catch (std::istream::failure e)
        {
            m_ostr << "Bad input\n";
        }
        catch (std::invalid_argument e)
        {
            m_ostr << "Argument must be in the range 3 - 100\n";
        }
        m_ostr << "Enter new max command number:\n";
    }
}

SetCalculator::ActionMap SetCalculator::createActions()
{
    return ActionMap
    {
        {
            "eval",
            "(uate) num ... - compute the result of function #num on the "
            "following set(s); each set is prefixed with the count of numbers to"
            " read",
            Action::Eval
        },
        {
            "uni",
            "(on) num1 num2 - Creates an operation that is the union of "
            "operation #num1 and operation #num2",
            Action::Union
        },
        {
            "inter",
            "(section) num1 num2 - Creates an operation that is the "
            "intersection of operation #num1 and operation #num2",
            Action::Intersection
        },
        {
            "diff",
            "(erence) num1 num2 - Creates an operation that is the "
            "difference of operation #num1 and operation #num2",
            Action::Difference
        },
        {
            "prod",
            "(uct) num1 num2 - Creates an operation that returns the product of"
            " the items from the results of operation #num1 and operation #num2",
            Action::Product
        },
        {
            "comp",
            "(osite) num1 num2 - creates an operation that is the composition "
            "of operation #num1 and operation #num2",
            Action::Comp
        },
        {
            "del",
            "(ete) num - delete operation #num from the operation list",
            Action::Del
        },
		{
			"read",
			"reads the path for a file",
			Action::Read
		},
		{
			"resize",
            "resizes the number of the maximum commands that can be entered",
			Action::Resize
		},
        {
            "help",
            " - print this command list",
            Action::Help
        },
        {
            "exit",
            " - exit the program",
            Action::Exit
        }
    };
}

SetCalculator::OperationList SetCalculator::createOperations()
{
    return OperationList
    {
        std::make_shared<Union>(std::make_shared<Identity>(), std::make_shared<Identity>()),
        std::make_shared<Intersection>(std::make_shared<Identity>(), std::make_shared<Identity>()),
        std::make_shared<Difference>(std::make_shared<Identity>(), std::make_shared<Identity>())
    };
}
