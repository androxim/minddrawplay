/***************************************************************************
 *   Copyright (C) 2007 by J. Adam Wilson   *
 *   jawilson@cae.wisc.edu   *
 *   juergen.mellinger@uni-tuebingen.de: fixed a parsing issue,
 *                                       added receiving thread.
 *                                                                         *
 * $BEGIN_BCI2000_LICENSE$
 *
 * This file is part of BCI2000, a platform for real-time bio-signal research.
 * [ Copyright (C) 2000-2012: BCI2000 team and many external contributors ]
 *
 * BCI2000 is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * BCI2000 is distributed in the hope that it will be useful, but
 *                         WITHOUT ANY WARRANTY
 * - without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_BCI2000_LICENSE$
 ***************************************************************************/

#include "appconnect.h"
#include <sstream>
#include "QDebug"

using namespace std;

appconnect::appconnect()
: mReceivingThread( *this )
{  
  setupUI(); 
  count=0;
  conn=false;
  ready=false;
  totalch = 64;
  datach = new double *[64]; // max channels number
  for (int i=0; i<64; i++)
  {
      datach[i]=new double[10]; // max sample block length
      for (int j=0; j<10; j++)
          datach[i][j]=0;
  }
  mainLayout = new QVBoxLayout;
  mainLayout->addWidget(connectGroup,0);
  mainLayout->addWidget(statesGroup,0);  
  setLayout(mainLayout);
  setWindowTitle("BCI2000 data acquisition");
  setWindowFlags(Qt::WindowTitleHint | Qt::WindowMinimizeButtonHint);
  resize(250,500);  
}

appconnect::~appconnect()
{
  {
    QMutexLocker locker( &mMutex );
    recSocket.close();
  }
  mReceivingThread.wait();
}

//---------------------------------------------------------------
void appconnect::connectButCallback()
{  
  bool sendconnected = true, recconnected = true;
  stringstream str;

  {
    QMutexLocker locker( &mMutex ); // protect access to recSocket.
    recConnection.close();
    recConnection.clear();
    recSocket.close();
  }
  mReceivingThread.wait();

  sendConnection.close();
  sendConnection.clear();
  sendSocket.close();

  recSocket.open(receiveBox->text().toStdString().c_str());
  recConnection.open(recSocket);  

  if (!recConnection.is_open())
  {
    //statusBox->setText("Could not connect to receiving address");
    sendconnected = false;
    str <<"Receive = false ";
  }
  else
  {
    mReceivingThread.start();
    str << "Receive = true ";
  }  

  sendSocket.open(sendBox->text().toStdString().c_str());
  sendConnection.open(sendSocket);
  if (!sendConnection.is_open())
  {
    //statusBox->setText("Could not connect to sending address");
    recconnected = false;
    str << "Send = false";
  }
  else
  {
    str << "Send = true";
  }  

  statusBox->setText(str.str().c_str());
  if (!sendconnected || !recconnected)
    return;
  else
  {
    statusBox->setText("Connected!");
    ready=true;
    //mw->bciconnect = true;
  }
  //if we successfully connected, then start the timer
  timer->start();
}

void appconnect::clearstates()
{
    states.clear();
}

void appconnect::timerUpdate()
{
  //update state display
  QMutexLocker locker( &mMutex );
  if ( abs(states.size()) != abs(stateTable->rowCount()) )
  {
    stateTable->setRowCount( static_cast<int>( states.size( )) );
    for( int row = 0; row < stateTable->rowCount(); ++row )
    {
      stateTable->setItem( row, 0, new QTableWidgetItem );
      stateTable->setItem( row, 1, new QTableWidgetItem );
    }
  }
  int row = 0;
  for( map<string, string>::iterator i = states.begin(); i != states.end(); ++i )
  {
      stateTable->item( row, 0 )->setText( i->first.c_str() );
      stateTable->item( row, 1 )->setText( i->second.c_str() );
      ++row;
  }
}

// This function runs inside its own thread.
void appconnect::ReceivingThread::run()
{
    while( parent.recSocket.is_open() )
    {
        QMutexLocker locker( &parent.mMutex );
        if( parent.recSocket.wait_for_read(10) )
        {
            string line;
            getline( parent.recConnection, line );
            if (line.find("Signal(")==0)
            {
                istringstream ss( line );
                string name, value;
                ss >> name >> value;
                parent.states[name] = value;
                if (abs(stod(value))<100)
                {
                    if (parent.mw->paintw_started)
                        parent.ps->getdata(stod(value)*2);
                    if (parent.wd->start)
                        parent.wd->bcidata(stod(value)*2);
                }
            }
        }
    }
}
//---------------------------------------------------------------
void appconnect::setupUI()
{
  setupConnectionGroup();
  setupStatesGroup();
  resize(minimumSizeHint());

  timer = new QTimer(this);
  timer->connect(timer, SIGNAL(timeout()), this, SLOT(timerUpdate()));
  timer->setInterval(20);
  timer->stop();
}

void appconnect::setupConnectionGroup()
{
  connectGroup = new QGroupBox("Communication Ports");
  //connectGroup->setGeometry(QRect(10, 180, 150, 150));

  connectLayout = new QVBoxLayout;
  receiveLabel = new QLabel("Input IP:Port");
  receiveBox = new QLineEdit();
  receiveBox->setText("localhost:20321");
  sendLabel = new QLabel("Output IP:Port");
  sendBox = new QLineEdit();
  sendBox->setText("localhost:20320");
  statusBox = new QLineEdit;
 // connect(connectBut, SIGNAL(clicked()), this, SLOT(connectButCallback()));

  connectLayout->addWidget(receiveLabel,0);
  connectLayout->addWidget(receiveBox,0);
  connectLayout->addWidget(sendLabel,0);
  connectLayout->addWidget(sendBox,0);
  connectLayout->addWidget(statusBox);

  connectGroup->setLayout(connectLayout);
}

void appconnect::setupStatesGroup()
{
  statesGroup = new QGroupBox("States");
  //statesGroup->setGeometry(QRect(10, 10, 251, 171));

  statesLayout = new QGridLayout;
  statesLayout->setSpacing(6);
  statesLayout->setMargin(0);

  stateTable = new QTableWidget;
  stateTable->setColumnCount(2);
  stateTable->setRowCount(1);
  QStringList headers;
  headers << "State" << "Value";
  stateTable->setHorizontalHeaderLabels(headers);

  statesLayout->addWidget(stateTable,0,0);

  statesGroup->setLayout(statesLayout);
}
