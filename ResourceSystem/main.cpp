#include <iostream>
#include <sstream>

#include "ResourceSystem.h"

int main()
{

	Fox::JSONFile file;

	Fox::BoolValue bv = false;
	Fox::StringValue sv = "images/sick.png";

	Fox::StringValue sv2 = "images/sick2.png";
	Fox::StringValue sv3 = "images/sick3.png";


	Fox::JSONObject obj;
	std::string truth = "truth_value";
	std::string path = "path";

	std::string child = "child";

	Fox::JSONObject obj2;
	obj2.set("path2", &sv2);
	obj2.set("truth_value2", &bv);

	obj.set(path, &sv);
	obj.set(truth, &bv);
	obj.set(child, &obj2);


	Fox::JSONObject obj3;
	obj3.set("path3", &sv3);
	obj3.set("truth_value3", &bv);

	Fox::JSONObject objArray;
	Fox::StringValue sval = "arvo";
	objArray.set("value", &sval);

	std::string myArrayString = "myArray";

	Fox::JSONValueArray myArray;

	Fox::StringValue testiStr = "testiStringi";
	Fox::BoolValue boolv = false;
	Fox::FloatValue* fv = new Fox::FloatValue(3.1415926f);
	Fox::IntValue iv = 2;

	Fox::JSONValue* arr[4] = {
		&testiStr,
		&objArray,
		&boolv,
		fv
	};


	std::string numval = "a";

	obj.set(numval, fv);

	myArray.fromArray(arr, 4);

	obj3.set(myArrayString, &myArray);

	obj2.set("child2", &obj3);

	//obj3.set("myArray", myArray);

	file.setRoot(obj);

//	std::cout << file << std::endl; 

	std::stringstream ss;
	ss << file;

	Fox::ResourceSystem::writeToJsonFile("oma.json", file);
	std::string str = ss.str();
	Fox::JSONFile file2;
	file2.parse(str);

	Fox::ResourceSystem::writeToJsonFile("parsed.json", file2);
}
