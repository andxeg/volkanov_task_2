//Author: Chupakhin Andrey
//Year: September, 2015
/* Данная программа вычисляет и записывает в текстовой файл states.txt множество достижимых состояний
 * заданной программы для заданных значений параметров функций, а также выводит в стандартный
 * поток вывода количество достижимых состояний.
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
int f_a, f_b, g_a, g_b;
int count = 0;
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

        //Print without convertion
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
};

class Procedure {
public:
    Procedure() {}
    ~Procedure() {}
    virtual int execute(State & curr_state, const int & currComm) const = 0;
};

class F: public Procedure {
public:
    F():Procedure() {}
    ~F() {}
    int execute(State & curr_state, const int & currComm) const {
        //execute operation in ip -> change State and change ip
        // curr_state -> execute command -> new_state and new value in ip
        // if currComm < ip therefore not implement string
        if ( curr_state.c_f > currComm )
            return 0;

        //std::cout << "F::";
        //else only one the rest variant: currComm == ip  => implement string
        switch (curr_state.c_f) {
            case  0:
                //std::cout << "int x, y" << std::endl;
                curr_state.c_f = 1;
                break;

            case  1:
                //std::cout << "x = 7" << std::endl;
                curr_state.f_x = 7;
                curr_state.init[1] = 1;
                curr_state.c_f = 2;
                break;

            case  2:
                //std::cout << "y = 7" << std::endl;
                curr_state.f_y = 7;
                curr_state.init[2] = 1;
                curr_state.c_f = 3;
                break;

            case  3:
                //std::cout << "h = 4" << std::endl;
                curr_state.h = 4;
                curr_state.init[0] = 1;
                curr_state.c_f = 4;
                break;

            case  4:
                //std::cout << "if ( h < b )" << std::endl;
                ( curr_state.h < f_b) ? curr_state.c_f = 5 : curr_state.c_f = 7;
                break;

            case  5:
                //if ( h > 6 )  in all cases TRUE
                //std::cout << "if ( h > 6 )" << std::endl;
                curr_state.c_f = 6;
                break;

            case  6:
                //std::cout << "x = 2" << std::endl;
                curr_state.f_x = 2;
                curr_state.init[1] = 1;
                curr_state.c_f = 7;
                break;

            case  7:
                //std::cout << "if ( h > y )" << std::endl;
                curr_state.c_f = 8;
                break;

            case  8:
                //end of the procedure
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
    int execute(State & curr_state, const int & currComm) const {
        if ( curr_state.c_g > currComm )
            return 0;

        //std::cout << "G::";
        //else only one the rest variant: currComm == ip  => implement string
        switch (curr_state.c_g) {
            case  0:
                //std::cout << "int x, y;" << std::endl;
                curr_state.c_g = 1;
                break;

            case  1:
                //std::cout << "x = 1;" << std::endl;
                curr_state.g_x = 1;
                curr_state.init[3] = 1;
                curr_state.c_g = 2;
                break;

            case  2:
                //std::cout << "y = 10" << std::endl;
                curr_state.g_y = 10;
                curr_state.init[4] = 1;
                curr_state.c_g = 3;
                break;

            case  3:
                //std::cout << "h = 1" << std::endl;
                curr_state.h = 1;
                curr_state.init[0] = 1;
                curr_state.c_g = 4;
                break;

            case  4:
                //if ( x < 10 ) in all cases TRUE can write only curr_state.c_g = 5
                //std::cout << "if ( x < 10 )" << std::endl;
                ( curr_state.g_x < 10 ) ? curr_state.c_g = 5 : curr_state.c_g = 7;
                break;

            case  5:
                //if ( h > y ) in all cases FALSE can write only curr_state.c_g = 6
                //std::cout << "if ( h > y )" << std::endl;
                curr_state.c_g = 6;
                break;

            case  6:
                //std::cout << "x = 0" << std::endl;
                curr_state.g_x = 0;
                curr_state.init[3] = 1;
                curr_state.c_g = 7;
                break;

            case  7:
                //std::cout << "y = 7" << std::endl;
                curr_state.g_y = 7;
                curr_state.init[4] = 1;
                curr_state.c_g = 8;
                break;

            case  8:
                //std::cout << "x = 6" << std::endl;
                curr_state.g_x = 6;
                curr_state.init[3] = 1;
                curr_state.c_g = 9;
                break;

            case  9:
                //while ( x < 9 ) in all cases TRUE can write only curr_state.c_g = 10
                //std::cout << "while ( x < 9 )" << std::endl;
                ( curr_state.g_x < 9 ) ? curr_state.c_g = 10 : curr_state.c_g = 12;
                break;

            case  10:
                //if ( h > 0 ) in all cases TRUE
                //std::cout << "if ( h > 0 )" << std::endl;
                curr_state.c_g = 11;
                break;

            case  11:
                //std::cout << "break" << std::endl;
                curr_state.c_g = 12;
                break;

            case  12:
                //end of the procedure
                break;

            default:
                std::cout <<  "Not found appropriate command" << std::endl;
                break;
        }

        return 1;
    }
};

void addState(std::vector<State> & states, const State & state) {
    uint size = states.size();
    int flag = 0;
    for ( uint i = 0; i < size; i++ ) {
        if ( states[i] == state ) {
            flag = 1;
            break;
        }
    }

    if ( !flag )
        states.push_back(state);
}

void implementTrace(const uint & trace, std::vector<State> & states) {
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
    addState(states, state);
    //
    while ( (one_count + null_count) < int(fLines + gLines) ) {
        //std::cout << one_count << " " << null_count << " " << int(fLines + gLines) << std::endl;

        //take bit with number == ( one_count + null_count)
        //if bit == 0 then f.execute
        //if bit == 1 then g.execute
        //print new state in execute method if string was implemented
        //make shift on (one_count + null_count) and take first bit
        if ( (trace >> (one_count + null_count)) & 1 ){
            if ( g.execute(state, one_count) ) // {5}
                addState(states, state);
            one_count += 1;
        } else {
            if ( f.execute(state, null_count) ) // {6}
                addState(states, state);
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

int parserInputParameters( int argc, char ** argv ) {
    int a,b,c,d;
    if (argc < 5 || argc > 8)
        return 0;
    //argc = {5,6,7,8}
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
    //

    //Check other input parameters
    // Four variants:
    // [-file <filename>] second [-count]
    // second [-count] [-file <filename>]
    // [-file <filename>]
    // [-count]
    if ( argc == 6 ) {
        if ( strcmp(argv[5],"-count\0") == 0 ) {
            count = 1;
            std::cout << "-count parameter successful read" << std::endl;
            return 1;
        } else {
            std::cout << "Error in the 5-th parameter" << std::endl;
            return 0;
        }
    }

    if ( argc == 7 ) {
        if ( ( strcmp(argv[5],"-file\0") == 0 ) && ( strcmp(argv[6],"-count\0") != 0) ) {
            filename = std::string(argv[6]);
            std::cout << "-filename parameter successful read" << std::endl;
            return 1;
        } else {
            std::cout << "Error in the 5-th and 6-th parameters" << std::endl;
            return 0;
        }
    }

    if ( argc == 8 ) {
        if ( ( strcmp(argv[5],"-file\0") == 0 ) && ( strcmp(argv[6],"-count\0") != 0 ) && ( strcmp(argv[7],"-count\0") == 0 ) ) {
            std::cout << "-filename parameter successful read" << std::endl;
            std::cout << "-count parameter successful read" << std::endl;
            filename = std::string(argv[6]);
            count = 1;
            return 1;
        } else if ( ( strcmp(argv[5],"-count\0") == 0 ) && ( strcmp(argv[6],"-file\0") == 0 ) && ( strcmp(argv[7],"-count\0") != 0 ) ) {
            std::cout << "-filename parameter successful read" << std::endl;
            std::cout << "-count parameter successful read" << std::endl;
            filename = std::string(argv[7]);
            count = 1;
            return 1;
        } else {
            std::cout << "Error in the 5-th, 6-th, 7-th parameters" << std::endl;
            return 0;
        }
    }
    //
    //std::cout << "End of parserInputParameters" << std::endl;
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
    //
    for ( uint trace = START_PATH; trace <= /*END_PATH_NEW*/END_PATH; trace++ ) {
        if ( !correctTrace(trace) )
            continue;
        //printTrace( trace );
        implementTrace(trace, states); // {2}
    }

    if ( !printStates(states) )
        return 1;

    //
    return 0;
}
