#ifndef TreeModel_h_
#define TreeModel_h_

#include <QAbstractItemModel>
#include <unordered_map>
#include <vector>

class Node;
class QMutex;

struct TreeModelItem {
	explicit TreeModelItem(Node *p, int r) : parent(p), row(r) {};
	Node *parent;
	int row;
	std::vector<Node *> children;
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
	void updateNode(Node *node);

	Node *mRootNode;
	QMutex *mNodeTreeMutex;
	typedef std::unordered_map<const Node *, TreeModelItem> NodeHash;
	NodeHash mNodeHash;
};

#endif
