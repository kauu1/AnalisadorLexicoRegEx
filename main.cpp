#include <cstdio>
#include <fstream>
#include <iostream>
#include <regex>
#include <ostream>
#include <string>
#include <vector>
#include <algorithm>

//checa se um item pertence em uma lista
bool in_array(const std::string &value, const std::vector<std::string> &array)
{
    return std::find(array.begin(), array.end(), value) != array.end();
}

//checa se um char pertence a um alfabeto
int check_char(char c, unsigned int line, std::string alphabet){
    if(alphabet.find(c)==std::string::npos){
        std::cerr << "Erro: '" << c << "' does not belong to the language alphabet in line: " << line << std::endl;
        return 0;
    }else{
        return 1;
    }
}

int main(){

    //abertura do arquivo de leitura
    std::ifstream program_template;
    program_template.open("program.txt", std::ifstream::in);

    if(program_template.is_open()){
        std::cout << "File opened successfully" << std::endl;
    }

    //abertura do arquivo de escrita da tabela
    std::ofstream table;
    table.open("table.txt", std::ofstream::out);

    if(table.is_open()){
        std::cout << "File created successfully" << std::endl;
    }

    //conjunto de regras regex indentificadoras
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

    //regras regex e variaveis para o tratamento de comentarios

    std::regex comment_same_line ("\\{.*\\}");

    std::regex comment_other_line ("\\{.*[^\\}]");

    std::regex closed_comment (".*\\}");

    bool comment_open = false;

    int open_comment_line = 0;

    //-----------------------------------------------------

    std::vector<std::string>key_words = {"program", "var", "integer", "real", "boolean", "procedure",
    "begin", "end", "if", "then", "else", "while", "do", "not"};

    std::string alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 \t\n{}();:.(),=<>+-*/";

    //contador de linhas
    int lines = 1;

    std::string str = "\0";

    //permanece em loop ate o fim do arquivo a ser lido
    while(program_template.peek() != EOF){
        
        
        std::smatch matches; //variavel para computar os "matches" de uma regra regex

        std::getline(program_template, str); //pega linha por linha do txt de entrada
        
        //checagem de cada caracter da string no alfabeto
        for(char x : str){
            check_char(x, lines, alphabet);
        }

        //procura um comentario que abre e fecha na mesma linha
        //caso exista ele apaga tudo que esta dentro do comentario
        while (std::regex_search(str, matches, comment_same_line)) {

            if(matches.empty()) break;

            else
            {
                str = std::regex_replace(str, comment_same_line, "");
            }
        
        }
        
        //caso tenha um comentario aberto em uma linha passada
        //ele checa se na leitura da linha atual possui um fechamento de comentario
        //caso exista, todos os caracteres antes do fechamento sao removidos
        //e a variavel "comment_open" se torna "false"

        //caso nao exista, ele soma uma linha ao contador de linhas e le a linha seguinte
        if(comment_open){
            if(str.find("}") != std::string::npos){
                str = std::regex_replace(str, closed_comment, "");
                comment_open = false;
            }
            else{
                ++lines;
                continue;
            }
        }

        //procura um comentario que foi aberto e nao fechado na mesma linha
        //caso ache, ele remove todos os caracteres da linha apos o comentario aberto
        //e tambem coloca a variavel "comment_open" para "true"
        while (std::regex_search(str, matches, comment_other_line)) {

            open_comment_line = lines; //salva a linha do comentario aberto

            if(matches.empty()) break;

            else
            {
                str = std::regex_replace(str, comment_other_line, "");
                comment_open = true;
            }
        }
        //em cada linha, sao checadas todas as regras do vetor "rules" na ordem que foram postas na declaracao do vetor
        //e os "matches" sao colocados no arquivo "table.txt" no formato "linha" "indentificado" "tipo"

        for(int i = 0; i < 8; i++){

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

    if(comment_open){
        std::cerr << "Erro: comment open in line: " << open_comment_line << "\n";
    }

    program_template.close();
    table.close();

    return 0;
}