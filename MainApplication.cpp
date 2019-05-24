#include "MainApplication.h"

#include <QtCore/QtGlobal>
#include <QMutex>
#include <QTimer>

#include "Node.h"
#include "MainWindow.h"
#include "ReaderThread.h"
#include <unistd.h>

MainApplication::MainApplication(int &argc, char *argv[]) :
	QApplication(argc, argv),
	mRootNode(new Node("<root>", 6)),
	mNodeTreeMutex(new QMutex())
{
	mRootNode->last_child = true;
	mRootNode->sibling = mRootNode;
	readerThread = new ReaderThread(mRootNode, mNodeTreeMutex);
	readerThread->start();
	mainWindow = new MainWindow(mRootNode, mNodeTreeMutex);;
	mainWindow->show();
}

MainApplication::~MainApplication()
{
	delete mainWindow;
	delete readerThread;
	delete mRootNode;
	delete mNodeTreeMutex;
}
