#include <iostream>

#include "ResourceSystem.h"

int main()
{

	Fox::JSONFile file;

	Fox::BoolValue bv = false;
	Fox::StringValue sv = "images/sick.png";

	Fox::JSONObject obj;
	std::string truth = "truth_value";
	std::string path = "path";

	std::string child = "child";

	Fox::JSONObject obj2;
	obj2.set(path, sv);
	obj2.set(truth, bv);

	obj.set(path, sv);
	obj.set(truth, bv);
	obj.set(child, obj2);


	Fox::JSONObject obj3;
	obj3.set(path, sv);
	obj3.set(truth, bv);

	Fox::JSONObject objArray;
	Fox::StringValue sval = "arvo";
	objArray.set("value", sval);

	std::string myArrayString = "myArray";

	Fox::JSONValueArray myArray;

	Fox::StringValue testiStr = "testiStringi";
	Fox::BoolValue boolv = false;


	Fox::JSONValue* arr[3] = {
		&testiStr,
		&objArray,
		&boolv
	};


	myArray.fromArray(arr, 3);

	obj3.set(myArrayString, myArray);

	obj2.set("child2", obj3);

	obj3.set("myArray", myArray);

	file.setRoot(obj);

	std::cout << file << std::endl; 

	Fox::ResourceSystem::writeToJsonFile("oma.json", file);
}
