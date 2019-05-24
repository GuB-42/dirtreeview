#ifndef MainWindow_h_
#define MainWindow_h_

#include <QMainWindow>

class Node;
class QMutex;
class QTimer;
class TreeModel;

namespace Ui {
	class MainWindow;
}
class MainWindow : public QMainWindow {
	Q_OBJECT
public:
	explicit MainWindow(Node *root_node, QMutex *node_tree_mutex,
	                    QWidget *parent = NULL);
	virtual ~MainWindow();

private:
	Ui::MainWindow *ui;
	TreeModel *treeModel;
	QTimer *updateTimer;
};

#endif
