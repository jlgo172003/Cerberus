#ifndef CLIENT_H
#define CLIENT_H
#include <QtGui>
#include <QtNetwork>

namespace Ui{
	class client_dlg;
}

class ClientDlg : public QDialog{
	Q_OBJECT;
public:
	ClientDlg(QWidget* parent = 0);
	~ClientDlg();
private slots:
	void onConnectBtn();
	void onQuitBtn();
	void onConnect();
	void onError(QAbstractSocket::SocketError error);
private:
	QSslSocket* m_socket;
	Ui::client_dlg* m_dlg;
};

#endif