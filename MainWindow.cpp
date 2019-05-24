#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QTimer>
#include "TreeModel.h"

MainWindow::MainWindow(Node *root_node, QMutex *node_tree_mutex,
                       QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	treeModel(new TreeModel(root_node, node_tree_mutex, this)),
	updateTimer(new QTimer(this))
{
	ui->setupUi(this);
	connect(ui->action_Quit, SIGNAL(triggered()), this, SLOT(close()));

	ui->treeView->setModel(treeModel);
	updateTimer->setInterval(100);
	connect(updateTimer, SIGNAL(timeout()), treeModel, SLOT(updateTree()));
	updateTimer->start();
}

MainWindow::~MainWindow() {
	delete ui;
}

