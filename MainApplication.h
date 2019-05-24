#ifndef MainApplication_h_
#define MainApplication_h_

#include <QApplication>

class MainWindow;
class ReaderThread;
class Node;
class QMutex;

class MainApplication : public QApplication
{
	Q_OBJECT
public:
	MainApplication(int &argc, char *argv[]);
	~MainApplication();

	Node *rootNode() { return mRootNode; };
	QMutex *nodeTreeMutex() { return mNodeTreeMutex; };

private:
	MainWindow *mainWindow;
	ReaderThread *readerThread;
	Node *mRootNode;
	QMutex *mNodeTreeMutex;
};

#endif
