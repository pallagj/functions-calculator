//#include <initializer_list>
//#include <exception>
#include <algorithm>
#include <iostream>
//#include <cstring>
#include <math.h>
//#include <cctype>
#include <string>
#include <vector>
#include <memory>
#include <map>

#include "calculator.hpp"
#include "function.hpp"

std::vector<std::map<std::string, std::shared_ptr<ReferenceFunction>>> Calculator::base_data;


void Calculator::syntaxCheck(const std::string s){
    int deep= 0;

    for(char c : s){
        if(c == '(') deep++; if(c == ')') deep--;
        if(deep < 0)
        	throw CalculatorError("Incorrect parenthesis");
     }

    if(deep != 0) throw CalculatorError("Incorrect parenthesis");
}

double Calculator::calculatePlusMinus(std::string s){
    double sum = 0;
    int deep  = 0, index = 0,
        start = 0, sign  = 1;

    for(char c : s){
        if(c == '(') deep++; else if(c == ')') deep--;

        if((c == '+' || c == '-' ) && deep== 0){
            if(start == index && index != 0)
            	throw CalculatorError{"Bad syntax: ", std::string()+c, " operator"};

            if(index != 0)
            	sum += sign*calculateMultDiv(s.substr(start, index-start));

            sign = (c == '+') ? +1 : -1;
            start= index+1;
        }

        index++;
    }

    if(start == index)
        throw CalculatorError{"Bad syntax"};

    sum += sign*calculateMultDiv(s.substr(start, index-start));

    return sum;
}

double Calculator::calculateMultDiv(std::string s){
    double mult = 1;
    int deep  = 0, index = 0,
        start = 0;
    bool is_mult = true;

    for(char c : s){
        if(c == '(') deep++; else if(c == ')') deep--;

        if((c == '*' || c == '/' ) && deep == 0){
            if(start == index) throw CalculatorError{"Bad operator aritmethic (", std::string()+c, " operator)"};

            if(is_mult)
                mult *= calculateFunction(s.substr(start, index-start));
            else
                mult /= calculateFunction(s.substr(start, index-start));

            is_mult = (c == '*');
            start= index+1;
        }
        index++;
    }

    if(start == index) throw CalculatorError{"Bad operator aritmethic)"};

    if(is_mult)
        mult *= calculateFunction(s.substr(start, index-start));
    else
        mult /= calculateFunction(s.substr(start, index-start));

    return mult;
}

double Calculator::calculateFunction(std::string s){
    if(s[0] == '('){
        if(s[s.size()-1] == ')')
            return calculatePlusMinus(s.substr(1, s.size()-2));
        else
            throw CalculatorError("Operator shortage");
    }

    std::size_t i = s.find("(");
    if(i != std::string::npos && s[s.size()-1] != ')')
        throw CalculatorError("Incorrect function call");

    std::vector<double> inputs = std::vector<double>(0);
    if(i != std::string::npos){
        //ElÅ‘ kell Ã¡llÃ­tani az inputs vektort!
        std::string in = s.substr(i+1, s.size()-i-2);

        int deep= 0;
        std::string stemp = "";
        for(char c : in){
            if(c == '(') deep++; else if(c == ')') deep--;

            if(deep == 0){
                if(c == ','){
                    inputs.push_back(calculatePlusMinus(stemp));
                    stemp = "";
                }else{
                    stemp += c;
                }
            }else stemp += c;
        }

        //Az utolsÃ³t is bele kell tenni
        inputs.push_back(calculatePlusMinus(stemp));

        //ZÃ¡rÃ³jeleket eltÃ¡volÃ­tani
        s = s.substr(0, i);
    }


    if(base_data.size() > inputs.size() && base_data[inputs.size()].count(s))
        return base_data[inputs.size()].at(s)->eval(inputs);

    if(data.size() > inputs.size() && data[inputs.size()].count(s))
        return data[inputs.size()].at(s)->eval(inputs);

    if(s == "ans")
        return ans;

    try{
        return std::stod(s);
    }catch(std::exception ex){
        if(inputs.size() == 0)
            throw CalculatorError{"Unknown variable (",s,")"};
        else
            throw CalculatorError{"Unknown function (",s,", input size = ",std::to_string(inputs.size()),")"};
    }

}

//Megold egy szÃ¡molÃ¡st (fÃ¼ggvÃ©nyhÃ­vÃ¡ssal egyÃ¼tt)
double Calculator::solve(std::string s){
	s.erase(std::remove_if(s.begin(), s.end(), [](unsigned char x){return std::isspace(x);}), s.end());
    syntaxCheck(s);
    ans = calculatePlusMinus(s);
    return ans;
}



std::vector<std::string> split(std::string s, char c){
    std::vector<std::string> vec;
    std::string temp = "";
    for(char c_s : s){
        if(c != c_s)
            temp += c_s;
        else{
            vec.push_back(temp);
            temp = "";
        }
    }
    vec.push_back(temp);
    return vec;
}

std::shared_ptr<Element> Calculator::fcalculatePlusMinus(std::string s, std::vector<std::string> input_variables){
    std::vector< std::shared_ptr<Element> > inputs;
    std::vector<bool> signs;

    int deep  = 0, index = 0,
        start = 0, sign  = 1;

    bool used = false;

    for(char c : s){
        if(c == '(') deep++; else if(c == ')') deep--;

        if((c == '+' || c == '-' ) && deep== 0){
            used = true;

            if(start == index && index != 0)
            	throw CalculatorError{"Bad syntax: ", std::string()+c, " operator"};

            if(index != 0){
                inputs.push_back(fcalculateMultDiv(s.substr(start, index-start), input_variables));
                signs.push_back(sign == -1);
            }

            sign = (c == '+') ? +1 : -1;
            start= index+1;
        }

        index++;
    }

    if(start == index)
    	throw CalculatorError{"Bad syntax: operator"};

    if(!used)
        return fcalculateMultDiv(s.substr(start, index-start), input_variables);

    if(index != 0){
        inputs.push_back(fcalculateMultDiv(s.substr(start, index-start), input_variables));
        signs.push_back(sign == -1);
    }

    return std::make_shared<ReferenceFunctionWithInputs>(inputs, std::make_shared<BaseFunctionSum>(signs));
}

std::shared_ptr<Element> Calculator::fcalculateMultDiv(std::string s, std::vector<std::string> input_variables){
    int deep  = 0, index = 0,
        start = 0;
    bool is_mult = true;
    bool used = false;

    std::vector< std::shared_ptr<Element> > inputs;
    std::vector<bool> dived;
    for(char c : s){
        if(c == '(') deep++; else if(c == ')') deep--;

        if((c == '*' || c == '/' ) && deep == 0){
            if(start == index) throw CalculatorError{"Bad syntax: ", std::string()+c, " operator"};

            dived.push_back(!is_mult);
            inputs.push_back(fcalculateFunction(s.substr(start, index-start), input_variables));


            is_mult = (c == '*');
            start= index+1;

            used = true;
        }
        index++;
    }

    if(start == index) throw CalculatorError{"Bad syntax: operator"};

    if(!used)
        return fcalculateFunction(s, input_variables);

    dived.push_back(!is_mult);
    inputs.push_back(fcalculateFunction(s.substr(start, index-start), input_variables));

    return std::make_shared<ReferenceFunctionWithInputs>(inputs,std::make_shared<BaseFunctionMult>(dived));
}

std::shared_ptr<Element> Calculator::fcalculateFunction(std::string s, std::vector<std::string> input_variables){
    if(s[0] == '('){
        if(s[s.size()-1] == ')')
            return fcalculatePlusMinus(s.substr(1, s.size()-2), input_variables);
        else
            throw CalculatorError("Operator shortage");
    }

    std::size_t i = s.find("(");
    if(i != std::string::npos && s[s.size()-1] != ')')
        throw CalculatorError("Incorrect function call");

    std::vector< std::shared_ptr<Element> > inputs;
    if(i != std::string::npos){
        //ElÅ‘ kell Ã¡llÃ­tani az inputs vektort!
        std::string in = s.substr(i+1, s.size()-i-2);

        int deep= 0;
        std::string stemp = "";
        for(char c : in){
            if(c == '(') deep++; else if(c == ')') deep--;

            if(deep == 0){
                if(c == ','){
                    inputs.push_back(fcalculatePlusMinus(stemp, input_variables));
                    stemp = "";
                }else{
                    stemp += c;
                }
            }else stemp += c;
        }

        //Az utolsÃ³t is bele kell tenni
        inputs.push_back(fcalculatePlusMinus(stemp, input_variables));

        //ZÃ¡rÃ³jeleket eltÃ¡volÃ­tani
        s = s.substr(0, i);
    }


    if(base_data.size() > inputs.size() && base_data[inputs.size()].count(s))
        return std::make_shared<ReferenceFunctionWithInputs>(inputs, base_data[inputs.size()][s]);

    if(data.size() > inputs.size() && data[inputs.size()].count(s))
        return std::make_shared<ReferenceFunctionWithInputs>(inputs, data[inputs.size()][s]);


    for(unsigned int i = 0; i < input_variables.size(); i++)
        if(input_variables[i] == s)
            return std::make_shared<Variable>(i);
    try{
        return std::make_shared<Constans>(std::stod(s));
    }catch(std::exception ex){
        if(inputs.size() == 0)
            throw CalculatorError{"Unknown variable (",s,")"};
        else
            throw CalculatorError{"Unknown function (",s,", input size = ",std::to_string(inputs.size()),")"};
    }

}
void Calculator::addFunction(std::string s){
	//Szóközök eltávolítása
	s.erase(std::remove_if(s.begin(), s.end(), [](unsigned char x){return std::isspace(x);}), s.end());
	syntaxCheck(s);

	//Függvény nevének meghatározása

    std::size_t start = s.find("(");
    std::size_t end = s.find(")");
    std::size_t eq = s.find("=");

    //Nincs egyenlosegjel
    if(eq == std::string::npos)
        throw CalculatorError("Incorrect function call");

    std::vector<std::string> inputs;

    std::string name;
    std::string type= "function";
    if(start != std::string::npos && start<eq){
        inputs = split(s.substr(start+1, end-start-1), ',');
        name = s.substr(0, start);
    }else{
        name = s.substr(0, eq);
        type = "variable";
    }

    if(base_data.size() > inputs.size())
        if(base_data[inputs.size()].count(name))
            throw CalculatorError{"You can't add built-in ", type};


    if(data.size() > inputs.size()){
        if(data[inputs.size()].count(name)){
            data[inputs.size()][name]->setFunction(
                std::make_shared<ReferenceFunction>(fcalculatePlusMinus(s.substr(eq+1, s.size()-eq-1), inputs))
            );
            os << "The " << type << " '" << name << "' has been refreshed!" <<std::endl;
        }else{
            data[inputs.size()].insert(
                std::make_pair (
                    name,
                    std::make_shared<ReferenceFunction>(fcalculatePlusMinus(s.substr(eq+1, s.size()-eq-1), inputs))
                )
            );
            os << "The " << type << " '" << name << "' has been added!" <<std::endl;
        }
    }else{
        data.resize(inputs.size()+1);
        data[inputs.size()].insert(
            std::make_pair(
                name,
                std::make_shared<ReferenceFunction>(fcalculatePlusMinus(s.substr(eq+1, s.size()-eq-1), inputs))
            )
        );
        os << "The " << type << " '" << name << "' has been added!" <<std::endl;
    }


}

//ALAPÉRTELMEZETT FÜGGVÉNYEK INICIALIZÁLÁSA
void Calculator::init(){

    Calculator::base_data     =   {
            //KONSTANSOK
            {
                std::make_pair("pi", std::make_shared<ReferenceFunction>(std::make_shared<ReferenceFunction>((std::make_shared<Constans>((3.14159265359)))))),
                std::make_pair("e",  std::make_shared<ReferenceFunction>(std::make_shared<ReferenceFunction>((std::make_shared<Constans>((2.71828182846)))))),
            },

            //1 VALTOZOS FUGGVENYEK
            {

#define AddReferenceFunction(name) std::make_pair(#name, std::make_shared<ReferenceFunction>(std::make_shared<BaseFunction1<name>>()))

                AddReferenceFunction(sin),
                AddReferenceFunction(cos),
                AddReferenceFunction(tan),


                std::make_pair("asin",   std::make_shared<ReferenceFunction>(std::make_shared<BaseFunction1<asin>>())),
                std::make_pair("arcsin", std::make_shared<ReferenceFunction>(std::make_shared<BaseFunction1<asin>>())),
                std::make_pair("acos",   std::make_shared<ReferenceFunction>(std::make_shared<BaseFunction1<acos>>())),
                std::make_pair("arccos", std::make_shared<ReferenceFunction>(std::make_shared<BaseFunction1<acos>>())),
                std::make_pair("atan",   std::make_shared<ReferenceFunction>(std::make_shared<BaseFunction1<atan>>())),
                std::make_pair("arctan", std::make_shared<ReferenceFunction>(std::make_shared<BaseFunction1<atan>>())),

                std::make_pair("sinh", std::make_shared<ReferenceFunction>(std::make_shared<BaseFunction1<sinh>>())),
                std::make_pair("sh",   std::make_shared<ReferenceFunction>(std::make_shared<BaseFunction1<sinh>>())),
                std::make_pair("cosh", std::make_shared<ReferenceFunction>(std::make_shared<BaseFunction1<cosh>>())),
                std::make_pair("ch",   std::make_shared<ReferenceFunction>(std::make_shared<BaseFunction1<cosh>>())),
                std::make_pair("tanh", std::make_shared<ReferenceFunction>(std::make_shared<BaseFunction1<tanh>>())),
                std::make_pair("th",   std::make_shared<ReferenceFunction>(std::make_shared<BaseFunction1<tanh>>())),

                std::make_pair("arcsinh", std::make_shared<ReferenceFunction>(std::make_shared<BaseFunction1<asinh>>())),
                std::make_pair("asinh",   std::make_shared<ReferenceFunction>(std::make_shared<BaseFunction1<asinh>>())),
                std::make_pair("arsh",    std::make_shared<ReferenceFunction>(std::make_shared<BaseFunction1<asinh>>())),

                std::make_pair("arccosh", std::make_shared<ReferenceFunction>(std::make_shared<BaseFunction1<acosh>>())),
                std::make_pair("acosh",   std::make_shared<ReferenceFunction>(std::make_shared<BaseFunction1<acosh>>())),
                std::make_pair("arch",    std::make_shared<ReferenceFunction>(std::make_shared<BaseFunction1<acosh>>())),

                std::make_pair("arctanh", std::make_shared<ReferenceFunction>(std::make_shared<BaseFunction1<atanh>>())),
                std::make_pair("atanh",   std::make_shared<ReferenceFunction>(std::make_shared<BaseFunction1<atanh>>())),
                std::make_pair("arth",    std::make_shared<ReferenceFunction>(std::make_shared<BaseFunction1<atanh>>())),

                std::make_pair("sqrt", std::make_shared<ReferenceFunction>(std::make_shared<BaseFunction1<sqrt>>())),

                std::make_pair("abs",  std::make_shared<ReferenceFunction>(std::make_shared<BaseFunction1<fabs>>())),

                std::make_pair("log", std::make_shared<ReferenceFunction>(std::make_shared<BaseFunction1<log>>())),
                std::make_pair("ln",  std::make_shared<ReferenceFunction>(std::make_shared<BaseFunction1<log>>())),
                std::make_pair("lg",  std::make_shared<ReferenceFunction>(std::make_shared<BaseFunction1<log10>>())),


                std::make_pair("exp", std::make_shared<ReferenceFunction>(std::make_shared<BaseFunction1<exp>>()))
            },

            //2 VALTOZOS FUGGVENYEK
            {
                std::make_pair("pow", std::make_shared<ReferenceFunction>(std::make_shared<BaseFunction2<pow>>()))
            }
        };

}

