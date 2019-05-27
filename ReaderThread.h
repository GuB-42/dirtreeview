#include <QThread>

class Node;
class QMutex;

class ReaderThread: public QThread {
Q_OBJECT

public:
	ReaderThread(Node *root_node, QMutex *node_tree_mutex,
	             QObject *parent = NULL);

signals:
	void treeUpdated();

protected:
	void run();

private:
	void read_file(FILE *stream);

	Node *mRootNode;
	QMutex *mNodeTreeMutex;
};
