/*
 * microRadar software
 * (progetto polimi 2022)
 *
 * Salvatore Passaro
 */

#include <wx/wx.h>
#include "mainFrame.h"

class radarApp: public wxApp
{
    wxFrame* m_mainFrame;
public:
    virtual bool OnInit();
};

bool radarApp::OnInit() {
	m_mainFrame = new mainFrame( "microRadar logger 1.1", wxPoint(50, 50) );
    m_mainFrame->Show( true );
	
    return true;
}

wxIMPLEMENT_APP(radarApp);