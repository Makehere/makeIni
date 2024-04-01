# MakeIni C++ Library

MakeIni is a versatile C++ library designed to simplify the handling of INI and ENV files. It offers a range of functionalities, including typecasting, inline comments handling, variable reading, changing default types for numbers, and efficient operations for working with INI and ENV files.

## Key Features

- **Typecasting:**
Easily convert values to appropriate data types, such as integers, strings, doubles, booleans, and more. For casting it relies on variant which is C++17 feature.

- **Implicit Typecasting:**
By default it uses implicit typecasting for values in senerios where compiler can predict data type with Tmp i.e
```cpp

```
- **Inline Comments Handling:**
Inline comments are allowed with both section and key value pairs. 

- **Variable Reading:** Read and handle variables, supporting dynamic values in your configurations also ignore quotes in cases you don't want to do comment just use the # symbol.

```env
APP_NAME = MYAPP #My first comment
APP_KEY = key
VITE_APP_NAME = ${APP_NAME}
VITE_APP_KEY = "${VITE_APP_KEY}" #now with quotes on I can comment too even after variable 

APP_KEY_HASH = "agbhtdd#ggfdWavhu;788" #The real comment is starting from here

DONNOT_WANT_THIS_NUMBER_TO_BE_CASTED = "678" #When you are using quotes like this on your number it won't be converted into string even though you are using typecasting to read data. It is to ensure that if you want to deal with some numbers by yourself you can easily do it you will get this number without quotes in a string

NUMBER = 788 #It would be read as a number
DOUBLE = 99.1 #It would be read as a double 
```

- **INI and ENV File Operations:** Comprehensive support for working with INI and ENV file formats.

- **Extensive Helper Functions:** Includes a set of helper functions for printing values, reading, and writing INI and ENV files.

-**Customization:** 
Heavily uses templates for better customization.

- **Imporatant Notice** I will update the library and testcases as soon as possible. The documentation needs an update too because I add the optional implicit type conversion in this library which makes it more flexibale and easy to use.
## Getting Started

1. **Include the Library:** Add the `MakeIni.h` header file to your C++ project.

2. **Namespace:** Utilize the `MakeIni` namespace to access the library functionalities.

3. **Create Instances:** Instantiate objects for reading and writing INI and ENV files. There are two two ways to read a file one when you want to cast values to their appropriate types second is when you just want to get values as string.

##Usage example 
IIt is super easy to use this library I will teach you in 3 steps to use this library.

### 1) Installing
-It is a single header file library which you can easily install like this after downloading from here or vcpkg , Conan package manager.
```bash
vcpkg install makeIni
```
And then you can include the header file like this.
```cpp
#include "MakeIni.h"
```


### 2) Creating Parser Instance
Parser is a class which has functionalities for reading and writing files.  It accepts optional template arguments for the type which should be use for casting integer numbers by default it is `int` but you can also pass `long` or `long long` for this and it also second optional template argument for floating numbers by default parser used `double` but you can also use `long double`;

```cpp
MakeIni::Parser i;//This is right two because template arguments are optional

//This is the template arguments all numbers if you are casting then casted as these given types.
MakeIni::Parser<long long, long double> parser;
```
### 3) Reading files
Before reading files we should know the data type handling.

It uses a `MakeIni::val` variant to store values which looks like this.

```cpp
using val = variant<int, string, double, bool, long, long long, long double>;
```

For ini files we have two kinds of data type.
-1) When we will use `MakeIni::ini_val` all data types will automatically be casted ,it is default. In this case we will use value to store values.
-2) Second is `MakeIni::ini_str` no casting would be done when we use this to store data.In it's case we will use std::string.

Same for env files two data types.
-1) `MakeIni::env` It casts values. Values are store as val variant.
-2) `MakeIni::env_str` It doesn't casts values. It stores values in std::string 


Now start reading file, Parser class provides two functions for reading file one for ini and one for env file.
#### Reading Ini files
For reading ini files we use `readIni` function of parser class. It accepts optional template argument for datatype to store data.
Both `readIni` and `readEnv` functions ignores empty keys.
If you do want to read empty values too,just pass second argument as true. First argument would be file name.

#### With typecasting
```cpp
Parser parser;
//Default we are using ini_val which uses typecasting and it is default argument for template class and function so no need to pass it.
MakeIni::iniReader iniData = MakeIni:: parser.readIni("data.ini");

//Accessing a single value would be like this here get function is a std function from variant header
cout<<std::get<std::string>(iniData["SectionName"]["keyName"])<<"\n";

cout<<std::boolalpha<<std::get<bool>(iniData["SectionName"]["boolValue"])<<"\n";

//You can do also see all values type easily
iniData.printIni();

//For some reason if you want to get values as string after reading file with type casting you can use  parser uncastIni function 
 MakeIni::iniReader<MakeIni::ini_str> strIni = parser.unCastIni(iniData);
 
//Now you don't need to use std::get with value type 
cout<<strIni["SectionName"]["key"]<<"\n";

 //Remember iniData still contains data so you can clear it
iniData.clear();
 cout<<strIni["sectionName"]["boolValue"]<<"\n";//It will output value like this "false";
```

#### Reading ini files without typecasting

It is super easy to read an ini file without typecasting. You just need to pass a template argument in readIni function.

```cpp
MakeIni::iniReader<MakeIni::ini_str> iniDataWithOutCasting = parser.readIni<MakeIni::ini_str>("data.ini");

//Accessing a single value 
cout<<iniDataWithOutCasting["sectionName"]["Key"]<<"\n";

```

#### Reading env files with typecasting 
From above we know that by changing template argument we can easily read ini as casted and uncasted, same goes for env. To save ini files data we use `iniReader` class for saving env files data we use `envParsed` class.
```cpp
MakeIni::envParsed envData = parser.readEnv(".env");

cout<<
std::get<string>(envData["Key"])
<<"\n";

//You can uncast env too

MakeIni::envParsed<MakeIni::env_str> env_str_data = parser.unCastEnv(envData);


//As string value 
cout<<
	env_str_data["Key"]
<<"\n";

//For env you have printing functionalities too

env_str_data.printEnv();

//Don't forget to clear cadted data

envData.clear();
```

#### Reading  env files without typecasting

```cpp
MakeIni:: envParsed<MakeIni::env_str> envData = parser.readEnv<MakeIni::env_str>(".env");

cout<<envData["key"]<<"\n";
```

### Writing files

#### Writing Ini files
For writing a file from scratch you should use `ini_str` type for ini files. It is basically a map. And function should be `createIni` from parser claas. It accepts a second optional argument for pretty print which add an extra line after section ending. By default pretty print is true.

```cpp
MakeIni::ini_str writeData;
writeData ["Section1"]["key"] = "value";

writeData["Section2"] = {
	{"boolKey","false"},//pass everything as string 
	{"numbKey", "678"}
	};
//First aargument dara to write, second argument fileName where to write data and third optional argument for pretty print.
parser.createIni(writeData, "data.ini");
```
With pretty print created file would look like this.
```ini
[Section]
a = 88
b = 99

[Section1]
c  = true

```

Without prettyprint
```ini
[Section]
a = 88
b = 99
[Section1]
c  = true
```

Reading from a file and writing back to the other file.
```cpp
MakeIni:: iniReader iniData = parser.readIni("data.ini", true);// true for not ignoring empty keys

parser.createIni(iniData);

//Bow without typecasting 

MakeIni:: iniReader<MakeIni::ini_str> iniData = parser.readIni<MakeIni::ini_str>("data.ini", true);// true for not ignoring empty keys

parser.createIni<MakeIni::ini_str>(iniData);
```

#### Writing env files
Writing env files is pretty similar. If you are writing a file from scratch then type to use is  `env_str` and function would be `writeEnv` from parser class.
```cpp

MakeIni::env_str  writeData = {
	{"key1","value1"},
	{"key2","value2"},
	};
	
parser.writeEnv(writeData, "data.env");


// Reading from a file and writing back to another one. 

MakeIni::envParsed envData = parser.readEnv(".env");

parser.writeEnv(envData);
```
Reading and writing env file without typecasting.

```cpp
MakeIni::envParsed<MakeIni::env_str> envData = parser.readEnv<MakeIni::env_str>(".env");

if(parser.writeEnv<env_str>(envData)){
	cout<<"On successfully writing file all writing functions return true.\n";
	}
```

### Functionalities of iniReader and envParsed class

These classes stores the data of ini and env files as you have seen till now, 
These classes has some functions.

These functions which are listed below share by both iniReader and env class.

Both implements subscript operator `[]` for accessing value. 
In non const Instance case these operator returns a reference.
In const Instance case it will return copy.

```cpp
MakeIni:: iniReader<MakeIni::ini_str> iniData = parser.readIni<MakeIni::ini_str>("data.ini");

cout<<iniData["section"]["key"]<<"\n";//returns a reference

//You can easily change anything even whole section or some keys.

iniData["section"] = {
	{"a","67"}
};

//It cresates an empty key section2
iniData["secrion2"]["key5"];

//Changing values 
iniData["secrion2"]["key3"] = "Changed key3 value.";

// Now const case

const MakeIni:: iniReader<MakeIni::ini_str> iniData2 = parser.readIni<MakeIni::ini_str>("data.ini");

//You can't change any section or value, clearing data or adding new keys is not possible.


// envParsed has some procedure
MakeIni::envParsed envData = parser.readEnv(".env");

cout<<envData["key"]<<"\n";//returns a reference

envData["key"] = true;//also can set int, double, string


//In const it is  immutable 

// envParsed has some procedure
const MakeIni::envParsed<MakeIni::env_str>
envData = parser.readEnv<MakeIni::env_str> (".env");
```
-**size** Gets the size of all key and value in env case and in ini case returns the count of all sections.

-**getAll** Returns the copy of underlying unordered map which holds all data.

-**getAllMutable** Can only be used on non const Instance returns the reference of underlying unordered map.
-**empty** Returns a boolean value whether class holds any data or not.
-**get**. Returns the 

-**clear** Clear all the data.

-**printValue**. It prints values and also tell their type if you are using typecasting.

```cpp
iniData.printValue(iniData["section"]["key"]);


envData.printValue(envData["APP_NAME"]);
```

-**begin** Returns const_iterator of unordered map, because of this function you can also use range loop on data.

```cpp
MakeIni::Parser<long, long double> parser;

MakeIni:: iniReader<MakeIni::ini_val> data = parser.readIni<MakeIni::ini_val>("data.ini", true);

for(auto &it: data){
	//it.firsr would be string it.second would be env
}
```

-**end** Also returns end of map.


#### Functions specific to iniReader
-**hasSection** Returns a boolean whether section exists or not for the given section name.

```cpp
if(iniData.hasSection("SectionName")){
// Section exists 
	}
```
-**hasKey** Returns a boolean if a given section name has the give key.

```cpp
if(iniData.hasKey("mysection", "Keytofound")){
	// section with "mysection" name has "Keytofound"
	}
```

-**printIni** Print all the iniData.


-**get** Get the copy of the section which name you passed here.
```cpp
iniData.printEnvMap(iniData.get("sectionName"));

```

#### Functions specific to envParsed class

-**hasKey** Returns boolean value whether key exists or not.
```cpp
if(envData.hasKey("ketName")){
	
	}
```
-**printEnv** Print all the env data.

--**get** It returns a copy of value, accepts optional template argument for the type by default it is std::string.

```cpp
cout<<envData.get("strValue")<<"\n";

cout<<envData.get<bool>("boolValue")<<"\n";
```

-**Note**. If you like this library kindly give it a star  and also share it with fellow devs. If you find any bug email me at [makemutisticohere@gmail.com](mailto:makemutisticohere@gmail.com)and for contributing you can always fork this repository.
