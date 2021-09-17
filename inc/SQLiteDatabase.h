#ifndef SQLITEDATABASE_H_
#define SQLITEDATABASE_H_
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>
#include "common.h"



#define SQLITE_DATABASE     "/media/sdcard/database.db"    //"./database.db"
#define TABLE_NAME          "ReportData"



class SqliteDatabase
{
public:
	SqliteDatabase();
	~SqliteDatabase();
	int initDatabase();
	int insertData(unsigned char *pData, int len, int times);
	unsigned int queryCount(int flag, unsigned int id);
	int queryID(int times);
	unsigned int queryMaxID();
	int queryMaxTimes();
	unsigned int queryMaxTimesID();
	int readData(unsigned char *pBuff, int *len, int id);
	int deleteData(int id);
	int test();

private:
	sqlite3 *db;

};

extern SqliteDatabase *pSqliteDatabase;


#endif

