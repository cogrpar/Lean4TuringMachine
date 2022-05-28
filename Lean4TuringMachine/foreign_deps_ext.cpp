#include <stdio.h>
#include <cstring>
#include <vector>
#include <lean/lean.h>
#include <stdexcept>

char* copy (char* input){
  char* output = new char[strlen(input)+1];
  for (int i = 0; i < strlen(input); i++){
    output[i] = input[i];
  }
  output[strlen(input)] = '\0';
  return output;
}

std::vector<char*> split (char* input, char* delim){
    char* inputCopy = copy(input);
    std::vector<char*> result;
    result.push_back(strtok(inputCopy, delim));
    char* next = strtok(NULL, delim);
    while (next){
        result.push_back(next);
        next = strtok(NULL, delim);
    }
    return result;
}

extern "C" lean_object * general_transition_function(lean_object * Q, lean_object * Gamma, lean_object * q_accept, lean_object * q_reject, lean_object * transitions, lean_object * table, lean_object * q, lean_object * s, double QLen, double GammaLen, lean_object /* w */) {
    // returns (String × (String × String)) of the form Q × Γ × {−1, 1}

    // convert Q from List String to a char* array
    char* QArray[(int)QLen];
    lean_object ** QCtor = (lean_to_ctor(Q)->m_objs);
    for (int i = 0; i < QLen-1; i++){
        QArray[i] = lean_to_string(QCtor[0])->m_data;
        QCtor = (lean_to_ctor(QCtor[1])->m_objs);
    }
    QArray[(int)QLen-1] = lean_to_string(QCtor[0])->m_data;

    // convert Gamma from List String to a char* array
    char* GammaArray[(int)GammaLen];
    lean_object ** GammaCtor = (lean_to_ctor(Gamma)->m_objs);
    for (int i = 0; i < GammaLen-1; i++){
        GammaArray[i] = lean_to_string(GammaCtor[0])->m_data;
        GammaCtor = (lean_to_ctor(GammaCtor[1])->m_objs);
    }
    GammaArray[(int)GammaLen-1] = lean_to_string(GammaCtor[0])->m_data;

    // get accept and reject states as char arrays
    char* accept = lean_to_string(q_accept)->m_data;
    char* reject = lean_to_string(q_reject)->m_data;

    // convert transitions from (String × String) to a char* array
    char* transitionsArray[2] = {lean_to_string(lean_ctor_get(transitions, 0))->m_data, lean_to_string(lean_ctor_get(transitions, 1))->m_data};

    // get the program, current state, and current symbol as strings
    char* program = lean_to_string(table)->m_data;
    char* currentState = lean_to_string(q)->m_data;
    char* currentSymbol = lean_to_string(s)->m_data;
    if ((strcmp(currentSymbol, " ")==0)) currentSymbol = "\' \'";

    // split program into lines
    std::vector<char*> programLines = split(program, "\n");

    // remove any comments
    for (int i = 0; i < programLines.size(); i++){
        programLines.at(i) = split(programLines.at(i), "#").at(0);
    } 

    lean_object * returnState;
    lean_object * returnSymbol;
    lean_object * returnTransition;

    try{
        // make sure that the blank symbol is in Gamma
        for (int i = 0; i < programLines.size(); i++){
            char* blank = strstr(programLines.at(i), "blank");
            if (blank){
                std::vector<char*> blankChar = split(blank, "'");
                if (blankChar.size() < 2){
                    throw "Turing Machine Program Syntax Error";
                }
                if ((strcmp(blankChar.at(1), " ")==0)) blankChar.at(1) = "\' \'";
                bool contained = false;
                for (int j = 0; j < GammaLen && !contained; j++){
                    if ((strcmp(GammaArray[j], " ")==0)) GammaArray[j] = "\' \'";
                    contained = (strcmp(blankChar.at(1), GammaArray[j])==0);
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
            valid = (strcmp(QArray[i], currentState)==0);
            if (valid) break;
        }
        if (!valid) throw "Turing Machine Invalid State";
        valid = false;
        for (int i = 0; i < GammaLen; i++){
            valid = (strcmp(GammaArray[i], currentSymbol)==0);
            if (valid) break;
        }
        if (!valid) throw "Turing Machine Invalid Symbol";

        // find the transition based on the table
        char* retSy = "";
        char* retTr = "";
        char* retSt = "";
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
        char symbolPossibleOrientations[20][2] = {
            {' ', ' '}, {' ', ']'}, {' ', ','}, {' ', '\''}, {' ', '\0'},
            {',', ' '}, {',', ']'}, {',', ','}, {',', '\''}, {',', '\0'},
            {'[', ' '}, {'[', ']'}, {'[', ','}, {'[', '\''}, {'[', '\0'},
            {'\'', ' '}, {'\'', ']'}, {'\'', ','}, {'\'', '\''}, {'\'', '\0'}
        }; // array of possible ways that the current symbol could appear in context in the table
        for (int i = table_line+1; i < programLines.size(); i++){ 
            char* stateOrientation = new char[strlen(currentState)+3]; // check for (' '+currentState+':')
            stateOrientation[0] = ' ';
            for (int j = 0; j < strlen(currentState); j++){
                stateOrientation[j+1] = currentState[j];
            }
            stateOrientation[strlen(currentState)+1] = ':';
            stateOrientation[strlen(currentState)+2] = '\0';
            if (strstr(programLines.at(i), stateOrientation)){
                try{
                    std::vector<char*> nextLine;
                    bool reachedEnd = false;
                    bool reachedSymbol = false;
                    int steps = 1;
                    while (!reachedEnd){
                        nextLine = split(programLines.at(i+steps), ":");
                        for (int j = 0; j < QLen && !reachedEnd; j++){ // see if the end of this table entry has been reached
                            if (strstr(nextLine.at(0), QArray[j])) reachedEnd = true;
                        }
                        for (int j = 0; j < 20 && !reachedSymbol; j++){ // check if this line specifies the action to be taken for the current symbol
                            char* possibleOrientation = new char[strlen(currentSymbol)+3];
                            possibleOrientation[0] = symbolPossibleOrientations[j][0];
                            for (int k = 0; k < strlen(currentSymbol); k++){
                                possibleOrientation[k+1] = currentSymbol[k];
                            }
                            possibleOrientation[strlen(currentSymbol)+1] = symbolPossibleOrientations[j][1];
                            possibleOrientation[strlen(currentSymbol)+2] = '\0';
                            if (strstr(nextLine.at(0), possibleOrientation)) reachedSymbol = true;
                        }
                        if (reachedEnd && !reachedSymbol){
                            // the end was reached without finding the symbol, ie. the machine has halted
                            retSt = accept;
                            retSy = accept;
                            retTr = accept;
                        }
                        else if (!reachedEnd && reachedSymbol){
                            // located table entry for this machine configuration
                            if (strstr(nextLine.at(1), "write")){
                                // remove spaces
                                int count = 0;
                                for (int j = 0; nextLine.at(2)[j]; j++)
                                    if (nextLine.at(2)[j] != ' ')
                                        nextLine.at(2)[count++] = nextLine.at(2)[j];
                                nextLine.at(2)[count] = '\0';
                                retSy = split(nextLine.at(2), ",").at(0); // symbol

                                if (strcmp(split(nextLine.at(2), ",").at(1), "R")==0) retTr = "+1";
                                else retTr = "-1"; // transition

                                if (nextLine.size() > 3){
                                    count = 0;
                                    for (int j = 0; nextLine.at(3)[j]; j++)
                                        if (nextLine.at(3)[j] != ' ' && nextLine.at(3)[j] != '}')
                                            nextLine.at(3)[count++] = nextLine.at(3)[j];
                                    nextLine.at(3)[count] = '\0';
                                    retSt = nextLine.at(3);
                                }
                                else retSt = currentState; // state
                            }
                            else if (nextLine.size() == 3){
                                retSy = currentSymbol; //symbol

                                if (strstr(nextLine.at(1), "R")) retTr = "+1";
                                else retTr = "-1"; // transition

                                int count = 0;
                                for (int j = 0; nextLine.at(2)[j]; j++)
                                    if (nextLine.at(2)[j] != ' ' && nextLine.at(2)[j] != '}')
                                        nextLine.at(2)[count++] = nextLine.at(2)[j];
                                nextLine.at(2)[count] = '\0';
                                retSt = nextLine.at(2); // state
                            }
                            else if (nextLine.size() == 2){
                                retSt = currentState; // state
                                retSy = currentSymbol; //symbol
                                
                                if (strstr(nextLine.at(1), "R")) retTr = "+1";
                                else retTr = "-1"; // transition
                            }
                            else{
                                 throw std::out_of_range("invalid instructions");
                            }

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

        returnState = lean_mk_string(retSt);
        returnSymbol = lean_mk_string(retSy);
        returnTransition = lean_mk_string(retTr);

        // make sure that the return state and symbol are valid
        valid = false;
        for (int i = 0; i < QLen; i++){
            valid = (strcmp(QArray[i], retSt)==0);
            if (valid) break;
        }
        if (strcmp("halted", retSt)==0) valid = true;
        if (!valid) throw "Turing Machine Invalid State";
        valid = false;
        for (int i = 0; i < GammaLen; i++){
            valid = (strcmp(GammaArray[i], retSy)==0);
            if (valid) break;
        }
        if (strcmp("halted", retSy)==0) valid = true;
        if (!valid) throw "Turing Machine Invalid Symbol";
        valid = false;
        if (strcmp(transitionsArray[0], retTr)==0 || strcmp(transitionsArray[1], retTr)==0 || strcmp("halted", retTr)==0) valid = true;
        if (!valid) throw "Turing Machine Invalid Transition";
    }
    catch (char const* c){ // turing machine error, pass it to returned objects with the reject state
        returnState = lean_mk_string(reject);
        returnSymbol = lean_mk_string(c);
        returnTransition = lean_mk_string("+1");
    }

    lean_object * res_1 = lean_alloc_ctor(0, 2, 0);
    lean_object * res_2 = lean_alloc_ctor(0, 2, 0);

    lean_ctor_set(res_2, 0, returnSymbol);
    lean_ctor_set(res_2, 1, returnTransition); // (Γ × {−1, 1})
    lean_ctor_set(res_1, 0, returnState);
    lean_ctor_set(res_1, 1, res_2); // (Q × (Γ × {−1, 1}))

    return res_1;
}

extern "C" lean_object * check_mem (lean_object * elem, lean_object * list, double listLen, lean_object /* w */){
    // function to check list membership

    // convert list from List String to a char* array
    char* listArray[(int)listLen];
    lean_object ** listCtor = (lean_to_ctor(list)->m_objs);
    for (int i = 0; i < listLen-1; i++){
        listArray[i] = lean_to_string(listCtor[0])->m_data;
        listCtor = (lean_to_ctor(listCtor[1])->m_objs);
    }
    listArray[(int)listLen-1] = lean_to_string(listCtor[0])->m_data;

    // convert elem from lean_object to char array
    char* elemString = lean_to_string(elem)->m_data;

    // check to see if elemString is a member of listArray
    bool member = false;
    for (int i = 0; i < listLen; i++){
        if (strcmp(listArray[i], elemString)==0){
            member = true;
            break;
        }
    }

    if (member) return (lean_object *)true; // check membership
    else return (lean_object *)false;
}