#include <cstdio>
#include <fstream>
#include <iostream>
#include <regex>
#include <ostream>
#include <string>
#include <vector>
#include <algorithm>


bool in_array(const std::string &value, const std::vector<std::string> &array)
{
    return std::find(array.begin(), array.end(), value) != array.end();
}

int check_char(char c, unsigned int line, std::string alphabet){
    if(alphabet.find(c)==std::string::npos){
        std::cerr << "Erro: " << c << "does not belong to the language alphabet" << std::endl;
        return 0;
    }else{
        return 1;
    }
}

int main(){

    std::ifstream program_template;
    program_template.open("program.txt", std::ifstream::in);

    if(program_template.is_open()){
        std::cout << "File opened successfully" << std::endl;
    }

    std::ofstream table;
    table.open("table.txt", std::ofstream::out);

    if(table.is_open()){
        std::cout << "File created successfully" << std::endl;
    }

    std::vector<std::regex> rules = {
        std::regex("\\b[a-zA-Z]\\w+"), //words
        std::regex("\\b\\d+\\b"), //integers
        std::regex("\\b\\d+[.]\\d*"), //float
        std::regex(":(?!=)|[;]|[.]|[,]|[(]|[)]"), //delimiters
        std::regex("[:][=]"), //attribuition
        std::regex("(?![<|>])=|[<|>][=]|[<][>]|(?!<)>|[<]"), //relational operators
        std::regex("[+]|[-]"), //additive operators
        std::regex("[*]|[/]") //multiplicative operators
    };

    std::vector<std::string>key_words = {"program", "var", "integer", "real", "boolean", "procedure",
    "begin", "end", "if", "then", "else", "while", "do", "not"};

    std::string alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 \t\n{}();:.(),=<>+-*/";

    int lines = 1;

    std::string str = "\0";

    while(program_template.peek() != EOF){

        std::getline(program_template, str); //pega linha por linha do txt de entrada
        
        for(char x : str){
            check_char(x, lines, alphabet);
        }

        for(int i = 0; i < 8; i++){
            std::smatch matches;

            while(std::regex_search(str, matches, rules[i])){
                for(auto match : matches)
                {
                    switch (i) {
                        case 0:

                            if(in_array(match, key_words)) table << lines << ' ' << match << " key_word\n";
                            else if (match == "or") table << lines << ' ' << match << " additive operator\n";
                            else if (match == "and") table << lines << ' ' << match << " multiplicative operator\n";
                            else table << lines << ' ' << match << " indentifier\n";
                            break;
                        
                        case 1:
                            table << lines << ' ' << match << " integer\n";
                            break;
                        
                        case 2:
                            table << lines << ' ' << match << " float\n";
                            break;
                        
                        case 3:
                            table << lines << ' ' << match << " delimiter\n";
                            break;
                        
                        case 4:
                            table << lines << ' ' << match << " attribution\n";
                            break;
                        
                        case 5:
                            table << lines << ' ' << match << " relational operators\n";
                            break;
                        
                        case 6:
                            table << lines << ' ' << match << " additive operators\n";
                            break;

                        case 7:
                            table << lines << ' ' << match << " multiplicative operators\n";
                            break;
                    }

                    str = matches.suffix().str();
                }
            }
        }

        ++lines;
    }

    return 0;
}