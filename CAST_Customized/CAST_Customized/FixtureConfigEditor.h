
#ifndef _FIXTURE_CONFIG_EDITOR_H_

#define _FIXTURE_CONFIG_EDITOR_H_

#include <vector>
using namespace std;

#include "common.h"
//#include "ezxml.h"

// for eigen library
#include <Dense>
using namespace Eigen;

class CFixtureConfigEditor
{
public:
	CFixtureConfigEditor();
	~CFixtureConfigEditor();

// For reader
public:
	int readFixtureParams(string folderPath, MatrixXd &params);

// For writer
public:
	int genFixConfigJsonFile(string folderPath, FIXTURE fixture, MatrixXd params);
		void createNullFixConfigJsonFile(string folderPath);
};

#endif