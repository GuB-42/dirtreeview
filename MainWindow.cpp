#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QProgressBar>
#include "TreeModel.h"

MainWindow::MainWindow(Node *root_node, QMutex *node_tree_mutex,
                       QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	progressBar(new QProgressBar(this)),
	treeModel(new TreeModel(root_node, node_tree_mutex, this))
{
	ui->setupUi(this);
	connect(ui->action_Quit, SIGNAL(triggered()), this, SLOT(close()));

	progressBar->setSizePolicy(QSizePolicy::Expanding,
	                           QSizePolicy::Preferred);
	progressBar->setMinimum(0);
	progressBar->setMaximum(0);
	ui->statusbar->addWidget(progressBar);

	ui->treeView->setModel(treeModel);
}

MainWindow::~MainWindow() {
	delete ui;
}

void MainWindow::updateTree() {
	treeModel->updateTree();
}

void MainWindow::treeComplete() {
	treeModel->updateTree();
	progressBar->setMaximum(100);
	progressBar->setValue(100);
}
