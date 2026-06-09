# JSON Parser for C++

[![pl](https://img.shields.io/badge/lang-pl-red.svg)](https://github.com/Chackapon/json_parser_cpp/blob/main/lang/README.pl.md)
[![en](https://img.shields.io/badge/lang-en-blue.svg)](https://github.com/Chackapon/json_parser_cpp/blob/main/lang/README.en.md)

A C++ shared library that contains a class that allows you to parse a JSON file or string into something, that functions like Python's json library.

## Usage
### Parsing
Once you create a JSON_Tree object there are two ways to input data in it: 
- from a string by using importString() function
- from a json file using importFile() function (and optionaly setDirectory)

Regardless of the function used JSON_Tree then starts parsing the json code by first stripping it of white spaces and then building an internal tree consisting of JSON_Object objects.

JSON_Object is a struct that contains two fields: "type" which is and enum o possible json value types and "value" which is an alias for an std::variant type with all primitive types supported by JSON (including a custom struct JSON_Null meant to represent "null") as well as a pointer to JSON_Object type. This way JSON_Object can store every possible json value that can be accessed with a string key.

### Data extraction

JSON_Object has two function for data extraction:

- Overloaded operator[] that returns a JSON_Node object. This allows for nested calls, for example ``` parser["users"]["John"] ```
- value<T>() function which servers as the [] operator for the last step, example ``` parser["users"]["John"]["name"] ``` for a string type record. The reason it has to be this way is operator[] can't be overrloaded twice with just changing the return type. This function returns the std::variant's exact value, so besides from specifying the T type you don't have to do anything else.

## Version history
* [09.06.26, 03:09] v0.1: first public release. Still a lot to be done but the basic parser object is complete
* [10.06.26, 01:11] v0.2: added basic support for extraction of data from parsed json and changed how data is stored in nodes from a pair to a struct

Features
- [x] Support for values of primitive types: int, float, bool, string, null
- [x] Support for dictionaries
- [ ] Support for arrays
- [x] operator[] overload
- [ ] Actual release on github
- [ ] Some metadata for the library
- [ ] Automatic Python tester that generates random json file and plots performance vs other libraries
- [ ] JSON Writer
- [ ] Rewrite without recursion