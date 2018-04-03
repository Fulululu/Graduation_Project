#!/usr/bin/python
# -*- coding: utf-8 -*-

import sqlite3

database_name = "db.sqlite3"
conn = sqlite3.connect('%s' % database_name)  #create the database if it does not exist.
cursor = conn.cursor()

cursor.execute("CREATE TABLE user(UID INTERGER PROMARY KEY NOT NULL, ACCOUNT NVARCHAR(25) UNIQUE NOT NULL, PASSWORD NVARCHAR(25) NOT NULL, CREATETIME TimeStamp NOT NULL DEFAULT (datetime('now','localtime')))")
cursor.execute("CREATE TABLE device(UID INTERGER PRIMARY KEY NOT NULL, LAMP INTERGER CHECK(LAMP>=0 AND LAMP<=1), PUMP INTERGER CHECK(PUMP>=0 AND PUMP<=1))")
cursor.execute("CREATE TABLE light(UID INTERGER NOT NULL, NODE INTERGER NOT NULL, DATA INTERGER DEFAULT '-', CREATETIME TimeStamp NOT NULL DEFAULT (datetime('now','localtime')))")
cursor.execute("CREATE TABLE airtemp(UID INTERGER NOT NULL, NODE INTERGER NOT NULL, DATA REAL DEFAULT '-', CREATETIME TimeStamp NOT NULL DEFAULT (datetime('now','localtime')))")
cursor.execute("CREATE TABLE airhumi(UID INTERGER NOT NULL, NODE INTERGER NOT NULL, DATA INTERGER DEFAULT '-', CREATETIME TimeStamp NOT NULL DEFAULT (datetime('now','localtime')))")
cursor.execute("CREATE TABLE soiltemp(UID INTERGER NOT NULL, NODE INTERGER NOT NULL, DATA REAL DEFAULT '-', CREATETIME TimeStamp NOT NULL DEFAULT (datetime('now','localtime')))")
cursor.execute("CREATE TABLE soilhumi(UID INTERGER NOT NULL, NODE INTERGER NOT NULL, DATA INTERGER DEFAULT '-', CREATETIME TimeStamp NOT NULL DEFAULT (datetime('now','localtime')))")
cursor.execute("CREATE TABLE co2(UID INTERGER NOT NULL, NODE INTERGER NOT NULL, DATA REAL DEFAULT '-', CREATETIME TimeStamp NOT NULL DEFAULT (datetime('now','localtime')))")

cursor.close()
conn.commit()
conn.close()
