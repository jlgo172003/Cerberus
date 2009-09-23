#include "server_app.h"
#include "ui_server_app.h"
#include "net/server_net.h"

ServerApp::ServerApp(QWidget* parent) : QDialog(parent), m_dlg(new Ui::server_dlg){
	m_dlg->setupUi(this);
	m_network = new ServerNetwork(this);
}

ServerApp::~ServerApp(){
	delete m_network;
}

int main(int argc, char* argv[]){
}
