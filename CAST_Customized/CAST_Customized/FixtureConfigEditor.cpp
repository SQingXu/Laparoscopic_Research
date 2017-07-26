#include "stdafx.h"

#define _USE_MATH_DEFINES // for C++
#include <cmath>

#include "FixtureConfigEditor.h"
//#include "tokenizer.h"

#include <algorithm>
#include <string>

#include "json/json.h"
#include <iostream>
#include <fstream>

using namespace std;

CFixtureConfigEditor::CFixtureConfigEditor()
{

}

CFixtureConfigEditor::~CFixtureConfigEditor()
{

}

int CFixtureConfigEditor::readFixtureParams(string folderPath, MatrixXd &params)
{
	CString filePath;
	CString folderName = _T("config\\");
	string fileName(FIXTURE_CONFIG_FILE_NAME);
	filePath = CString::CStringT(CA2CT(folderPath.c_str())) + folderName + CString::CStringT(CA2CT(fileName.c_str()));


	FILE* sFile = fopen(CT2CA(filePath.operator LPCWSTR()), "r");
	// if unable to open file
    if (sFile == nullptr)
    {
        return _FAIL_;
    }

    fseek(sFile, 0, SEEK_END);
    size_t size = ftell(sFile);
	string jsonStr;
    jsonStr.resize(size);
    rewind(sFile);
    fread(&jsonStr[0], 1, size, sFile);
    fclose(sFile);


	Json::Reader reader;
	Json::Value root;
	bool bParsing = reader.parse(jsonStr, root);
	if (!bParsing)
	{
		return _FAIL_;
	}

	if (!root.isMember("left"))	return _FAIL_;
	if (!root.isMember("right")) return _FAIL_;

	params.setZero(2, 20);		// 2 X 20 parameters
	Json::Value curInfo;
	for (int i=0 ; i<2 ; i++)
	{
		if ((FIXTURE)i == LEFT)
			curInfo = root["left"];
		else
			curInfo = root["right"];
	
		if (!curInfo.isMember("Pg")) return _FAIL_;
		params(i,0) = curInfo["Pg"][0].asDouble();
		params(i,1) = curInfo["Pg"][1].asDouble();
		params(i,2) = curInfo["Pg"][2].asDouble();
	
		if (!curInfo.isMember("yawAngle"))	return _FAIL_;
		params(i,3) = curInfo["yawAngle"][0].asDouble();
		params(i,4) = curInfo["yawAngle"][1].asDouble();

		if (!curInfo.isMember("insLength")) return _FAIL_;
		params(i,5) = curInfo["insLength"][0].asDouble();
		params(i,6) = curInfo["insLength"][1].asDouble();

		if (!curInfo.isMember("pitAngle")) return _FAIL_;
		params(i,7) = curInfo["pitAngle"][0].asDouble();
		params(i,8) = curInfo["pitAngle"][1].asDouble();

		if (!curInfo.isMember("rolAngle")) return _FAIL_;
		params(i,9) = curInfo["rolAngle"][0].asDouble();
		params(i,10) = curInfo["rolAngle"][1].asDouble();

		if (!curInfo.isMember("yawEnc")) return _FAIL_;
		params(i,11) = curInfo["yawEnc"][0].asDouble();
		params(i,12) = curInfo["yawEnc"][1].asDouble();

		if (!curInfo.isMember("insEnc")) return _FAIL_;
		params(i,13) = curInfo["insEnc"][0].asDouble();
		params(i,14) = curInfo["insEnc"][1].asDouble();

		if (!curInfo.isMember("pitEnc")) return _FAIL_;
		params(i,15) = curInfo["pitEnc"][0].asDouble();
		params(i,16) = curInfo["pitEnc"][1].asDouble();

		if (!curInfo.isMember("rolEnc")) return _FAIL_;
		params(i,17) = curInfo["rolEnc"][0].asDouble();
		params(i,18) = curInfo["rolEnc"][1].asDouble();

		if (!curInfo.isMember("alpha")) return _FAIL_;
		params(i,19) = curInfo["alpha"].asDouble();
	}
	

	return _OK_;
}

int CFixtureConfigEditor::genFixConfigJsonFile(string folderPath, FIXTURE fixture, MatrixXd params)
{
	Json::Value root;

	// 1. read existing file
	bool bNoVailidFile = false;
	CString filePath;
	CString folderName = _T("config\\");
	string fileName(FIXTURE_CONFIG_FILE_NAME);
	filePath = CString::CStringT(CA2CT(folderPath.c_str())) + folderName + CString::CStringT(CA2CT(fileName.c_str()));


	FILE* sFile = fopen(CT2CA(filePath.operator LPCWSTR()), "r");
	// if unable to open file
    if (sFile == nullptr)
    {
        // need to create null file
		bNoVailidFile = true;
    }
	else {
		fseek(sFile, 0, SEEK_END);
		size_t size = ftell(sFile);
		string jsonStr;
		jsonStr.resize(size);
		rewind(sFile);
		fread(&jsonStr[0], 1, size, sFile);
		fclose(sFile);

		Json::Reader reader;
		bool bParsing = reader.parse(jsonStr, root);
		if (!bParsing)
		{
			// need to create null file
			bNoVailidFile = true;
		}
	}

	if (bNoVailidFile) {
		//create json file
		createNullFixConfigJsonFile(folderPath);
		FILE* sFile = fopen(CT2CA(filePath.operator LPCWSTR()), "r");
		if (sFile == nullptr)
		{
			return _FAIL_;
		}
		fseek(sFile, 0, SEEK_END);
		size_t size = ftell(sFile);
		string jsonStr;
		jsonStr.resize(size);
		rewind(sFile);
		fread(&jsonStr[0], 1, size, sFile);
		fclose(sFile);

		Json::Reader reader;
		bool bParsing = reader.parse(jsonStr, root);
		if (!bParsing)
		{
			return _FAIL_;
		}
	}
	
	if (fixture == LEFT) {
		root["left"]["Pg"][0] =  Json::Value(params(0));
		root["left"]["Pg"][1] =  Json::Value(params(1));
		root["left"]["Pg"][2] =  Json::Value(params(2));
		root["left"]["yawAngle"][0] =  Json::Value(params(3));
		root["left"]["yawAngle"][1] =  Json::Value(params(4));
		root["left"]["insLength"][0] =  Json::Value(params(5));
		root["left"]["insLength"][1] =  Json::Value(params(6));
		root["left"]["pitAngle"][0] =  Json::Value(params(7));
		root["left"]["pitAngle"][1] =  Json::Value(params(8));
		root["left"]["rolAngle"][0] =  Json::Value(params(9));
		root["left"]["rolAngle"][1] =  Json::Value(params(10));
		root["left"]["yawEnc"][0] =  Json::Value((long)params(11));
		root["left"]["yawEnc"][1] =  Json::Value((long)params(12));
		root["left"]["insEnc"][0] =  Json::Value((long)params(13));
		root["left"]["insEnc"][1] =  Json::Value((long)params(14));
		root["left"]["pitEnc"][0] =  Json::Value((long)params(15));
		root["left"]["pitEnc"][1] =  Json::Value((long)params(16));
		root["left"]["rolEnc"][0] =  Json::Value((long)params(17));
		root["left"]["rolEnc"][1] =  Json::Value((long)params(18));
		root["left"]["alpha"] =  Json::Value(params(19));
	}
	else {
		root["right"]["Pg"][0] =  Json::Value(params(0));
		root["right"]["Pg"][1] =  Json::Value(params(1));
		root["right"]["Pg"][2] =  Json::Value(params(2));
		root["right"]["yawAngle"][0] =  Json::Value(params(3));
		root["right"]["yawAngle"][1] =  Json::Value(params(4));
		root["right"]["insLength"][0] =  Json::Value(params(5));
		root["right"]["insLength"][1] =  Json::Value(params(6));
		root["right"]["pitAngle"][0] =  Json::Value(params(7));
		root["right"]["pitAngle"][1] =  Json::Value(params(8));
		root["right"]["rolAngle"][0] =  Json::Value(params(9));
		root["right"]["rolAngle"][1] =  Json::Value(params(10));
		root["right"]["yawEnc"][0] =  Json::Value((long)params(11));
		root["right"]["yawEnc"][1] =  Json::Value((long)params(12));
		root["right"]["insEnc"][0] =  Json::Value((long)params(13));
		root["right"]["insEnc"][1] =  Json::Value((long)params(14));
		root["right"]["pitEnc"][0] =  Json::Value((long)params(15));
		root["right"]["pitEnc"][1] =  Json::Value((long)params(16));
		root["right"]["rolEnc"][0] =  Json::Value((long)params(17));
		root["right"]["rolEnc"][1] =  Json::Value((long)params(18));
		root["right"]["alpha"] =  Json::Value(params(19));
	}
	
		
	std::string str;
	Json::StyledWriter writer;
	str = writer.write(root);


	FILE *pJsonFile;
	pJsonFile = fopen(CT2CA(filePath.operator LPCWSTR()), "wt");
	if (pJsonFile != NULL)
			fwrite(str.c_str(), 1, str.length(), pJsonFile);

	if (pJsonFile)
	{
		fclose(pJsonFile);
	}

	return _OK_;
}


void CFixtureConfigEditor::createNullFixConfigJsonFile(string folderPath)
{
	std::string str;

	Json::Value root;
	if (folderPath.find("ua") == std::string::npos)
		str = "unc";
	else 
		str = "ua";

	root["platform"] = str;
	Json::Value left, right;
	Json::Value vec(Json::arrayValue);
    vec.append(Json::Value(0.0));
    vec.append(Json::Value(0.0));
    vec.append(Json::Value(0.0));
	left["Pg"] = vec;
	right["Pg"] = vec;

	vec.clear();
	vec.append(Json::Value(0.0));
    vec.append(Json::Value(0.0));
	left["yawAngle"] = vec;
	left["pitAngle"] = vec;
	left["insLength"] = vec;
	left["rolAngle"] = vec;
	left["yawEnc"] = vec;
	left["pitEnc"] = vec;
	left["insEnc"] = vec;
	left["rolEnc"] = vec;
	left["alpha"] = 0;

	right["yawAngle"] = vec;
	right["pitAngle"] = vec;
	right["insLength"] = vec;
	right["rolAngle"] = vec;
	right["yawEnc"] = vec;
	right["pitEnc"] = vec;
	right["insEnc"] = vec;
	right["rolEnc"] = vec;
	right["alpha"] = 0;

	root["left"] = left;
	root["right"] = right;
		

	Json::StyledWriter writer;
	str = writer.write(root);

	CString filePath;
	CString folderName = _T("config\\");
	string fileName(FIXTURE_CONFIG_FILE_NAME);
	filePath = CString::CStringT(CA2CT(folderPath.c_str())) + folderName + CString::CStringT(CA2CT(fileName.c_str()));

	FILE *pJsonFile;
	pJsonFile = fopen(CT2CA(filePath.operator LPCWSTR()), "wt");
	if (pJsonFile != NULL)
			fwrite(str.c_str(), 1, str.length(), pJsonFile);

	if (pJsonFile)
	{
		fclose(pJsonFile);
	}
}







