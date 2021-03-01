#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <sstream>
#include <vector>
#include <unistd.h>

void allocateArrayOfCharArrays(char ***array_ptr, size_t array_length, size_t item_size);
void freeArrayOfCharArrays(char **array, size_t array_length);
void splitString(std::string text, char d, std::vector<std::string>& result);

int main (int argc, char **argv)
{
    // Get list of paths to binary executables
    std::vector<std::string> os_path_list;
    char* os_path = getenv("PATH");
    splitString(os_path, ':', os_path_list);


    /************************************************************************************
     *   Example code - remove in actual program                                        *
     ************************************************************************************/
    // Shows how to loop over the directories in the PATH environment variable
    int i;
    for (i = 0; i < os_path_list.size(); i++)
    {
        printf("PATH[%2d]: %s\n", i, os_path_list[i].c_str());
    }
    /************************************************************************************
     *   End example code                                                               *
     ************************************************************************************/


    // Welcome message
    printf("Welcome to OSShell! Please enter your commands ('exit' to quit).\n");

    // Allocate space for input command lists
    // `command_list` supports up to 32 command line parameters, 
    //     each with a parameter string length of up to 128 characters
    char **command_list;
    allocateArrayOfCharArrays(&command_list, 32, 128);

    // Repeat:
    //  Print prompt for user input: "osshell> " (no newline)
    //  Get user input for next command
    //  If command is `exit` exit loop / quit program
    //  If command is `history` print previous N commands
    //  For all other commands, check if an executable by that name is in one of the PATH directories
    //   If yes, execute it
    //   If no, print error statement: "<command_name>: Error command not found" (do include newline)
    std::string userinput;
    std::string history[128];
    int index = 0;
    int childcheck;

    //if (we created a file)
    //  find index from file

    userinput = "";
    std::vector<std::string> userarray;

    while(userinput != "exit")
    {
        std::cout << "osshell> ";
        std::getline(std::cin, userinput);

        std::cin.clear();
        //std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        history[index] = userinput;
        index ++;

        std::cout << "segfault check 1" << std::endl;

        splitString(userinput, ' ', userarray);

        std::cout << "segfault check 4" << std::endl;
        //std::cout << (*userarray)[0];

        if (userinput == "exit")
        {
            break;
        }
        else if (userinput == "history")
        {
            for(int i = 0; i < index - 1; i ++)
            {
                std::cout << "  " << (i + 1) << ": " << history[i] << std::endl;
            }
        }
        else if (userinput == "")
        {
            //nothing happens
        }
        else
        {
            childcheck = fork();
            if (childcheck == 0)
            {
                //execv()
            }
        }
    }

    // free memory for `command_list_exec`
    freeArrayOfCharArrays(command_list_exec, command_list.size() + 1);
    printf("------\n");

    return 0;
}

/*
   array_ptr: pointer to list of strings to be allocated
   array_length: number of strings to allocate space for in the list
   item_size: length of each string to allocate space for
*/
void allocateArrayOfCharArrays(char ***array_ptr, size_t array_length, size_t item_size)
{
    int i;
    *array_ptr = new char*[array_length];
    for (i = 0; i < array_length; i++)
    {
        (*array_ptr)[i] = new char[item_size];
    }
}

/*
   array: list of strings to be freed
   array_length: number of strings in the list to free
*/
void freeArrayOfCharArrays(char **array, size_t array_length)
{
    int i;
    for (i = 0; i < array_length; i++)
    {
        delete[] array[i];
    }
    delete[] array;
}

/*
   text: string to split
   d: character delimiter to split `text` on
   result: vector of strings - result will be stored here
*/
void splitString(std::string text, char d, std::vector<std::string>& result)
{
    enum states { NONE, IN_WORD, IN_STRING } state = NONE;

    int i;
    std::string token;
    result.clear();
    for (i = 0; i < text.length(); i++)
    {
        char c = text[i];
        switch (state) {
            case NONE:
                if (c != d)
                {
                    if (c == '\"')
                    {
                        state = IN_STRING;
                        token = "";
                    }
                    else
                    {
                        state = IN_WORD;
                        token = c;
                    }
                }
                break;
            case IN_WORD:
                if (c == d)
                {
                    result.push_back(token);
                    state = NONE;
                }
                else
                {
                    token += c;
                }
                break;
            case IN_STRING:
                if (c == '\"')
                {
                    result.push_back(token);
                    state = NONE;
                }
                else
                {
                    token += c;
                }
                break;
        }
    }
    if (state != NONE)
    {
        result.push_back(token);
    }
}