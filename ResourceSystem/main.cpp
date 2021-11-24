#include <iostream>
#include <sstream>

#include "ResourceSystem.h"

int main()
{
	
Fox::JSONFile* file = new Fox::JSONFile;
{
	Fox::BoolValue* bv = new Fox::BoolValue(false);
	Fox::BoolValue* bv2 = new Fox::BoolValue(false);
	Fox::BoolValue* bv3 = new Fox::BoolValue(false);

	Fox::StringValue* sv = new Fox::StringValue("images/sick.png");

	Fox::StringValue* sv2 = new Fox::StringValue("images/sick2.png");
	Fox::StringValue* sv3 = new Fox::StringValue("images/sick3.png");


	Fox::JSONObject* obj = new Fox::JSONObject;
	std::string truth = "truth_value";
	std::string path = "path";

	std::string child = "child";

	Fox::JSONObject* obj2 = new Fox::JSONObject;
	obj2->set("path2", sv2);
	obj2->set("truth_value2", bv);

	obj->set(path, sv);
	obj->set(truth, bv2);
	obj->set(child, obj2);


	Fox::JSONObject* obj3 = new Fox::JSONObject;
	obj3->set("path3", sv3);
	obj3->set("truth_value3", bv3);

	Fox::JSONObject* objArray = new Fox::JSONObject;
	Fox::StringValue* sval = new Fox::StringValue("arvo");
	objArray->set("value", sval);

	std::string myArrayString = "myArray";

	Fox::JSONValueArray* myArray = new Fox::JSONValueArray;

	Fox::StringValue* testiStr = new Fox::StringValue("testiStringi");
	Fox::BoolValue* boolv = new Fox::BoolValue(false);
	Fox::FloatValue* fv = new Fox::FloatValue(3.1415926f);
	Fox::FloatValue* fv2 = new Fox::FloatValue(3.1415926f);

	Fox::IntValue iv = 2;

	Fox::JSONValue* arr[4] = {
		testiStr,
		objArray,
		boolv,
		fv
	};


	std::string numval = "a";

	obj->set(numval, fv2);

	myArray->fromArray(arr, 4);

	obj3->set(myArrayString, myArray);

	obj2->set("child2", obj3);

	//obj3.set("myArray", myArray);

	file->setRoot(*obj);

	//	std::cout << file << std::endl; 

	std::stringstream ss;
	ss << *file;

	Fox::ResourceSystem::writeToJsonFile("oma.json", *file);
	std::string str = ss.str();
	Fox::JSONFile* file2 = new Fox::JSONFile;
	file2->parse(str);

	Fox::ResourceSystem::writeToJsonFile("parsed.json", *file2);

	std::string s = file2->get<Fox::JSONObject>().get<Fox::JSONObject>("child").get<Fox::JSONObject>("child2").get<Fox::JSONValueArray>("myArray").get<Fox::JSONObject>(1).get<Fox::StringValue>("value").get();;
	std::cout << "my str: " << s << std::endl;
	delete file2; }
	delete file;
}
