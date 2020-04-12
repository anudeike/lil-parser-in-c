/* front.c - a lexical analyzer system for simple
 arithmetic expressions */
#include <stdio.h>
#include <ctype.h>
#include <string>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "string.h"
#include <map>
#include <iostream>

/* Global declarations */
/* Variables */
int charClass;
char lexeme [100];
char nextChar;
int lexLen;
int size = 0;
int nextToken = -2;
FILE *in_fp, *fopen();

struct Info{
    // initializing for debugging purposes
    int code = -50; // this handles datatype and keywords
    std::string scope = "global"; // name of the local scope it is in
    int value = 0; // we only process ints rn, will also be able to handle functions with a special number.

    void showContents(){
        std::cout << "Data Type: " << code << "\n";
        std::cout << "Scope: " << scope << "\n";
        std::cout << "Value " << value << "\n";
    }
};

// create a symbol table Hash Map
std::map<std::string, Info> SymbolTableHashMap;

/* Function declarations */
void addChar();
void getChar();
void getNonBlank();
int lex();

/* Character classes */
#define LETTER 0
#define DIGIT 1
#define UNKNOWN 99

/* Token codes */
#define NEWLINE 2

#define INT_LIT 10
#define IDENT 11
#define ASSIGN_OP 20
#define ADD_OP 21
#define SUB_OP 22
#define MULT_OP 23
#define DIV_OP 24
#define LEFT_PAREN 25
#define RIGHT_PAREN 26
#define OPEN_BRACKET 27
#define CLOSE_BRACKET 28

// keywords
#define FUNC_KEYWORD 50
#define TARGET_KEYWORD 51

// type structs


// function prototypes for just expr()
void expr();
void stmt();
void funcdef();

// define the grammar
/*
 * grammar for a function
 * <program> -> # <target> <funcdef>
 * <target> -> "target" <lang>
 * <lang> -> "c" | "python" | "cpp"
 *
 * -- Already implemented
 * <funcdef> -> func <func_name> "(" ")" "{" <function_body> "}
 * <stmt> -> id = <expr> ;
   <expr> → <term> {(+ | -) <term>}
   <term> → <factor> {(* | /) <factor>}
   <factor> → id | int_constant | ( <expr> )

   playground compatible grammar
   <stmt> ::= <id> "=" <expr>
   <expr> ::= <term> | ("+" | "-") <term>
   <term> ::= <factor> | ("*" | "/") <factor>
   <factor> ::= <id> | <int_literal> | <expr>
   <id> ::= [a-z]+
   <int_literal> ::= [0-9]+
*/

// Utility Functions
void consumeNewline(){
    while(nextToken == NEWLINE){
        lex();
    }
}

// get the size of the file
// get the size
off_t fsize(const char *filename) {
    struct stat st;

    if (stat(filename, &st) == 0)
        return st.st_size;

    return -1;
}

// for viewing the symbol table
void displaySymbolTable(std::map<std::string, Info> table){
    std::map<std::string, Info>::iterator itr;
    printf("\nSymbol Table Contents\n");
    for(itr = table.begin(); itr != table.end(); itr++){
        printf()
    }
}

/* main driver */
int main() {
    // save the file path
    char path[] = "C:\\Users\\Ikechukwu Anude\\CLionProjects\\RDP_test\\front.in";

/* Open the input data file and process its contents */
    if ((in_fp = fopen(path, "r")) == NULL)
        // open the file
        printf("ERROR - cannot open front.in \n");
    else {
        // get the size of the file
        size = fsize(path);

        // get characters and lex()?
        getChar();
        do {
            lex();
            funcdef();
        } while (nextToken != EOF);
    }
}

void error(const char * msg){
    printf("ERROR: %s\n", msg);
    exit(1);
}

void funcdef() {
    printf("Enter <funcdef> \n");

    // while next token is a newLine
    while(nextToken == NEWLINE){

        // get the next token
        lex();
    }

    //if "func"
    if (nextToken == FUNC_KEYWORD){
        lex(); // get next token

        //if the next token is an identifier, it must be the function name
        if(nextToken == IDENT){
            lex();

            // if the next token is a left paren
            if (nextToken == LEFT_PAREN){
                lex();

                // if the next token is a right paren
                if(nextToken == RIGHT_PAREN){
                    lex();

                    // if the next token is open bracket
                    if(nextToken == OPEN_BRACKET){
                        lex();

                        consumeNewline();

                        while(nextToken == IDENT){
                            stmt();
                            consumeNewline();
                        }
//                        if(nextToken == IDENT){
//                            stmt();
//                        }
//
//                        // while next token is a newLine
//                        consumeNewline();
//
//                        // parse a statement
//                        stmt();
//
//
////                        // while the next token is an identifier
////                        while(nextToken == IDENT){
////                            printf("called\n\n");
////                            stmt();
////                        }
//
//                        // consumeNewLine
//                        consumeNewline();
//
//                        if(nextToken == IDENT){
//                            stmt();
//                        }
//
//                        consumeNewline();

                        // find a close bracket
                        if(nextToken == CLOSE_BRACKET){
                            printf("reached the end of the function.\n");
                            lex();
                        }else
                            error("Must close function");
                    } else
                        error("Must open function.");
                } else
                    error("Parenthesis not found.");
            } else
                error("Parenthesis not found.");
        }else{
            error("Function must have a name.");
        }
    }else {
        error("Need to define a function");
    }

    printf("Exit <funcdef>\n");
}


// the stmt function
void stmt() {
    printf("Enter <stmt>\n");

    while (nextToken == IDENT){

        lex(); // get next token

        if (nextToken == ASSIGN_OP){
            lex();
            expr();
        }
    }

    printf("Exit <stmt>\n");

}
void factor(){
    // must choose between two RHSs
    printf("Enter <factor>\n");

    // determine the RHSs
    if(nextToken == IDENT || nextToken == INT_LIT){
        lex(); // get next token
    }
        // if the RHS is <expr> tell the lex to pass over the left parenthesis and call expr and then check for right parentheses
    else {
        if(nextToken == LEFT_PAREN){
            lex();
            expr();
            if(nextToken == RIGHT_PAREN){
                lex();
            }else {
                error("error in factor"); // should exist at the very top
            }
        }
        else {
            // it wasnt an id or int or left parentheses
            error("Error in factor no left paren");
        }
    }

    printf("Exit <factor>\n");
}

// parse term
void term() {
    printf("Enter <term>\n");

    // parse the first factor
    factor();

    // as long as the next is * or / get the next token and parse the next factor
    while (nextToken == MULT_OP || nextToken == DIV_OP){
        lex(); // get the next token
        factor(); // parse the next factor
    }
    printf("Enter <expr> \n");
}

// parse expression
void expr() {
    printf("Enter <expr> \n");

    // parse first term
    term();

    // as long as the next token is + or -, get the next token and parse the next term
    while(nextToken == ADD_OP || nextToken == SUB_OP){
        // updates next token
        lex();

        // proocesses the term
        term();
    }
    printf("Exit <expr> \n");
}



/*****************************************************/
/* lookup - a function to lookup operators and parentheses
 and return the token */
int lookup(char ch) {
    switch (ch) {
        case '(':
            addChar();
            nextToken = LEFT_PAREN;
            break;
        case ')':
            addChar();
            nextToken = RIGHT_PAREN;
            break;
        case '+':
            addChar();
            nextToken = ADD_OP;
            break;
        case '-':
            addChar();
            nextToken = SUB_OP;
            break;
        case '*':
            addChar();
            nextToken = MULT_OP;
            break;
        case '/':
            addChar();
            nextToken = DIV_OP;
            break;
        case '=':
            // if the equal sign then set as assign op -> not sure what add char does
            //printf("[FROM lookup()] found the = \n");
            addChar();
            nextToken = ASSIGN_OP;
            break;

        case '{':
            addChar();
            nextToken = OPEN_BRACKET;
            break;

        case '}':
            addChar();
            nextToken = CLOSE_BRACKET;
            break;

        case '\n':

            // signify we hit a new line.
            nextToken = NEWLINE;
            lexeme[0] = 'N';
            lexeme[1] = 'L';
            lexeme[2] = 0;
            break;

        default:
            addChar();
            nextToken = EOF;
            break;
    }
    return nextToken;
}
/*****************************************************/
/* addChar - a function to add nextChar to lexeme */
void addChar() {
    if (lexLen <= 98) {
        lexeme[lexLen++] = nextChar;
        lexeme[lexLen] = 0;
    }
    else
        printf("Error - lexeme is too long \n");
}
/*****************************************************/
/* getChar - a function to get the next character of
 input and determine its character class */
void getChar() {
    if ((nextChar = getc(in_fp)) != EOF){
        //printf("\n[FROM getChar()] nextChar = %c \n\n",nextChar);
        if (isalpha(nextChar))
            charClass = LETTER;
        else if (isdigit(nextChar))
            charClass = DIGIT;
        else charClass = UNKNOWN;
    }
    else
        charClass = EOF;
}
/*****************************************************/
/* getNonBlank - a function to call getChar until it
 returns a non-whitespace character */
void getNonBlank() {
    // while is blank and not a newline (we can handle that)
    while (isspace(nextChar) && nextChar != '\n')
        getChar();
}
/*****************************************************/
/* lex - a simple lexical analyzer for arithmetic
 expressions */
int lex() {
    lexLen = 0;
    getNonBlank();
    switch (charClass) {

/* Parse identifiers */
        case LETTER:
            addChar();
            getChar();

            while (charClass == LETTER || charClass == DIGIT) {
                addChar();
                getChar();
            }

            // if the identifier matches func then set the token to the func keyword
            if ((strcmp(lexeme, "func")) == 0){
                nextToken = FUNC_KEYWORD;
                break;
            }

            // if the identifier matches the word target
            if((strcmp(lexeme, "target")) == 0){
                nextToken = TARGET_KEYWORD;
                break;
            }


            nextToken = IDENT;
            break;

/* Parse integer literals */
        case DIGIT:
            addChar();
            getChar();

            while (charClass == DIGIT) {
                addChar();
                getChar();
            }

            nextToken = INT_LIT;
            break;

/* Parentheses and operators */
        case UNKNOWN:
            //printf("[FROM lex()] nextChar %c\n",nextChar);
            lookup(nextChar);
            getChar();
            break;

/* EOF */
        case EOF:
            nextToken = EOF;
            lexeme[0] = 'E';
            lexeme[1] = 'O';
            lexeme[2] = 'F';
            lexeme[3] = 0;
            break;
    } /* End of switch */

    printf("Next token is: %d, Next lexeme is %s\n", nextToken, lexeme);


    return nextToken;
} /* End of function lex */
