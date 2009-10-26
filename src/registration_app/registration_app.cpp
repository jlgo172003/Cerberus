#include "registration_app.h"

#include "ui_add_team.h"
#include "ui_team_view.h"
#include "ui_teammember_edit.h"
#include "util/sql_util.h"

RegistrationApp::RegistrationApp (QWidget* parent)
{
	m_add_team_wnd = new Ui::add_team_wnd;
	m_team_view_wnd = new Ui::team_view_wnd;
	m_teammember_edit_wnd = new Ui::teammember_edit_wnd;

	//stuff for dialogs

	//network?

	//connecting stuff

	//connections for the buttons
	connect(m_add_team_wnd->add_team_btn, SIGNAL(clicked()), this, SLOT(addTeam()));
	connect(m_teammember_edit_wnd->add_member_btn,SIGNAL(clicked()), this, SLOT(addUser()));
	connect(m_teammember_edit_wnd->view_team_info_btn, SIGNAL(clicked()), this, SLOT(viewTeam()));
	connect(m_teammember_edit_wnd->back_to_edit_btn, SIGNAL(clicked()), this, SLOT(backToAddTeam()));


}

//void RegistrationApp::