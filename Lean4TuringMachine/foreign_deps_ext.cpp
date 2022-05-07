#include <stdio.h>
#include <cstring>
#include <vector>
#include <lean/lean.h>
#include <stdexcept>

std::vector<char*> split (char* input, char* delim){
    char* inputCopy = strdup(input);
    std::vector<char*> result;
    result.push_back(std::strtok(inputCopy, delim));
    char* next = strtok(NULL, delim);
    while (next){
        result.push_back(next);
        next = strtok(NULL, delim);
    }
    return result;
}

extern "C" lean_object * general_transition_function(lean_object *Q, lean_object *Gamma, lean_object *transitions, lean_object *table, lean_object *q, lean_object *s, double QLen, double GammaLen, lean_object /* w */) {
    // returns (String × (String × String)) of the form Q × Γ × {−1, 1}

    // convert Q from List String to a char* array
    char * QArray[(int)QLen];
    lean_object ** QCtor = (lean_to_ctor(Q)->m_objs);
    for (int i = 0; i < QLen-1; i++){
        QArray[i] = lean_to_string(QCtor[0])->m_data;
        QCtor = (lean_to_ctor(QCtor[1])->m_objs);
    }
    QArray[(int)QLen-1] = lean_to_string(QCtor[0])->m_data;

    // convert Gamma from List String to a char* array
    char * GammaArray[(int)GammaLen];
    lean_object ** GammaCtor = (lean_to_ctor(Gamma)->m_objs);
    for (int i = 0; i < GammaLen-1; i++){
        GammaArray[i] = lean_to_string(GammaCtor[0])->m_data;
        GammaCtor = (lean_to_ctor(GammaCtor[1])->m_objs);
    }
    GammaArray[(int)GammaLen-1] = lean_to_string(GammaCtor[0])->m_data;

    // convert transitions from (String × String) to a char* array
    char * transitionsArray[2] = {lean_to_string(lean_ctor_get(transitions, 0))->m_data, lean_to_string(lean_ctor_get(transitions, 1))->m_data};

    // get the program, current state, and current symbol as strings
    char * program = lean_to_string(table)->m_data;
    char * currentState = lean_to_string(q)->m_data;
    char * currentSymbol = lean_to_string(s)->m_data;
    if ((std::strcmp(currentSymbol, " ")==0)) currentSymbol = "\' \'";

    // split program into lines
    std::vector<char*> programLines = split(program, "\n");

    // remove any comments
    for (int i = 0; i < programLines.size(); i++){
        programLines.at(i) = split(programLines.at(i), "#").at(0);
    } 

    // make sure that the blank symbol is in Gamma
    for (int i = 0; i < programLines.size(); i++){
        char* blank = strstr(programLines.at(i), "blank");
        if (blank){
            std::vector<char*> blankChar = split(blank, "'");
            if (blankChar.size() < 2){
                throw "Turing Machine Program Syntax Error";
            }
            bool contained = false;
            for (int j = 0; j < GammaLen && !contained; j++){
                contained = (std::strcmp(blankChar.at(1), GammaArray[j])==0);
            }
            if (!contained){
                throw "Turing Machine Invalid Blank Symbol";
            }
            break;
        }
    }

    // make sure that the current state and symbol are valid
    bool valid = false;
    for (int i = 0; i < QLen; i++){
        valid = (std::strcmp(QArray[i], currentState)==0);
        if (valid) break;
    }
    if (!valid) throw "Turing Machine Invalid State";
    valid = false;
    for (int i = 0; i < GammaLen; i++){
        valid = (std::strcmp(GammaArray[i], currentSymbol)==0);
        if (valid) break;
    }
    if (!valid) throw "Turing Machine Invalid Symbol";

    // find the transition based on the table
    int table_line = -1;
    for (int i = 0; i < programLines.size(); i++){
        if (strstr(programLines.at(i), "table")){
            table_line = i;
            break;
        }
    }
    if (table_line == -1){
        throw "Turing Machine Program Syntax Error"; // table was not found
    }
    char* currentSymbolPossibleOrientations[] = {
        ' '+currentSymbol, ' '+currentSymbol+' ', ' '+currentSymbol+']', ' '+currentSymbol+',', ' '+currentSymbol+'\'', ' '+currentSymbol+'$',
        ','+currentSymbol, ','+currentSymbol+' ', ','+currentSymbol+']', ','+currentSymbol+',', ','+currentSymbol+'\'', ','+currentSymbol+'$',
        '['+currentSymbol, '['+currentSymbol+' ', '['+currentSymbol+']', '['+currentSymbol+',', '['+currentSymbol+'\'', '['+currentSymbol+'$',
        '\''+currentSymbol, '\''+currentSymbol+' ', '\''+currentSymbol+']', '\''+currentSymbol+',', '\''+currentSymbol+'\'', '\''+currentSymbol+'$'
    }; // array of possible ways that the current symbol could appear in context in the table
    for (int i = table_line+1; i < programLines.size(); i++){
        if (strstr(programLines.at(i), currentState)){
            try{
                std::vector<char*> nextLine;
                bool reachedEnd = false;
                bool reachedSymbol = false;
                int steps = 1;
                while (!reachedEnd){
                    nextLine = split(programLines.at(i+steps), ":");
                    std::strcat(nextLine.at(0), "$");
                    for (int j = 0; j < QLen && !reachedEnd; j++){ // see if the end of this table entry has been reached
                        if (strstr(nextLine.at(0), QArray[j])) reachedEnd = true;
                    }
                    for (int j = 0; j < sizeof(currentSymbolPossibleOrientations) && !reachedSymbol; j++){ // check if this line specifies the action to be taken for the current symbol
                        if (strstr(nextLine.at(0), currentSymbolPossibleOrientations[j])) reachedSymbol = true;;
                    }
                    if (reachedEnd && !reachedSymbol) throw std::out_of_range("end was reached without finding symbol"); // the end was reached without finding the symbol
                    else if (reachedSymbol){
                        programLines.at(i+steps) = currentSymbol;
                        break;
                    }
                    steps++;
                }

                break;
            }
            catch (const std::out_of_range& e) {
                throw "Turing Machine Program Syntax Error";
            }
        }
    }

    lean_object * returnState = lean_mk_string(currentState);
    lean_object * returnSymbol = lean_mk_string(currentSymbol);
    lean_object * returnTransition = lean_mk_string(programLines.at(8));

    // make sure that the return state and symbol are valid

    lean_object * res_1 = lean_alloc_ctor(0, 2, 0);
    lean_object * res_2 = lean_alloc_ctor(0, 2, 0);

    lean_ctor_set(res_2, 0, returnSymbol);
    lean_ctor_set(res_2, 1, returnTransition); // (Γ × {−1, 1})
    lean_ctor_set(res_1, 0, returnState);
    lean_ctor_set(res_1, 1, res_2); // (Q × (Γ × {−1, 1}))

    return res_1;
}
