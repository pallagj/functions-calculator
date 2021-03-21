#ifndef CALCULATOR_HPP_INCLUDED
#define CALCULATOR_HPP_INCLUDED

#include <map>
#include <math.h>
#include <vector>
#include <memory>
#include <cstring>

#include "function.hpp"


class Calculator{
    std::ostream& os;

    //Ebben van minden változó, és minden függvény!
    std::vector<std::map<std::string, std::shared_ptr<ReferenceFunction>>> data;
    static std::vector<std::map<std::string, std::shared_ptr<ReferenceFunction>>> base_data;

    double ans;

    void syntaxCheck(std::string);

	double calculatePlusMinus(std::string s);
	double calculateMultDiv(std::string s);
	double calculateFunction(std::string s);

	std::shared_ptr<Element> fcalculatePlusMinus(std::string s, std::vector<std::string>);
	std::shared_ptr<Element> fcalculateMultDiv(std::string s, std::vector<std::string>);
	std::shared_ptr<Element> fcalculateFunction(std::string s, std::vector<std::string>);


public:
    Calculator(std::ostream& os): os(os) {}

    //Megold egy képletet, a megoldás az ansban is eltárolódik
    double solve(std::string s);

    //Egy új konstanst vagy egy új függvényt vesz fel
    void addFunction(std::string command);

    //A beépített függvényekhez kötelező az inicializálás
    static void init();
};


#endif // CALCULATOR_HPP_INCLUDED
