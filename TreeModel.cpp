#include "TreeModel.h"

#include <algorithm>
#include <vector>
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
	return it->second.children.size();
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
	if (it_parent == mNodeHash.end() || row < 0 ||
	    row >= (int)it_parent->second.children.size()) return QModelIndex();
	Node *child_node = it_parent->second.children[row];
	if (!child_node) return QModelIndex();
	const_cast<TreeModel *>(this)->createItem(parent_node, row, child_node);
	return createIndex(row, column, child_node);
}

static void get_child_list(std::vector<Node *> *out, const Node *node)
{
	Node *first_node = NULL;
	for (Node *p = node->child; p; p = p->sibling) {
		if (p->last_child) {
			first_node = p->sibling;
			break;
		}
	}
	if (first_node) {
		out->push_back(first_node);
		for (Node *p = first_node->sibling ; p != first_node; p = p->sibling) {
			out->push_back(p);
		}
	}
}

QModelIndex TreeModel::parent(const QModelIndex &index) const
{
	if (!index.isValid()) return QModelIndex();
	Node *node = static_cast<Node *>(index.internalPointer());
	if (!node || node == mRootNode) return QModelIndex();
	NodeHash::const_iterator it = mNodeHash.find(node);
	if (it == mNodeHash.end() || !it->second.parent ||
	    it->second.parent == mRootNode) return QModelIndex();
	NodeHash::const_iterator it_parent = mNodeHash.find(it->second.parent);
	if (it_parent == mNodeHash.end()) return QModelIndex();
	return createIndex(it_parent->second.row, 0, it->second.parent);
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

void TreeModel::createItem(Node *parent, int row, Node *node)
{
	if (mNodeHash.find(node) != mNodeHash.end()) return;

	TreeModelItem item(parent, row);
	mNodeTreeMutex->lock();
	get_child_list(&item.children, node);
	node->has_new_children = false;
	mNodeTreeMutex->unlock();
	mNodeHash.insert(std::make_pair(node, item));
}

void TreeModel::updateTree()
{
	bool needed;
	mNodeTreeMutex->lock();
	needed = mRootNode->has_new_children;
	mNodeTreeMutex->unlock();
	if (needed) updateNode(mRootNode);
}

void TreeModel::updateNode(Node *node)
{
	NodeHash::iterator node_it = mNodeHash.find(node);
	if (node_it == mNodeHash.end()) return;
	TreeModelItem &item = node_it->second;

	std::vector<Node *> new_children;
	mNodeTreeMutex->lock();
	get_child_list(&new_children, node);
	std::vector<bool> has_new_children(new_children.size());
	for (int i = 0; i < (int)new_children.size(); ++i) {
		has_new_children[i] = new_children[i]->has_new_children;
	}
	node->has_new_children = false;
	mNodeTreeMutex->unlock();

	QModelIndex p_index =
		node == mRootNode ? QModelIndex() : createIndex(item.row, 0, node);
	int idx = 0;
	while (idx < (int)item.children.size()) {
		if (item.children[idx] == new_children[idx]) {
			if (has_new_children[idx]) updateNode(item.children[idx]);
			++idx;
		} else {
			int idx2 = idx + 1;
			while (item.children[idx] != new_children[idx2]) ++idx2;
			beginInsertRows(p_index, idx, idx2 - 1);
			item.children.insert(item.children.begin() + idx,
			                     new_children.begin() + idx,
			                     new_children.begin() + idx2);
			for (int i = idx2; i < (int)item.children.size(); ++i) {
				NodeHash::iterator child_it = mNodeHash.find(item.children[i]);
				if (child_it != mNodeHash.end()) child_it->second.row = i;
			}
			endInsertRows();
			idx = idx2;
		}
	}
	if (idx < (int)new_children.size()) {
		beginInsertRows(p_index, idx, new_children.size() - 1);
		item.children = new_children;
		endInsertRows();
	}
}
