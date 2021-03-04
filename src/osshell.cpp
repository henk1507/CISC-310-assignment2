#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <sstream>
#include <vector>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdio.h>

void splitString(std::string text, char d, std::vector<std::string>& result);
void vectorOfStringsToArrayOfCharArrays(std::vector<std::string>& list, char ***result);
void freeArrayOfCharArrays(char **array, size_t array_length);

int main (int argc, char **argv)
{
    // Get list of paths to binary executables
    std::vector<std::string> os_path_list;
    char* os_path = getenv("PATH");
    splitString(os_path, ':', os_path_list);


    // Welcome message
    printf("Welcome to OSShell! Please enter your commands ('exit' to quit).\n");

    std::vector<std::string> command_list; // to store command user types in, split into its variour parameters
    char **command_list_exec; // command_list converted to an array of character arrays


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
    std::string pathtest;
    std::string foundpath;
    int index = 0;
    int childcheck;
    int stat_loc;

    struct stat buffer;

    FILE *oldhistory;
    FILE *writehistory;

    char cwd[32768];
    char buff[255];

    getcwd(cwd, sizeof(cwd));

    oldhistory = fopen(strcat(cwd, "/oldhistory.txt"), "r");

    fgets(buff, 255, oldhistory);

    char newline[1];
    newline[0] = '\n';

    if(atoi(buff) < 0);
    {
        index = atoi(buff);
        for(int i = 0; i < index; i++)
        {
            fgets(buff, 255, oldhistory);
            buff[strcspn(buff, "\n")] = '\0';
            history[i] = buff;
        }
    }

    fclose(oldhistory);

    //if (we created a file)
    //  find index from file

    userinput = "";
    
    while(userinput != "exit")
    {
        std::cout << "osshell> ";
        std::getline(std::cin, userinput);

        std::cin.clear();
        //std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        splitString(userinput, ' ', command_list);
        vectorOfStringsToArrayOfCharArrays(command_list, &command_list_exec);

        if(userinput != "" && strcmp(command_list_exec[0], "history") != 0)
        {
            history[index] = userinput;
            index ++;
        }

        if (userinput == "exit")
        {
            writehistory = fopen(cwd, "w+");

            fputs(std::to_string(index).c_str(), writehistory);

            for(int i = 0; i < index; i++)
            {
                fputs("\n", writehistory);
                fputs(history[i].c_str(), writehistory);
            }

            break;
        }
        else if(strcmp(command_list_exec[0], "history") == 0)
        {
            if(command_list_exec[1] == NULL)
            {
                for(int i = 0; i < index; i ++)
                {
                    std::cout << "  " << (i + 1) << ": " << history[i] << std::endl;
                }

                history[index] = userinput;
                index ++;
            }
            else if(strcmp(command_list_exec[1], "clear") == 0)
            {
                index = 0;
            }
            else if(atoi(command_list_exec[1]) > 0)
            {
                for(int i = index - atoi(command_list_exec[1]); i < index; i ++)
                {
                    std::cout << "  " << (i + 1) << ": " << history[i] << std::endl;
                }

                history[index] = userinput;
                index ++;
            }
            else
            {
                std::cout << "Error: history expects an integer > 0 (or 'clear')" << std::endl;
            }
        }
        else if (userinput == "")
        {
            //nothing happens
        }
        else
        {
            foundpath = "";

            for(int i = 0; i < os_path_list.size(); i++)
            {
                if(userinput[0] == '.' || userinput[0] == '/')
                {
                    pathtest = command_list_exec[0];
                }
                else
                {
                    pathtest = (os_path_list[i] + "/" + command_list_exec[0]);
                }

                if(stat(pathtest.c_str(), &buffer) == 0)
                {
                    foundpath = pathtest;
                    break;
                }
            }

            if(foundpath == "")
            {
                std::cout << userinput << ": Error command not found" << std::endl;
            }
            else
            {
                childcheck = fork();
                if (childcheck == 0)
                {
                    execv(foundpath.c_str(), command_list_exec);
                }
                else
                {
                    waitpid(childcheck, &stat_loc, WUNTRACED);
                }
                freeArrayOfCharArrays(command_list_exec, command_list.size() + 1);
            }
        }
    }

    fclose(writehistory);

    return 0;
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

/*
   list: vector of strings to convert to an array of character arrays
   result: pointer to an array of character arrays when the vector of strings is copied to
*/
void vectorOfStringsToArrayOfCharArrays(std::vector<std::string>& list, char ***result)
{
    int i;
    int result_length = list.size() + 1;
    *result = new char*[result_length];
    for (i = 0; i < list.size(); i++)
    {
        (*result)[i] = new char[list[i].length() + 1];
        strcpy((*result)[i], list[i].c_str());
    }
    (*result)[list.size()] = NULL;
}

/*
   array: list of strings (array of character arrays) to be freed
   array_length: number of strings in the list to free
*/
void freeArrayOfCharArrays(char **array, size_t array_length)
{
    int i;
    for (i = 0; i < array_length; i++)
    {
        if (array[i] != NULL)
        {
            delete[] array[i];
        }
    }
    delete[] array;
}