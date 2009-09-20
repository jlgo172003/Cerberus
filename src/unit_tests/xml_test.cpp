#include <QFile>
#include <QString>
#include "tests/xml_test.h"
#include "data_types.h"

void XmlTest::q1ReadTest(){
	QFile file("resources/stage1_q.xml");
	QString xml = file.readAll();
	QVERIFY(xml.size() != 0);
	R1QData qd;
	R1QData td;
	qd.welcome_msg = "Insert welcome message here to be shown in the 1st screen.";
	R1Question qtemp;
	qtemp.question = "What is the bla bla bla?";
	qtemp.choices.insert(pair<int, string>(1, "Bla"));
	qtemp.choices.insert(pair<int, string>(2, "BLA"));
	qtemp.choices.insert(pair<int, string>(3, "bla"));
	qtemp.choices.insert(pair<int, string>(4, "blarg"));
	qd.questions.push_back(qtemp);
	qtemp.choices.clear();
	qtemp.question = "Who is Mr. Shires";
	qtemp.choices.insert(pair<int, string>(1, "Sir Diy"));
	qtemp.choices.insert(pair<int, string>(2, "Doc Mana"));
	qtemp.choices.insert(pair<int, string>(3, "Fr. David"));
	qtemp.choices.insert(pair<int, string>(4, "Michael Jackson"));
	qd.questions.push_back(qtemp);
	m_util.readR1QData(xml, td);
	QVERIFY(td == qd);
}

void XmlTest::q1WriteTest(){
	//TODO: Add a test case wherein the xml output is what's intended
}

void XmlTest::a1ReadTest(){
	QFile file("resources/stage1_a.xml");
	QString xml = file.readAll();
	QVERIFY(xml.size() > 0);
	R1Answers ad, td;
	ad.insert(pair<int, int>(1,1));
	ad.insert(pair<int, int>(2,4));
	m_util.readR1AData(xml, td);
	QVERIFY(td == ad);
}

void XmlTest::a1WriteTest(){
	//TODO: Add a test case wherein the xml output is what's intended
}

void XmlTest::clientConfReadTest(){
	QFile file("resources/client_config.xml");
	QString xml = file.readAll();
	QVERIFY(xml.size() > 0);
	ClientConfig cd, td;
	cd.ip = "192.168.1.100";
	cd.port = 2652;
	m_util.readClientConfig(xml, td);
	QVERIFY(td == cd);
}

void XmlTest::serverConfReadTest(){
	QFile file("resources/server_config.xml");
	QString xml = file.readAll();
	QVERIFY(xml.size() > 0);
	ServerConfig sd, td;
	sd.db_path = "resources/server.db";
	sd.port = 2652;
	StageData stemp;
	stemp.answer_file = "resources/stage1_q.xml";
	stemp.question_file = "resources/stage1_a.xml";
	sd.stage_data.push_back(stemp);
	m_util.readServerConfig(xml, td);
	QVERIFY(td == sd);
}

QTEST_MAIN(XmlTest);