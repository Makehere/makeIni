#ifndef MAKE_INI_H_

#define MAKE_INI_H_

#include <iostream>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <fstream>
#include <vector>
#include <variant>
#include <cmath>
#include <type_traits>

namespace MakeIni
{

#ifdef _WIN32
const std::string endl = "\r\n";
#else
const std::string endl = "\n";
#endif

/**
* The type which  is used for casting 
* your ini file value to appropriate 
* data types.
*/
using val = std::variant<int, std::string, double, bool, long, long long, long double>;

/**
* Added this for implicit conversion
* and SFINAE.*/

#ifndef MAKE_NO_IMPLICT_CAST
template <typename, typename>
struct has_template_param;

template <typename T, template <typename...> typename Type, typename... Ts>
struct has_template_param<T, Type<Ts...>>
{
	static constexpr bool value = (... || std::is_same_v<T, Ts>);
};
#endif
struct value
{
	val innerVal;
	value() : innerVal("") {}

	/*	value(const char c): innerVal (new string (c)){}
	*/
	/*
	value(const val &c) : innerVal(c) {}
	*/
	value(const char *c) : innerVal(std::string(c)) {}

	template <typename T>
	value(const T &v) : innerVal(v) {}

	template <typename T>
	value &operator=(const T &t)
	{
		innerVal = t;
		return *this;
	}
#ifndef MAKE_NO_IMPLICT_CAST
	template <typename T, typename = std::enable_if_t<has_template_param<T, val>::value>>
	operator T() const
	{
		return std::get<T>(innerVal);
	}
#endif

	template <typename T>
	T get() const
	{
		return std::get<T>(innerVal);
	}

  protected:
	void explicitType(
		std::ostream &stream,
		const std::string &c) const
	{
#ifndef MAKE_INI_HIDE_TYPE_DETAIL
		stream << "(" << c << ") ";
#endif
	}

  public:
	friend std::ostream &operator<<(std::ostream &stream, value const &v)
	{
		if (auto it = std::get_if<std::string>(&v.innerVal))
		{
			v.explicitType(stream, "string");
			stream << *it;
		}
		else if (auto it = std::get_if<int>(&v.innerVal))
		{
			v.explicitType(stream, "int");
			stream << *it;
		}
		else if (auto it = std::get_if<double>(&v.innerVal))
		{
			v.explicitType(stream, "double");
			stream << *it;
		}
		else if (auto it = std::get_if<long>(&v.innerVal))
		{
			v.explicitType(stream, "long");
			stream << *it;
		}
		else if (auto it = std::get_if<long double>(&v.innerVal))
		{
			v.explicitType(stream, "long double");
			stream << *it;
		}
		else if (auto it = std::get_if<long long>(&v.innerVal))
		{
			v.explicitType(stream, "long long");
			stream << *it;
		}

		return stream;
	}
};
/**
* The data which is to read env files.
*/
using env = std::unordered_map<std::string, value>;
/**
* The data which is to read ini files.
*/
using ini_val = std::unordered_map<std::string, env>;

/**
* If you are writing a file from scratch
* than you should write it with this data * type 
*/
using env_str = std::unordered_map<std::string, std::string>;

/**
* If you are writing an ini 
* file from scratch
* than you should write it with this data * type 
*/
using ini_str = std::unordered_map<std::string, env_str>;

/**
* If you are writing an env file with an
* existing ini file data 
*/
using writeEnv = std::variant<env_str, env>;

/**
* If you are writing an ini file with an
* existing ini file data 
*/
using writeInI = std::variant<ini_str, ini_val>;

namespace numeric
{
void strTod(long double &d, const std::string &s, char *&end) //pointer by reference
{
	d = strtold(s.c_str(), &end);
}
void strTod(double &d, const std::string &s, char *&end)
{
	d = strtod(s.c_str(), &end);
}

} // namespace numeric
template <typename T, typename U>
class BaseReader
{
	//#ifdef MAKE_INI_DEFINE_DATA_AS_PUBLIC
	//  public:
	//	T data;
	//#else
  protected:
	T data;

  public:
	template <typename forMap>
	void printEnvMap(const forMap &envData, const std::string &indent = "") const
	{
		if (indent == "")
			std::cout << "Sizeof All keys " << envData.size() << "\n";
		else
			std::cout << "(" << envData.size() << ")\n";
		for (auto &it : envData)
		{
			std::cout << indent << it.first << " = ";
			printValue(it.second);
		}
	}

	//	void print(){}
	//#endif

	BaseReader(T &&data) : data(move(data))
	{
	}
	BaseReader(BaseReader &other) : data(other.data)
	{
	}
	BaseReader(BaseReader &&other) : data(move(other.data))
	{
		other.data.clear();
	}
	~BaseReader()
	{
		data.clear();
	}
	T getAll() const
	{
		return data;
	}
	T &getAllMutable()
	{
		return data;
	}

	U &operator[](const std::string &key)
	{
		return data[key];
	}
	U operator[](const std::string &key) const
	{
		auto it = data.find(key);
		if (it != data.end())
		{
			return it->second;
		}
		return U();
	}

	size_t size() const
	{
		return data.size();
	}
	bool empty() const
	{
		return data.empty();
	}
	typename T::const_iterator begin() const
	{
		return data.begin();
	}
	typename T::const_iterator end() const
	{
		return data.end();
	}
	void clear()
	{
		data.clear();
	}

	void printValue(const std::string &str) const
	{
		std::cout << str << "\n";
	}
	void printValue(const value &v) const
	{
		std::cout << v << "\n";
	}
};
template <typename m = env>
class envParsed : public BaseReader<m,
									typename std::conditional<
										std::is_same<m, env>::value,
										value,
										std::string>::type>
{
  private:
	using ConditionalType = typename std::conditional<
		std::is_same<m, env>::value,
		value,
		std::string>::type;

	template <typename T>
	T subGet(const value &v) const
	{
		return v.template get<T>();
	}
	template <typename T>
	std::string subGet(const std::string &v) const
	{
		return v;
	}

  public:
	//Default constructor
	envParsed(m &&data) : BaseReader<m, ConditionalType>(move(data))
	{
	}

	void printEnv() const
	{
		this->template printEnvMap<m>(this->data);
	}

	/*ConditionalType getExplicit(const string &key) const
	{
		auto it = this->data.find(key);
		if (it != this->data.end())
		{
			return it->second;
		}
		return ConditionalType();
	}*/
#ifndef MAKE_NO_IMPLICT_CAST
//When implicit type casting is turned on
	template <typename T, typename = std::enable_if_t<has_template_param<T, val>::value>>
#else
	template <typename T>
#endif
	T get(const std::string &key) const
	{
		auto it = this->data.find(key);

		if (it != this->data.end())
			return subGet<T>(it->second);
		return T();
	}

	bool hasKey(const std::string &key) const
	{
		return this->data.find(key) != this->data.end();
	}
};
template <typename forMap = ini_val>
class iniReader : public BaseReader<forMap,
									typename std::conditional<
										std::is_same<forMap, ini_val>::value,
										env,
										env_str>::type>
{
  private:
	using ConditionalType = typename std::conditional<
		std::is_same<forMap, ini_val>::value,
		env,
		env_str>::type;

  public:
	iniReader(forMap &&data) : BaseReader<forMap, ConditionalType>(move(data))
	{
	}

	//Returns a copy
	/*ConditionalType get(const string &key)
	{
		return this->data[key];
	}*/
	bool hasSection(const std::string &key) const
	{
		return this->data.find(key) != this->data.end();
	}
	bool hasKey(const std::string &sectionName, const std::string &key) const
	{
		auto it = this->data.find(sectionName);
		if (it != this->data.end())
		{
			return it->second.find(key) != it->second.end();
		}
		return false;
	}

	void printIni() const
	{
		std::cout << "\nTotal sections count " << this->data.size() << ".\n";

		for (auto &it : this->data)
		{
			std::cout << it.first << ": ";
			this->template printEnvMap<ConditionalType>(it.second, "  ");
		}
		std::cout << "\n";
	}

	ConditionalType get(const std::string &key) const
	{
		auto it = this->data.find(key);
		if (it != this->data.end())
		{
			return it->second;
		}
		return ConditionalType();
	}
};

//A good iniParser
template <typename i = int, typename d = double>
class Parser
{
  private:
	//d is s DataType for for double
	mutable d doubleValue;
	//	unordered_map<string, unordered_map<string, string>> iniData;
	//ini_val iniData;
	mutable bool readEmptyValues = false;

	std::vector<std::string> allowedFormats = {"ini", "env"};
	void writeSection(
		const env_str &data, std::string &str) const
	{
		std::string key;
		for (auto &it : data)
		{
			key = trim(it.first);
			if (key.size())
				str += key + " = " + trim(it.second) += "\n";
		}
	}

	template <typename forMap>
	forMap readFile(
		std::string fileName,
		bool processEmptyValues = false,
		int checkSection = 1) const
	{
		forMap iniData = {};
		bool castVal = std::is_same<forMap, ini_val>::value;
		std::ifstream ini = openFile(fileName);
		readEmptyValues = processEmptyValues;

		//cout << "Castval is " << castVal << " " << (is_same<forMap, ini_str>::value)<< "\n";
		std::vector<std::string> kV;
		std::string line,
			currentSection = "default",
			token;
		value token2;

		while (
			getline(ini, line))
		{
			/**
			    * In order to read line token
				* token should not be empty
				* and itshould not be a 
				* comment too.
				*/
			line = trim(line);

			if (line.empty() || line[0] == ';' || line[0] == '#')
			{
				continue;
			}
			if (checkSection && line[0] == '[')
			{
				//cout << "here before section name \n";
				// It is a section
				line = readValue<forMap>(line, false, iniData).template get<std::string>();
				//.get<string>();
				//cout << "here after section name \n";
				int s = line.size();
				//cout << "Size of section " << s << "\n";

				if (s > 2)
				{
					currentSection = line.substr(1, s -
														(line[s - 1] == ']' ? 2 : 1) //We are checking that is section closed correctly or not or if not then we won't destroy session name
					);
				}
			}
			else if (splitKeyValue(line, kV))
			{
				//Make sure that we have keys we need to process
				token2 = readValue<forMap>(kV[1], castVal, iniData);
				if (readEmptyValues)
				{
					assignVal(iniData[currentSection], kV[0], token2);
				}
				else
				// if (kV.size() == 2)
				{
					if (auto it = std::get_if<std::string>(&token2.innerVal))
					{
						//	kV[1] = *it; //get<string>(token2);

						if (!(*it).empty())
							assignVal(iniData[currentSection], kV[0], token2);
					}
					else
					{
						assignVal(iniData[currentSection], kV[0], token2);
					}
				}
			}
		}
		ini.close();
		//return fileData;
		return iniData;
	}

	void assignVal(
		env &keyValueMap,
		const std::string &key,
		const value &token) const
	{
		keyValueMap[key] = token;
	}

	void assignVal(env_str &keyValueMap, const std::string &key, const value &token) const
	{
		if (auto it = std::get_if<std::string>(&token.innerVal))
			keyValueMap[key] = *it;
	}
	template <typename forMap>
	value readSubValues(
		std::string str,
		bool castKeys,
		int size,
		const forMap &iniData,
		bool alreadyChecked = false,
		bool justPass = false) const
	{
		if (alreadyChecked || (str[0] == '$' && str[size - 1] == '}'))
		{
			//It is a variable
			//find the variable in all section
			str = str.substr(2, size - 3);
			for (auto &it : iniData)
			{
				auto j = it.second.find(str);
				if (j != it.second.end())
				{
					return j->second;
				}
			}
			// A variable with this name doesn't exist so return empty string
			return "";
		}
		//It is not a variable return the original string
		if (!justPass && castKeys)
		{
			//cout << str << "\n";
			return castValue(str);
		}
		return str;
	}
	val castValue(std::string &str) const
	{
		if (str == "false")
			return false;
		else if (str == "true")
			return true;

		else if (is_int(str))
		{
			if (std::is_same<i, int>::value)
				return std::stoi(str);
			else if (std::is_same<i, long>::value)
				return std::stol(str);
			else if (std::is_same<i, long long>::value)
				return std::stoll(str);
		}

		if (is_double(str))
		{
			return doubleValue;
		}
		return str;
	}

	bool is_double(const std::string &s) const
	{
		char *end = nullptr;

		//d = strtod(s.c_str(), &end);
		numeric::strTod(doubleValue, s, end);
		return end != s.c_str() && *end == '\0' && doubleValue != HUGE_VAL;
	}
	/**
	* If you want to use other key value
	* and also inline comment after
	* that variable then add "${var}"
	* double quotes after it.
	*/
	template <typename forMap>
	value readValue(std::string str, bool castVal, const forMap &iniData) const
	{
		int size = str.size();
		/**
 	   * Checking that a value is quoted
		* inside double quotes like this
		* "value".
		* If so then pass the remaining
		* part inside from the value.
 	*/
		if (str[0] == '$' && str[size - 1] == '}')
			return readSubValues<forMap>(str, castVal, size, iniData, true);
		if (str[0] == '"' && str[size - 1] == '"')
		{
			return readSubValues<forMap>(str.substr(1, size - 2), castVal, size - 2, iniData, false, true);
		}
		/**
	* Checking for comment inside
	* a string like this
	* MY_DB_PORT = 8080 #Myqli db
	* port
	* We already checked for double
	* quotes string.
	*/
		int index = str.find_last_of("#;");
		if (index != std::string::npos)
		{
			str = rtrim(str.substr(0, index));
			size = str.size();
			if (str[0] == '"' && str[size - 1] == '"')
			{
				//This is a quoted string with a comment after it like this DB_NAME = "Travel booking" #database name
				return readSubValues(str.substr(1, size - 2), castVal, size - 2, iniData);
			}
		}
		return castVal ? castValue(str) : str;
	}

	bool splitKeyValue(const std::string &str, std::vector<std::string> &kV) const
	{
		size_t pos = str.find_first_of("=");
		if (pos != std::string::npos)
		{
			kV.clear();
			std::string token = trim(str.substr(0, pos));
			if (token.empty())
				return false;

			kV.push_back(token);

			kV.emplace_back(trim(str.substr(pos + 1)));
			return true;
		}
		return false;
	}
	template <typename T>
	void checkFile(T &ini) const
	{
		if (!ini.is_open())
			throw std::runtime_error("The given file could not be opened.");
	}

	bool checkExt(const std::string &fileName) const
	{
		std::string ext = fileName.substr(fileName.find_last_of(".") + 1);
		if (
			find(
				allowedFormats.begin(),
				allowedFormats.end(),
				ext) == allowedFormats.end())
			throw std::invalid_argument("The given file must be of ini or env type.");
		return true;
	}

	void writeSection(
		const env &data,
		std::string &str) const
	{
		std::string key;
		for (auto &it : data)
		{
			key = trim(it.first);
			if (key.size())
				str += key + " = " + getStrVal(it.second.innerVal) += MakeIni::endl;
		}
	}
	envParsed<env_str> unCastEnv(const envParsed<env> &data) const
	{
		return envParsed<env_str>(
			move(
				unCastEnv(data.getAll())));
	}
	env_str unCastEnv(const env &data) const
	{
		env_str strMap = {};
		for (auto &it : data)
		{
			strMap[it.first] = getStrVal(it.second.innerVal);
		}
		return strMap;
	}

	iniReader<ini_str> unCastIni(const iniReader<ini_val> &data) const
	{
		ini_str strMap = {};

		for (auto &it : data)
		{
			strMap[it.first] = unCastEnv(it.second);
		}

		return iniReader<ini_str>(move(strMap));
	}

  public:
	std::string trim(std::string str) const
	{
		return rtrim(ltrim(str));
	}
	std::string ltrim(std::string str) const
	{
		return str.erase(0, str.find_first_not_of(" \t\n\r\f\v"));
	}
	std::string rtrim(std::string str) const
	{
		return str.erase(str.find_last_not_of(" \t\n\r\f\v") + 1);
	}

	template <typename forMap = ini_val>
	iniReader<forMap> readIni(
		const std::string &fileName,
		const bool &processEmptyValues = false) const

	{
		return iniReader<forMap>(
			move(
				readFile<forMap>(fileName, processEmptyValues, 1)));
		;
	}

	std::ifstream openFile(const std::string &fileName) const
	{
		checkExt(fileName);
		std::ifstream ini(fileName);

		checkFile(ini);
		return ini;
	}

	/**
	* In env case we read empty values
	* but if you want you can ignore 			* them.
	*/
	template <typename formap = env>
	envParsed<formap>
	readEnv(
		std::string fileName,
		bool processEmptyValues = false)
		const
	{
		return envParsed<formap>(
			move(
				readFile<
					typename std::conditional<
						std::is_same<
							formap, env>::value,
						ini_val,
						MakeIni::ini_str>::type>(
					fileName,
					processEmptyValues,
					0)["default"]));
	}

	std::ofstream createFile(std::string fileName) const
	{
		checkExt(fileName);
		std::ofstream f(fileName);
		checkFile(f);
		return f;
	}

	bool createIni(
		const writeInI &data,
		std::string fileName,
		bool prettyPrint = true) const
	{
		std::string str;
		visit([&str, prettyPrint, this](auto &arg) {
			int index = 0;
			for (auto &it : arg)
			{
				if (prettyPrint && index)
					str += MakeIni::endl;
				index++;
				str += "[" + trim(it.first) + "]" + MakeIni::endl;
				this->writeSection(it.second, str);
			}
		},
			  data);
		writeToFile(str, fileName);
		return true;
	}
	template <typename formap = ini_val>
	bool createIni(
		const iniReader<formap> &data,
		std::string fileName,
		bool prettyPrint = true) const
	{
		return createIni(
			static_cast<const writeInI &>(data.getAll()),
			fileName,
			prettyPrint);
	}

	bool writeEnv(
		const writeEnv &data,
		const std::string &fileName) const
	{
		std::string str;

		visit([&str, this](auto &arg) {
			this->writeSection(arg, str);
		},
			  data);

		writeToFile(str, fileName);
		return true;
	}
	template <typename formap = env>
	bool writeEnv(
		const envParsed<formap> &data,
		std::string fileName) const
	{
		return writeEnv(
			static_cast<const MakeIni::writeEnv &>(data.getAll()), fileName);
	}

	void writeToFile(
		const std::string &str, std::string fileName) const
	{
		std::ofstream f = createFile(fileName);
		f << str;
		f.close();
	}

	bool is_int(const std::string &s) const
	{
		std::string::const_iterator it = s.begin();
		while (it != s.end() && std::isdigit(*it))
			++it;
		return !s.empty() && it == s.end();
	}

	std::string getStrVal(const val &v) const
	{
		if (auto it = std::get_if<std::string>(&v))
		{
			return trim(*it);
		}
		else if (
			auto it = std::get_if<d>(&v))
		{
			return std::to_string(*it);
		}
		else if (auto it = std::get_if<int>(&v))
		{
			return std::to_string(*it);
		}
		else if (std::holds_alternative<bool>(v))
		{
			bool boolean = std::get<bool>(v);
			if (boolean)
				return "true";
			return "false";
			//It needs to be a bool
		}
		return "";
	}
};
}; // namespace MakeIni
#endif