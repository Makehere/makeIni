#include <iostream>
#include <string>
// #define MAKE_INI_HIDE_TYPE_DETAIL
// #define MAKE_NO_IMPLICT_CAST
#include "makeIni2.h"

int main(int argc, char *argv[])
{

	MakeIni::ini_val d;

	d["hi"] = {
		{"b", 5}
	};

	MakeIni::Parser i;
	MakeIni::iniReader b = i.readIni("test.ini");
	b["Section2"]["key"] = "9";
	std::string implicitlyCasted = b["Section2"]["key"];
	MakeIni::iniReader<MakeIni::ini_str> op = i.readIni<MakeIni::ini_str>("test.ini");

	op.printEnvMap(op.get("Section2"));
	MakeIni::iniReader n = move(b);
	std::cout << "By class built in helper "
			  << "\n";
	n.printIni();

	op.printIni();

	std::cout << "after reading \n";

	std::cout << "Op " << op["Section3"]["Key4"] << "\n";

	std::cout << "Printing b \n";
	
	// b.clear();

	std::cout << "printing h\n";

	cout<<boolalpha<<b.hasKey("Section2","Key3")<<"\n";

	std::cout << std::numeric_limits<long long>::max() << "\n";

	// i.readEnv()

MakeIni::envParsed<MakeIni :: env_str> e = i.readEnv < MakeIni::env_str > ("example.env", true);

	e.printEnv();
	e["j"] = "k";

MakeIni::envParsed sd = i.readEnv("example.env", true);

	cout << "\nPrinting sd\n";

	sd.printEnv();

	std::string app_name = sd["APP_NAME"].get<string>();

	std::string app_name_two = sd["APP_NAME"].get<string>();
	std::string app_name_get = sd.get<string>("APP_NAME");

	cout << "Here after creating variable " << app_name << "\n";

	cout << "With explicitly getting it " << app_name_get << "\n";
	// cout<<e["APP_NAME"]<<"\n";

	// cout<<e["j"]<<"\n";
	// env f = e.getAll();

	// f ={};
	// cout<<e.size()<<"\n";

	// const env_str k =  static_cast<env_str>(f);

	// cout<<k["APP_NAME"]<<"\n";
	// cout<<e["APP_NAME"]<<"\n";
	// cout<<get<string>(e["QUEUE_CONNECTION"])<<"\n";
	// iniReader j = move(iniReader);

	// i.writeEnv(e ,"easy.env");

MakeIni::env_str l;
	l["key"] = "You are great.";

	cout << l["key"] << " " << e.get("APP_NAME") << "\n";

	envParsed<env_str> j = MakeIni : envParsed < MakeIni : env_str > (move(l));

	std::cout << j["key"] << "\n";

MakeIni::ini_str k;
	k["u"]["j"] = "88";
	std::cout << k["u"]["j"] << "\n";

MakeIni::envParsed<MakeIni::env_str> s = move(e);

	std::cout << s["APP_NAME"] << "\n";

MakeIni::envParsed<env_str> x = s;

	std::cout << x["PUSHER_APP_ID"] << "\n";

	std::cout << x.get<string>("PUSHER_APP_ID") << "\n";

	MakeIni::ini_str f;
	f["Hj"] = {
		{"k", "l"},
		{"t", "s"}};
	f["kl"] = {
		{"s", "67"}};

MakeIni::iniReader g = i.readIni("test.ini");

	std::cout << "Reading a highly typecasting ini file \n";
	g.printIni();

	std::string bnj = g["Section2"]["Key3"].get<std::string>();


	std::cout << "Variable " << bnj << "\n";

	return 0;
}