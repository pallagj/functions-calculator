#include <sstream>
#include <iostream>


#include "function.hpp"
#include "calculator.hpp"

void consol(){
    Calculator calc(std::cout);
    std::string s;
    std::cout << "Welcome at EasyMath!" << std::endl;

    std::cout << ": ";

    while(std::getline(std::cin, s) && s != "exit"){
        if(s == "clear"){ std::system("cls"); std::cout<<": "; continue; }
        try{
            if(s.find("=") == std::string::npos)
                std::cout << " >> " << calc.solve(s) << std::endl;
            else
                calc.addFunction(s);


        }catch(CalculatorError err){
            std::cout << "  ERROR: " << err.what() << std::endl;
        }
        std::cout<<std::endl;
        std::cout << ": ";
    }
}

void test(){
    std::istringstream stream(
        //Test
     //   "sin(0)\n"
     //   "ln(e)\n"
     //   "1+(3*5-lg(100))\n"
     //   "sqrt(pow(3,2)+pow(4,2))\n"
     //   "2*pi-1/20\n"
        "3*2+5\n"
        "ans+1\n"
        //Test variables
        "a=2.71828182846\n"
        "b=2*a\n"
        //Test functions
        "f(x,y)=x*x-y\n"
        "f(x,y,z)=x*x-y+f(z-x,x)\n"
        "f(1,2,3)\n"
        "exit"
    );

    double test[8] = {11, 12, 2.0};
    std::ostringstream os;
    Calculator calc(os);

    std::string s;

    int i = 0;
    while(stream >> s && s != "exit"){
        try{
            if(s.find("=") == std::string::npos){
                if(abs(round(calc.solve(s)) - test[i++]) > 0.001)
                    throw "Helytelen mukodes";
            }else{
                calc.addFunction(s);
            }
        }catch(const std::exception &s){
            std::cerr<<s.what()<<std::endl;
        }catch (const char* s){
            std::cerr<<s <<std::endl;
        }
    }
}

int main(){

    //A beépített függvények használatához szükséges az inicializálás
    Calculator::init();

    //test();
    consol();

    return 0;
}
