//Author: Chupakhin Andrey
//Year: September, 2015
/* Данная программа вычисляет и записывает в текстовой файл states.txt множество достижимых состояний
 * заданной программы для заданных значений параметров функций, а также выводит в стандартный
 * поток вывода количество достижимых состояний.
*/

/*
 * FUNCTION F -> RED COLOR
 * FUNCTION G -> BLUE COLOR
 */

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <limits.h>
#include <string.h>
#include <fstream>

#define fLines 9
#define gLines 13

//GLOBAL VARIABLES -> <FILENAME< <INPUT PARAMETERS FOR PROCEDURE F AND G> <COUNT FLAG>
//F_Procedure line - {0}, G_Procedure line - {1}
//Implementation of the program is represented by the binary vector
//Weight of the vector is a constant. Weight == gLines
std::string filename("states.txt");
std::string filenameLTS;
int f_a, f_b, g_a, g_b;
int count = 0;
int lts = 0;
int file = 0;
uint START_PATH;// 0000000001111111111111
uint END_PATH;  // 1111111111111000000000
//

void initPathes() {
    START_PATH = 1;
    for (int i = 0; i < gLines-1; ++i ) {
        START_PATH = START_PATH << 1;
        START_PATH |= 1;
    }
    END_PATH = START_PATH << fLines;
}

int correctTrace( const uint & trace ) {
    //Trace is correct if weight == gLines and length == (fLines + gLines)
    char ones = 0;
    for (uint i = 0; i < (fLines + gLines); i++){
        if ( (trace >> i) & 1 )
            ones += 1;
    }
    if ( ones == gLines )
        return 1;

    return 0;
}

class State {
public:
    int f_x;
    int f_y;
    int g_x;
    int g_y;
    int h;
    int c_f;
    int c_g;
    std::vector<int> init;//array for initialization control of the local variables { h, f_x, f_y, g_x, g_y }
public:
    State( const int & x_f = 0, const int & y_f = 0, const int & x_g = 0, const int & y_g = 0, const int & h_fg = 0,
           const int & f_c = 0, const int & g_c = 0 ):
            f_x(x_f), f_y(y_f), g_x(x_g), g_y(y_g), h(h_fg), c_f(f_c), c_g(g_c)
    {
        init = std::vector<int>(5,0);
    }

    ~State(){}

    void print( std::ofstream & out) {
        int fIp = c_f;
        int gIp = c_g;

        //Print with convertion
        if ( c_f == 6 || c_f == 7 )
            fIp +=1;
        if ( c_f == 8 )
            fIp = 12;

        if ( c_g >= 6 && c_g <= 11 )
            gIp +=1;

        if ( c_g == 12 )
            gIp = 18;


        out <<
        fIp << "  " <<
        gIp << "  ";

        if (init[0])
            out << h << "  ";
        else
            out << "#" << "  ";

        if (init[1])
            out << f_x << "  ";
        else
            out << "#" << "  ";

        if (init[2])
            out << f_y << "  ";
        else
            out << "#" << "  ";

        if (init[3])
            out << g_x << "  ";
        else
            out << "#" << "  ";

        if (init[4])
            out << g_y;
        else
            out << "#";

        out << std::endl;
    }
    bool operator==( const State & right) {
        return ( this->c_f == right.c_f ) && ( this->c_g == right.c_g ) &&
                ( ( this->init[0] == right.init[0] ) && ( this->h == right.h ) ) &&
                ( ( this->init[1] == right.init[1] ) && ( this->f_x == right.f_x ) ) &&
                ( ( this->init[2] == right.init[2] ) && ( this->f_y == right.f_y ) ) &&
                ( ( this->init[3] == right.init[3] ) && ( this->g_x == right.g_x ) ) &&
                ( ( this->init[4] == right.init[4] ) && ( this->g_y == right.g_y ) );

    }

    State & operator=(const State & right ) {
        f_x = right.f_x;
        f_y = right.f_y;
        g_x = right.g_x;
        g_y = right.g_y;
        h = right.h;
        c_f = right.c_f;
        c_g = right.c_g;
        return *this;
    }

    void printDOT( std::ofstream & out) {
        int fIp = c_f;
        int gIp = c_g;

        //Print with convertion
        if ( c_f == 6 || c_f == 7 )
            fIp +=1;
        if ( c_f == 8 )
            fIp = 12;

        if ( c_g >= 6 && c_g <= 11 )
            gIp +=1;

        if ( c_g == 12 )
            gIp = 18;


        out << "[label=\"" <<
        fIp << "," <<
        gIp << ",";

        if (init[0])
            out << h << ",";
        else
            out << "#" << ",";

        if (init[1])
            out << f_x << ",";
        else
            out << "#" << ",";

        if (init[2])
            out << f_y << ",";
        else
            out << "#" << ",";

        if (init[3])
            out << g_x << ",";
        else
            out << "#" << ",";

        if (init[4])
            out << g_y;
        else
            out << "#";

        out << "\"];" << std::endl;
    }

};

class Action {
public:
    State currState;
    State nextState;
    int curr;
    int next;
    std::string operation;
    Action() :
            currState(), nextState(), curr(0), next(0), operation()
    {};

    ~Action() {};

    void printDOT( std::ofstream & out) {
        std::string color = std::string("blue");
        if ( nextState.c_f > currState.c_f )
            color = std::string("red");
        out << curr << " -> " << next << "[label=\"" << operation << " " <<
        "color=\"" << color << "\"];" << std::endl;
    }

    bool operator==( const Action & right ) {
        return ( currState == right.currState &&
                nextState == right.nextState &&
                curr == right.curr &&
                next == right.next &&
                operation == right.operation );
    }

};

class Procedure {
public:
    Procedure() {}
    ~Procedure() {}
    virtual int execute(State & curr_state, const int & currComm, Action & action, int curr) const = 0;
};

class F: public Procedure {
public:
    F():Procedure() {}
    ~F() {}
    int execute(State & curr_state, const int & currComm, Action & action, int curr) const {
        //execute operation in ip -> change State and change ip
        // curr_state -> execute command -> new_state and new value in ip
        // if currComm < ip therefore not implement string
        if ( curr_state.c_f > currComm )
            return 0;

        //Set current State in action
        action.currState = curr_state;
        action.curr = curr;//number of state in vector with states <=> states[curr] == curr_state

        //std::cout << "F::";
        //else only one the rest variant: currComm == ip  => implement string
        switch (curr_state.c_f) {
            case  0:
                //std::cout << "int x, y" << std::endl;
                action.operation = std::string("int x, y;");
                curr_state.c_f = 1;
                action.nextState = curr_state;
                break;

            case  1:
                //std::cout << "x = 7" << std::endl;
                action.operation = std::string("x = 7;");
                curr_state.f_x = 7;
                curr_state.init[1] = 1;
                curr_state.c_f = 2;
                action.nextState = curr_state;
                break;

            case  2:
                //std::cout << "y = 7" << std::endl;
                action.operation = std::string("y = 7;");
                curr_state.f_y = 7;
                curr_state.init[2] = 1;
                curr_state.c_f = 3;
                action.nextState = curr_state;
                break;

            case  3:
                //std::cout << "h = 4" << std::endl;
                action.operation = std::string("h = 4;");
                curr_state.h = 4;
                curr_state.init[0] = 1;
                curr_state.c_f = 4;
                action.nextState = curr_state;
                break;

            case  4:
                //std::cout << "if ( h < b )" << std::endl;
                //( curr_state.h < f_b) ? curr_state.c_f = 5 : curr_state.c_f = 7;

                if ( curr_state.h < f_b) {
                    action.operation = std::string("( h < b )");
                    curr_state.c_f = 5;
                    action.nextState = curr_state;
                } else {
                    action.operation = std::string("!( h < b )");
                    curr_state.c_f = 7;
                    action.nextState = curr_state;
                }

                break;

            case  5:
                //if ( h > 6 )  in all cases FALSE
                //std::cout << "if ( h > 6 )" << std::endl;
                action.operation = std::string("!( h > 6 )");
                curr_state.c_f = 6;
                action.nextState = curr_state;
                break;

            case  6:
                //std::cout << "x = 2" << std::endl;
                action.operation = std::string("x = 2;");
                curr_state.f_x = 2;
                curr_state.init[1] = 1;
                curr_state.c_f = 7;
                action.nextState = curr_state;
                break;

            case  7:
                //std::cout << "if ( h > y )" << std::endl;
                action.operation = std::string("!( h > y )");
                curr_state.c_f = 8;
                action.nextState = curr_state;
                break;

            case  8:
                //end of the procedure
                //Infinite loop in last state
                action.operation = std::string("END");
                action.nextState = curr_state;
                action.next = curr;
                break;

            default:
                std::cout <<  "Not found appropriate command" << std::endl;
                break;
        }

        return 1;
    }
};

class G: public Procedure {
public:
    G():Procedure() {}
    ~G() {}
    int execute(State & curr_state, const int & currComm, Action & action, int curr) const {
        if ( curr_state.c_g > currComm )
            return 0;

        //Set current State in action
        action.currState = curr_state;
        action.curr = curr;//index of state in vector with states <=> states[curr] == curr_state

        //std::cout << "G::";
        //else only one the rest variant: currComm == ip  => implement string
        switch (curr_state.c_g) {
            case  0:
                //std::cout << "int x, y;" << std::endl;
                action.operation = std::string("int x, y");
                curr_state.c_g = 1;
                action.nextState = curr_state;
                break;

            case  1:
                //std::cout << "x = 1;" << std::endl;
                action.operation = std::string("x = 1;");
                curr_state.g_x = 1;
                curr_state.init[3] = 1;
                curr_state.c_g = 2;
                action.nextState = curr_state;
                break;

            case  2:
                //std::cout << "y = 10" << std::endl;
                action.operation = std::string("y = 10;");
                curr_state.g_y = 10;
                curr_state.init[4] = 1;
                curr_state.c_g = 3;
                action.nextState = curr_state;
                break;

            case  3:
                //std::cout << "h = 1" << std::endl;
                action.operation = std::string("h = 1;");
                curr_state.h = 1;
                curr_state.init[0] = 1;
                curr_state.c_g = 4;
                action.nextState = curr_state;
                break;

            case  4:
                //if ( x < 10 ) in all cases TRUE can write only curr_state.c_g = 5
                //std::cout << "if ( x < 10 )" << std::endl;
                //( curr_state.g_x < 10 ) ? curr_state.c_g = 5 : curr_state.c_g = 7;

                if ( curr_state.g_x < 10 ) {
                    action.operation = std::string("( x < 10 )");
                    curr_state.c_g = 5;
                    action.nextState = curr_state;
                } else {
                    action.operation = std::string("!( x < 10 )");
                    curr_state.c_g = 7;
                    action.nextState = curr_state;
                }

                break;

            case  5:
                //if ( h > y ) in all cases FALSE can write only curr_state.c_g = 6
                //std::cout << "if ( h > y )" << std::endl;
                action.operation = std::string("!( h > y )");
                curr_state.c_g = 6;
                action.nextState = curr_state;
                break;

            case  6:
                //std::cout << "x = 0" << std::endl;
                action.operation = std::string("x = 0;");
                curr_state.g_x = 0;
                curr_state.init[3] = 1;
                curr_state.c_g = 7;
                action.nextState = curr_state;
                break;

            case  7:
                //std::cout << "y = 7" << std::endl;
                action.operation = std::string("y = 7;");
                curr_state.g_y = 7;
                curr_state.init[4] = 1;
                curr_state.c_g = 8;
                action.nextState = curr_state;
                break;

            case  8:
                //std::cout << "x = 6" << std::endl;
                action.operation = std::string("x = 6;");
                curr_state.g_x = 6;
                curr_state.init[3] = 1;
                curr_state.c_g = 9;
                action.nextState = curr_state;
                break;

            case  9:
                //while ( x < 9 ) in all cases TRUE can write only curr_state.c_g = 10
                //std::cout << "while ( x < 9 )" << std::endl;
                //( curr_state.g_x < 9 ) ? curr_state.c_g = 10 : curr_state.c_g = 12;

                if ( curr_state.g_x < 9 ) {
                    action.operation = std::string("( x < 9 )");
                    curr_state.c_g = 10;
                    action.nextState = curr_state;
                } else {
                    action.operation = std::string("!( x < 9 )");
                    curr_state.c_g = 12;
                    action.nextState = curr_state;
                }

                break;

            case  10:
                //if ( h > 0 ) in all cases TRUE
                //std::cout << "if ( h > 0 )" << std::endl;
                action.operation = std::string("( h > 0 )");
                curr_state.c_g = 11;
                action.nextState = curr_state;
                break;

            case  11:
                //std::cout << "break" << std::endl;
                action.operation = std::string("break;");
                curr_state.c_g = 12;
                action.nextState = curr_state;
                break;

            case  12:
                //end of the procedure
                //Infinite loop in last state
                action.operation = std::string("END");
                action.nextState = curr_state;
                action.next = curr;
                break;

            default:
                std::cout <<  "Not found appropriate command" << std::endl;
                break;
        }

        return 1;
    }
};

void addStateAndAction(std::vector<State> & states, const State & state, std::vector<Action> & actions, Action & action) {
    uint size;
    int index = 0;
    int flag = 0;

    //CHECK STATE
    size = states.size();
    for ( uint i = 0; i < size; i++ ) {
        if ( states[i] == state ) {
            index = i;
            flag = 1;
            break;
        }
    }

    if ( !flag ) {
        states.push_back(state);
        index = size;
    }
    //

    //If lts flag is not exist then not work with action
    if ( !lts )
        return;
    //


    action.next = index;

    //CHECK ACTION
    size = actions.size();
    for ( uint i = 0; i < size; i++ ) {
        if ( actions[i] == action ) {
            flag = 1;
            break;
        }
    }

    if ( !flag )
        actions.push_back(action);
    //

}

int stateIndex( std::vector<State> & states, State & state ) {
    int result = 0;
    for ( uint i = 0; i < states.size(); i++) {
        if ( states[i] == state )
            result = i;
            break;
    }
    return result;
}

void implementTrace(const uint & trace, std::vector<State> & states, std::vector<Action> & actions) {
    //Passing nulles and ones while implement trace
    //max one_count  == gLines
    //max null_count  == fLines
    int one_count = 0;
    int null_count = 0;
    //
    //create new procedures F and G
    F f;
    G g;
    //
    //Init state. All variables have unknown values. Only c_f == 0 and c_g == 0
    State state;
    Action action;
    states.push_back(state);//add first initial state
    //
    while ( (one_count + null_count) < int(fLines + gLines) ) {
        //std::cout << one_count << " " << null_count << " " << int(fLines + gLines) << std::endl;

        //take bit with number == ( one_count + null_count)
        //if bit == 0 then f.execute
        //if bit == 1 then g.execute
        //print new state in execute method if string was implemented
        //make shift on (one_count + null_count) and take first bit

        int currStateIndex = stateIndex(states, state);

        if ( (trace >> (one_count + null_count)) & 1 ){
            if ( g.execute(state, one_count, action, currStateIndex) ) // {5}
                addStateAndAction(states, state, actions, action);
            one_count += 1;
        } else {
            if ( f.execute(state, null_count, action, currStateIndex) ) // {6}
                addStateAndAction(states, state, actions, action);
            null_count += 1;
        }
    }
}

void printTrace( const uint & trace) {
    int k = fLines + gLines;
    std::string result = std::string("");
    for ( int i = k - 1; i >=0 ; --i ) {
        if ( ( trace >> i ) & 1 )
            std::cout << '1';
        else
            std::cout << '0';
    }
    std::cout << std::endl;
}

int printStates(std::vector<State> & states) {
    std::cout <<  "Feasible states -> " << states.size() << std::endl;
    if ( count ) {
        std::cout << "All feasible states -> 25272 * 2^128" << std::endl;
        std::cout << "All potential states -> 247 * 2^288" << std::endl;
    }

    //Write states in file <filename>
    std::ofstream fout;
    fout.open(filename, std::ios_base::out |  std::ios_base::trunc);
    if ( !fout.is_open() ) {
        std::cout << "Error in opening file" << std::endl;
        return 0;
    }

    fout << "c_f" << "  " << "c_g" << "  " << "h" << "  " << "f.x" << "  " << "f.y" << "  " << "g.x" << "  " << "g.y" << std::endl;
    for ( uint i = 0; i < states.size(); ++i )
        states[i].print(fout);

    fout.close();

    return 1;
}

int printLTS(std::vector<State> & states, std::vector<Action> & actions ) {

    std::ofstream fout;
    fout.open(filenameLTS, std::ios_base::out |  std::ios_base::trunc);
    if ( !fout.is_open() ) {
        std::cout << "Error in opening file for LTS" << std::endl;
        return 0;
    }

    fout << "digraph LTS {" << std::endl;

    for ( uint i = 0; i < states.size(); i++ ) {
        fout << i << " ";
        states[i].printDOT(fout);
    }

    for ( uint i = 0; i < actions.size(); i++ ) {
        actions[i].printDOT(fout);
    }

    fout << "}" << std::endl;

    fout.close();

    return 1;
}


int strToInt( const char * number, int & result ) {
    char* end_ptr;
    long val = strtol(number, &end_ptr, 10);
    if (*end_ptr) {
        std::cout << "Error in the number -> " << std::string(number) << std::endl;
        return 0;
    }

    if ( ((val == LONG_MAX || val == LONG_MIN ) && errno == ERANGE) || ( val > INT_MAX || val < INT_MIN ) ) {
        std::cout << "OverFlow in number ->" << number << std::endl;
        return 0;
    }

    result = static_cast<int>(val);
    return 1;
}

bool checkFlag(const char * flag, char ** argv, int argc, int & position) {
    bool exist = false;


    for (int i = 0; i < argc; i++) {
        if ( strcmp(argv[i], flag) != 0 )
            continue;

        //flag exist => check three variants
        if ( strcmp("-count\0", flag) == 0 ) { //check one-stand flag
            exist = true;
            std::cout << "-count exist" << std::endl;
            break;
        } else if ( ( strcmp("-file\0", flag) == 0 ) && ( i < ( argc - 1 ) ) ) { //check -file flag after this flag must stand <filename>
            position = i;
            exist = true;
            std::cout << "-file exist" << std::endl;
            break;
        } else if ( ( strcmp("-lts\0", flag) == 0 ) && ( i < ( argc - 1 ) ) ) { //check -lts flag after this flag must stand <filename>
            position = i;
            exist = true;
            std::cout << "-lts exist" << std::endl;
            break;
        }

    }

    return exist;
}

int parserInputParameters( int argc, char ** argv ) {
    int a,b,c,d;
    if (argc < 5 || argc > 10)
        return 0;
    //argc = {6,7,8,9,10}
    //Check input parameters for procedure f and g
    if ( !strToInt(argv[1], a ) )
        return 0;
    if ( !strToInt(argv[2], b ) )
        return 0;
    if ( !strToInt(argv[3], c ) )
        return 0;
    if ( !strToInt(argv[4], d ) )
        return 0;
    //Set global variables
    f_a = a;
    f_b = b;
    g_a = c;
    g_b = d;

    //New parser
    int position;
    if ( checkFlag("-count\0", argv, argc, position) )
        count = 1;

    if ( checkFlag("-file\0", argv, argc, position) ) {
        file = 2;
        filename = std::string(argv[position+1]);
    }

    if ( checkFlag("-lts\0", argv, argc, position) ) {
        lts = 2;
        filenameLTS = std::string(argv[position+1]);
    }

    if ( argc != ( count + file + lts + 5 ) ) {
        std::cout << "error in number argument" << std::endl;
        return 0;
    }

    //
    return 1;
}

int main( int argc, char **argv ) {
    /*имя_программы <f_a> <f_b> <g_a> <g_b>
     * [-file имя_файла] - запись состояний в указанный файл,
     * [-count] - вывод общего количества состояний программы в стандартный поток вывода.
     * При запуске без параметров (либо с недостаточным количеством параметров)
     * программа должна выводить информацию о программе, авторе, годе написания и параметрах запуска.
     * Формат вывода в файл c_f, c_g, h, f.x, f.y, g.x, g.y.
     */
    if (!parserInputParameters(argc, argv)) {
        std::cerr << "Error in the input parameters" << std::endl;
        std::cout << "This program computes the set of reachable states "
            "for the program from the file task.txt" << std::endl <<
            "Author: Chupakhin Andrey" << std::endl <<
            "Year: September, 2015" << std::endl <<
            "usage: prog_name <f_a> <f_b> <g_a> <g_b> [-file <filename>] [-count]" << std::endl;
        return 1;
    }

    initPathes();
    std::vector<State> states;
    std::vector<Action> actions;
    //
    for ( uint trace = START_PATH; trace <= /*END_PATH_NEW*/END_PATH; trace++ ) {
        if ( !correctTrace(trace) )
            continue;
        //printTrace( trace );
        implementTrace(trace, states, actions); // {2}
    }

    if ( !printStates(states) )
        return 1;

    if ( !lts )
        return 0;

    if ( !printLTS(states, actions) )
        return 1;

    //
    return 0;
}
