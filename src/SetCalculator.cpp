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

//the c-tor of the SetCalculator class
//it sets all the initial values and asks the user what they want the max command number to be
SetCalculator::SetCalculator(std::istream& istr, std::ostream& ostr)
    : m_actions(createActions()), m_operations(createOperations()), m_istr(istr), m_ostr(ostr),
      m_readMode(false), m_continueReading(true)
{
    checkCommandRange();
}

//this function asks the user to enter the max command number he wants
//then it checks to validate that the number is in the range
void SetCalculator::checkCommandRange()
{
    m_ostr << "Please enter the maximum number of functions:\n";
    m_maxCommands = readNewMax(m_istr);
}

//this function runs the program
void SetCalculator::run()
{
    do
    {
        m_istr.clear(); //to clear the buffer
        m_istr.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        runCommands(m_istr);

    } while (m_running);
}

//this function takes in the command that the user wants and runs it
void SetCalculator::runCommands(std::istream& input)
{
    try
    {
        initialPrint(); //print the information about the program
        const auto action = readAction(input);
        runAction(action, input); //run the command that the user wants to perform
        return;
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
    if (m_readMode) //if there's something wrong with the commands in the file
        throw FileError();
}

//this function prints the information about the program
void SetCalculator::initialPrint() const
{
    m_ostr << '\n';
    m_ostr << "Maximum number of commands: " << m_maxCommands << '\n';
    m_ostr << "Number of available commands: " << m_maxCommands - m_operations.size() << '\n';
    printOperations(); //prints the list of available commands
    m_ostr << "Enter command ('help' for the list of available commands): ";
}

void SetCalculator::eval(std::istream& input)
{
    if (auto index = readOperationIndex(input); index)
    {
        const auto& operation = m_operations[*index];
        auto inputs = std::vector<Set>();
        for (auto i = 0; i < operation->inputCount(); ++i)
        {
            inputs.push_back(Set(input));
        }

        operation->print(m_ostr, inputs);
        m_ostr << " = " << operation->compute(inputs) << '\n';
    }
}

void SetCalculator::del(std::istream& input)
{
    if (m_operations.size() == MIN_COMMANDS)
        throw std::out_of_range("Reached the minimum number of operations\n");
    if (auto i = readOperationIndex(input); i)
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

//this function prints out the list of available commands
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

//this function reads the operation index the user wants to perform
std::optional<int> SetCalculator::readOperationIndex(std::istream& input)
{
    int i = 0;
    try
    {
        input >> i;
        if (bool failed = input.fail())
            throw std::istream::failure("");
        if (i >= m_operations.size())
            throw (std::out_of_range(""));
        return i;
    }
    catch (std::istream::failure e)
    {
        m_ostr << "\nBad Operation Input\n";
    }
    catch (std::out_of_range e)
    {
        m_ostr << "\nOperation #" << i << " doesn't exist\n";
    }
    if (m_readMode) //an error in the file
        throw FileError();
    return {};
}

//this function reads the action that will pe performed from the user
SetCalculator::Action SetCalculator::readAction(std::istream& input) const
{
    auto action = std::string();
	input >> action;
	const auto i = std::ranges::find(m_actions, action, &ActionDetails::command);
	if (i != m_actions.end())
	{
		return i->action;
	}
	else
		throw std::invalid_argument("\nReceived incorrect value\n");
    return Action::Invalid;
}

void SetCalculator::runAction(Action action, std::istream& input)
{
    switch (action)
    {
        default:
            m_ostr << "Unknown enum entry used!\n";
            break;

        case Action::Invalid:
            m_ostr << "Command not found\n";
            break;

        case Action::Eval:         eval(input);                     break;
        case Action::Union:        binaryFunc<Union>(input);        break;
        case Action::Intersection: binaryFunc<Intersection>(input); break;
        case Action::Difference:   binaryFunc<Difference>(input);   break;
        case Action::Product:      binaryFunc<Product>(input);      break;
        case Action::Comp:         binaryFunc<Comp>(input);         break;
        case Action::Del:          del(input);                      break;
        case Action::Resize:       Resize(input);                   break;
        case Action::Read:         Read();                          break;
        case Action::Help:         help();                          break;
        case Action::Exit:         exit();                          break;
    }
}

//this function reads the path of a file and executes the commands in it
void SetCalculator::Read()
{
    std::ifstream file;
    openFile(file);
    m_continueReading = true;//to know if the user wants to continue reading the file
    m_readMode = true;//to know that we're in the mode of reading a file
    m_numOfLine = 0;
    std::string temp;

    while (!file.eof() && m_continueReading)
    {
        try
        {
            runCommands(file);
            m_numOfLine++;
        }
        catch (FileError e)//if an error in the file occures
        {
            std::getline(file, temp); //to get to the next lines
            m_ostr << e.what() << m_numOfLine;
            checkToContinue();
        }
    }
    m_readMode = false;//reading the file is done
}

//this function checks if the user wants to continue reading the file
//after finding an error in the file
void SetCalculator::checkToContinue()
{
    int answer;
    m_ostr << "\nDo you want to continue reading the file?\n"
        << "1 = yes, 0 = no\n";
    m_istr >> answer;
    if (!answer)
        m_continueReading = false;
}

//this function takes in the path of the file from the user and opens it
void SetCalculator::openFile(std::ifstream& file)
{
    std::string fileName;
    m_istr >> fileName;
    file.open(fileName + ".txt");
    if (!file.is_open())
        throw InvalidPath();
}

//this function resizes the max number of commands that the user can enter
void SetCalculator::Resize(std::istream& input)
{
    int temp = readNewMax(input);

    if (temp < m_operations.size()) //if the new max is less than the existed commands
    {
        int answer;
        m_ostr << "Do you want to cancel the command or to delete the extra commands?\n"
            << "0 = cancel ,1 = delete extra commands\n";
        input >> answer;
        if (answer)//if the user chooses to erase the extra commands
            m_operations.erase(m_operations.begin() + temp, m_operations.end());
    }
    else
        m_maxCommands = temp;
}

//this function takes in the new max number of commands from the user
int SetCalculator::readNewMax(std::istream& input)
{
    int temp;
    while(true)
    {
        try
        {
            input >> temp;
            if (bool failed = input.fail())
                throw std::istream::failure("");
            if (temp < MIN_COMMANDS || temp > MAX_COMMANDS)//check in range
                throw std::invalid_argument("\nreceived incorrect value\n");
            return temp;
        }
        catch (std::istream::failure e)
        {
            m_ostr << "\nBad input\n";
            m_istr.clear(); //to clear the buffer
            m_istr.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        catch (std::invalid_argument e)
        {
            m_ostr << "\nArgument must be in the range 3 - 100\n";
        }
        if (m_readMode)//the error is in the file
            throw FileError();
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
