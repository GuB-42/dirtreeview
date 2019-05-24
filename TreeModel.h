#ifndef TreeModel_h_
#define TreeModel_h_

#include <QAbstractItemModel>
#include <QHash>
#include <QVector>

class Node;
class QMutex;

struct TreeModelItem {
	explicit TreeModelItem(Node *p, int r) : parent(p), row(r) {};
	Node *parent;
	int row;
	QVector<Node *> children;
};

class TreeModel : public QAbstractItemModel {
Q_OBJECT
public:
	explicit TreeModel(Node *root_node, QMutex *node_tree_mutex,
	                   QObject *parent = 0);
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;

	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex &index) const;

	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;

public slots:
	void updateTree();

private:
	void createItem(Node *parent, int row, Node *node);
	void insertNodes(Node *node, TreeModelItem *item,
	                 const QVector<Node *> &new_child_list, int idx);
	void updateNode(Node *node);
	void printItem(const Node *node) const;

	Node *mRootNode;
	QMutex *mNodeTreeMutex;
	typedef QHash<const Node *, TreeModelItem> NodeHash;
	NodeHash mNodeHash;
};

#endif
