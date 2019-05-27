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
	mainWindow = new MainWindow(mRootNode, mNodeTreeMutex);;
	connect(readerThread, SIGNAL(treeUpdated()), mainWindow, SLOT(updateTree()));
	connect(readerThread, SIGNAL(finished()), mainWindow, SLOT(treeComplete()));
	readerThread->start();
	mainWindow->show();
}

MainApplication::~MainApplication()
{
	delete mainWindow;
	if (readerThread->isRunning()) {
		readerThread->terminate();
		readerThread->wait(2000);
	}
	delete readerThread;
	delete mRootNode;
	delete mNodeTreeMutex;
}
