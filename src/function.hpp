#ifndef FUNCTION_HPP_INCLUDED
#define FUNCTION_HPP_INCLUDED

#include <vector>
#include <memory>
#include <initializer_list>

class CalculatorError: public std::exception{
protected:
    std::string s;
public:
    explicit CalculatorError(const char * s): s(s) {}
    explicit CalculatorError(const std::string& s): s(s) {}
    CalculatorError(std::initializer_list<std::string> slist){
        for(std::string sa : slist)
            s += sa;
    }
    virtual ~CalculatorError() throw(){}
    virtual const char* what() const throw(){ return s.c_str(); }
};

class Element{
public:
	virtual double eval(std::vector<double> input) const = 0;
	virtual bool check(Element* f) const{ return this == f; }
    virtual ~Element(){}
};

class Variable: public Element{
	int var_i;
public:
	Variable(int var_i): var_i(var_i){}
	double eval(std::vector<double> input) const{
		return input[var_i];
	}
};


class Function: public Element{
public:
};

class Constans: public Function{
	double c;
public:
	Constans(double c): c(c){}
	double eval(std::vector<double> input) const{
		return c;
	}
};


template<double (*fv)(double)>
class BaseFunction1: public Function{
public:
	double eval(std::vector<double> input) const{
		return fv(input[0]);
	}
};

template<double (*fv)(double,double)>
class BaseFunction2: public Function{
public:
	double eval(std::vector<double> input) const{
		return fv(input[0], input[1]);
	}
};

class BaseFunctionSum: public Function{
    std::vector<bool> neg;  // False - pozitiv, true - negativ sign
public:
    BaseFunctionSum(){}
    BaseFunctionSum(std::vector<bool> neg): neg(neg){}

	double eval(std::vector<double> input) const{
		double sum = 0;

		for(unsigned int i = 0; i<input.size(); i++){
            if(i < neg.size() && neg[i])
                sum -= input[i];
            else
                sum += input[i];

		}

        return sum;
	}
};


class BaseFunctionSub: public Function{
public:
	double eval(std::vector<double> input) const{
		return input[0]-input[1];
	}
};

class BaseFunctionNeg: public Function{
public:
	double eval(std::vector<double> input) const{
		return -input[0];
	}
};

class BaseFunctionMult: public Function{
    std::vector<bool> dived;
public:
    BaseFunctionMult(){}
    BaseFunctionMult(std::vector<bool> dived): dived(dived){}

	double eval(std::vector<double> input) const{
		double mult = 1;
		for(unsigned int i = 0; i<input.size(); i++){
            if(i < dived.size() && dived[i])
                mult /= input[i];
            else
                mult *= input[i];

		}
        return mult;
	}
};

class BaseFunctionDiv: public Function{
public:
	double eval(std::vector<double> input) const{
		return input[0]/input[1];
	}
};


class ReferenceFunction: public Function{
protected:
	std::shared_ptr<Element> f;
public:
    ReferenceFunction(): f(nullptr){}
    ReferenceFunction(std::shared_ptr<Element> f): f(f){}
    std::shared_ptr<Element> getFunction() const{ return f; }
    void setFunction(std::shared_ptr<Element> f){
        if(f->check(this))
            throw CalculatorError("Circular references");
        this->f = f;
    }

    double eval(std::vector<double> input) const{
        return f->eval(input);
	}

	bool check(Element* f) const{
        return this == f || this->f->check(f);
	}
};

class ReferenceFunctionWithInputs: public ReferenceFunction{
	std::vector<std::shared_ptr<Element> > in;
public:
	ReferenceFunctionWithInputs(std::vector<std::shared_ptr<Element> > in, std::shared_ptr<Element> f)
	: ReferenceFunction(f), in(in){}

	double eval(std::vector<double> input) const{
		std::vector<double> in_value;

		for(std::shared_ptr<Element> e : in)
			in_value.push_back(e->eval(input));

		return f->eval(in_value);
	}

	bool check(Element* f) const{
	    if(ReferenceFunction::check(f)) return true;
        for(std::shared_ptr<Element> e: in){
            if(e->check(f)) return true;
        }
        return false;
	}
};

#endif // FUNCTION_HPP_INCLUDED
