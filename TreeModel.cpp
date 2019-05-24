#include "TreeModel.h"

#include <QMutex>
#include "Node.h"

TreeModel::TreeModel(Node *root_node, QMutex *node_tree_mutex,
                     QObject *parent) :
	QAbstractItemModel(parent),
	mRootNode(root_node), mNodeTreeMutex(node_tree_mutex)
{
	createItem(NULL, 0, root_node);
}

int TreeModel::rowCount(const QModelIndex &parent) const
{
	Node *parent_node = parent.isValid() ?
		static_cast<Node *>(parent.internalPointer()) : mRootNode;

	if (!parent_node) return 0;
	NodeHash::const_iterator it = mNodeHash.find(parent_node);
	if (it == mNodeHash.end()) return 0;
	return it->children.count();
}

int TreeModel::columnCount(const QModelIndex &parent) const
{
	if (parent.isValid() && parent.column() != 0) return 0;
	return 1;
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const
{
	Node *parent_node = parent.isValid() ?
		static_cast<Node *>(parent.internalPointer()) : mRootNode;

	if (!parent_node) return QModelIndex();;
	NodeHash::const_iterator it_parent = mNodeHash.find(parent_node);
	if (it_parent == mNodeHash.end() ||
	    row < 0 || row >= it_parent->children.count()) return QModelIndex();
	Node *child_node = it_parent->children[row];
	if (!child_node) return QModelIndex();
	const_cast<TreeModel *>(this)->createItem(parent_node, row, child_node);
	return createIndex(row, column, child_node);
}

void TreeModel::createItem(Node *parent, int row, Node *node)
{
	if (mNodeHash.contains(node)) return;

	TreeModelItem item(parent, row);
	mNodeTreeMutex->lock();
	Node *first_node = NULL;
	for (Node *p = node->child; p; p = p->sibling) {
		if (p->last_child) {
			first_node = p->sibling;
			break;
		}
	}
	if (first_node) {
		item.children.append(first_node);
		for (Node *p = first_node->sibling ; p != first_node; p = p->sibling) {
			item.children.append(p);
		}
	}
	node->has_new_children = false;
	mNodeTreeMutex->unlock();
	mNodeHash.insert(node, item);
}

QModelIndex TreeModel::parent(const QModelIndex &index) const
{
	if (!index.isValid()) return QModelIndex();
	Node *node = static_cast<Node *>(index.internalPointer());
	if (!node || node == mRootNode) return QModelIndex();
	NodeHash::const_iterator it = mNodeHash.find(node);
	if (it == mNodeHash.end() || !it->parent ||
	    it->parent == mRootNode) return QModelIndex();
	return createIndex(it->row, 0, it->parent);
}

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid()) return QVariant();
	Node *node = static_cast<Node *>(index.internalPointer());
	if (!node) return QVariant();

	switch (role) {
	case Qt::DisplayRole:
		return node->name;
	default:
		return QVariant();
	}
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
	if (!index.isValid()) return 0;
	Node *node = static_cast<Node *>(index.internalPointer());
	if (!node) return 0;

	return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

void TreeModel::insertNodes(Node *node, TreeModelItem *item,
                            const QVector<Node *> &new_child_list, int idx)
{
	if (new_child_list.isEmpty()) return;
	QModelIndex p_index =
		node == mRootNode ? QModelIndex() : createIndex(item->row, 0, node);
	beginInsertRows(p_index, idx, idx + new_child_list.count() - 1);
	if (idx < item->children.count()) {
		item->children.insert(idx, new_child_list.count(), NULL);
		for (int i = 0; i < new_child_list.count(); ++i) {
			item->children[idx + i] = new_child_list[i];
		}
		for (int i = idx + new_child_list.count();
		     i < item->children.count(); ++i) {
			NodeHash::iterator it =
				mNodeHash.find(item->children[i]);
			if (it != mNodeHash.end()) it->row = i;
		}
	} else {
		item->children << new_child_list;
	}
	endInsertRows();
}

void TreeModel::updateNode(Node *node)
{
	NodeHash::iterator it = mNodeHash.find(node);
	if (it != mNodeHash.end() && node->has_new_children) {
		Node *first_node = NULL;
		for (Node *p = node->child; p; p = p->sibling) {
			if (p->last_child) {
				first_node = p->sibling;
				break;
			}
		}

		if (first_node) {
			QVector<Node *> to_insert;
			int idx = 0;
			bool first = true;
			Node *p = first_node;
			while (first || p != first_node) {
				if (idx < it->children.count() && it->children[idx] == p) {
					insertNodes(node, &*it, to_insert, idx);
					idx += to_insert.count();
					to_insert.clear();
					updateNode(p);
					++idx;
				} else {
					to_insert.append(p);
				}
				p = p->sibling;
				first = false;
			}
			insertNodes(node, &*it, to_insert, idx);
		}
		node->has_new_children = false;
	}
}

void TreeModel::updateTree()
{
	mNodeTreeMutex->lock();
	updateNode(mRootNode);
	mNodeTreeMutex->unlock();
}

void TreeModel::printItem(const Node *node) const
{
	QString path;
	const Node *p = node;
	while (p) {
		path.insert(0, p->name);
		NodeHash::const_iterator it = mNodeHash.find(p);
		if (it == mNodeHash.end()) {
			path.insert(0, "<?>/");
		} else {
			path.insert(0, QString("[%1]/").arg(it->row));
			p = it->parent;
		}
	}
	printf("%s\n", qPrintable(path));
	NodeHash::const_iterator it = mNodeHash.find(node);
	if (it != mNodeHash.end()) {
		for (int i = 0; i < it->children.count(); ++i) {
			NodeHash::const_iterator child_it = mNodeHash.find(it->children[i]);
			if (child_it == mNodeHash.end()) {
				printf(" - %3d: %s (null)\n",
				       i, it->children[i]->name);
			} else {
				printf(" - %3d: %s (%s[%d])\n",
				       i, it->children[i]->name,
				       child_it->parent ? child_it->parent->name : "<null>",
				       child_it->row);
			}
		}
	}
}
