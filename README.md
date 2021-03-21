# functions-calculator

A console `C++ application` that allows you to create `mathematical functions` and `variables` that you can create and evaluate.

## Variables

You can create your own `variable` with `=` operator.

**Example:**
```
: 4*sqrt(3)*(sin(pi/3)+1)
>> 12.9282032303

: a = Ans
>> a = 12.9282032303

```


### Built-in variables


| Name | Description                 |
|:---- | --------------------------- |
| pi   | π = 3.14159265359           |
| e    | e = 2.71828182846           |
| ---- | --------------------------- |
| Ans  | Previously calculated value |


## Functions

### Built-in functions
Available functions:
 - `sin(x)`, `cos(x)`, `tan(x)`
 - `sinh(x)`, `cosh(x)`, `tanh(x)`
 - `asin(x)`, `acos(x)`, `atan(x)`
 - `asinh(x)`, `acosh(x)`, `atanh(x)`
 - `exp(x)`
 - `log(x)`, `ln(x)`
 - `sqrt(x)`

**Available operators** are `+`, `-`, `*`, `/`, `^`, `()`, and `=`.


## Commands
| Name | Description |
|:---- |:----------- |
| mode: rad | calculation set to radian |
| mode: deg | calculation set to deg    |
| help      | help for this program     |
| exit      | exit from program         |
