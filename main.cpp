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
int check_char(char c, unsigned int line, unsigned int current_state){

    for(unsigned char cmp = 0; cmp < 255; cmp++){
        if(cmp == 38){
            continue;
        }
        if(c == cmp){
            return 1;
        }
    }
    if(current_state != 1){
        std::cerr << "Line " << line << " "<< c << " does not belong to the language alphabet\n";
    }
    return 0;
}


typedef struct
{
    const std::string name;
    const std::regex pattern;
} Pattern;

int main(){

    //abertura do arquivo de leitura
    std::ifstream program_template;
    program_template.open("program.txt", std::ifstream::in);

    if(program_template.is_open()){
        std::cout << "File opened successfully" << std::endl;
    }

    //abertura do arquivo de escrita da tabela
    std::ofstream output_file;
    output_file.open("output_file.txt", std::ofstream::out);

    if(output_file.is_open()){
        std::cout << "File created successfully" << std::endl;
    }

    //conjunto de regras regex indentificadoras
    std::vector<Pattern> rules = {
        {"words", std::regex("[a-zA-Z]\\w*")}, //words
        {"float", std::regex("\\d+[.]\\d*")}, //float
        {"interger", std::regex("\\d+")}, //integers
        {"attribution", std::regex(":=")}, //attribuition
        {"delimiter", std::regex(":(?!=)|[;]|[.]|[,]|[(]|[)]")}, //delimiters
        {"relational operators", std::regex("(?![<|>])=|[<|>][=]|[<][>]|(?!<)>|[<]")}, //relational operators
        {"additive operators", std::regex("[+]|[-]")}, //additive operators
        {"multiplicative operators", std::regex("[*]|[/]")} //multiplicative operators
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

    //contador de linhas
    int lines = 1;

    std::string str = "\0";

    //permanece em loop ate o fim do arquivo a ser lido
    while(program_template.peek() != EOF){

    
        std::smatch matches; //variavel para computar os "matches" de uma regra regex

        std::getline(program_template, str); //pega linha por linha do txt de entrada

        std::string::const_iterator start = str.begin(), end = str.end();
        int match_size, match_position;      

        //checagem de cada caracter da string no alfabeto
        for(char x : str){
            check_char(x, lines, 0);
        }

        //procura um comentario que abre e fecha na mesma linha
        //caso exista ele apaga tudo que esta dentro do comentario
        if (std::regex_search(str, matches, comment_same_line)) {
            if(matches.empty()) continue;

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
        if(std::regex_search(str, matches, comment_other_line)) {
            open_comment_line = lines; //salva a linha do comentario aberto

            if(matches.empty()){}

            else
            {   
                str = std::regex_replace(str, comment_other_line, "");
                comment_open = true;
                continue;
            }
        }
        //em cada linha, sao checadas todas as regras do vetor "rules" na ordem que foram postas na declaracao do vetor
        //e os "matches" sao colocados no arquivo "output_file.txt" no formato "linha" "indentificado" "tipo"

        std::string type, token;

        while (true)
        {
            if(comment_open) break;
            match_size = 0;
            match_position = str.size();

            for (auto pattern : rules)
            {
                // procura por uma match por todos os padrões
                if (std::regex_search(start, end, matches, pattern.pattern))
                {
                    // Se a procura resultou em um match, duas condições são verificadas:

                    // 1: Caso a posição do match corrente seja menor do que a menor posição previamente escolhida, o match corrente torna-se o escolhido
                    // Em outras palavras, o match mais a esquerda é escolhido
                    if (matches.position() < match_position)
                    {
                        match_position = matches.position();
                        match_size = matches.length();
                        type = pattern.name;
                        token = matches[0];
                    }
                    // 2: Caso a posição do match corrente coincida com o match previamente escolhido, torna-se o escolhido aquele que tem mais caracteres
                    // Importante para tratar os casos de <= e <, em que ambos começam no mesmo lugar, porém o <= contém mais caracteres
                    else if (matches.position() == match_position)
                    {
                        if (matches.length() > match_size)
                        {
                            match_size = matches.length();
                            type = pattern.name;
                            token = matches[0];
                        }
                    }
                }
            }

            // Caso a busca não encontre um match, interrompe o laço e vai para a proxima linha (se possivel)

            if (match_size == 0)
                break;
            

            if(type == "words"){
                if(in_array(token, key_words)){
                    type = "key_word";
                }else{
                    type = "identifier";
                }
            }

            // A escrita é performada no arquivo de saída
            output_file << lines << ' ' << token << ' ' << type << std::endl;
            // std::cout << lines << ' ' << token << ' ' << type << std::endl;

            // Atualiza-se o inicio para logo após ao match encontrado
            start += match_position + match_size;
        }

        ++lines;
    }

    if(comment_open){
        std::cerr << "Erro: comment open in line: " << open_comment_line << "\n";
    }

    program_template.close();
    output_file.close();

    return 0;
}