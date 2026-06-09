# JSON Parser for C++

[![pl](https://img.shields.io/badge/lang-pl-red.svg)](https://github.com/Chackapon/json_parser_cpp/blob/main/lang/README.pl.md)
[![en](https://img.shields.io/badge/lang-en-blue.svg)](https://github.com/Chackapon/json_parser_cpp/blob/main/lang/README.en.md)

A C++ shared library that contains a class that allows you to parse a JSON file or string into something, that functions like Python's json library.

Version history
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