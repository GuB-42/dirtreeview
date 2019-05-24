#ifndef Node_h_
#define Node_h_

#include <QMetaType>
#include <string>
#include <map>
#include "mempool.h"

struct Node;
struct NodePoolAlloc {
	static void *operator new(size_t size) {
		return pool.alloc(size);
	}
	static void operator delete(void *p) {
		pool.free(static_cast<Node *>(p));
	}
	static MemPool<Node> pool;
};

struct Node : public NodePoolAlloc {
	Node(const char *n, size_t n_size);
	~Node();

	char *name;
	Node *child;
	Node *sibling;
	bool last_child:1;
	bool is_new:1;
	bool has_new_children:1;

	Node *insert_node(const char *path);
	void print_tree(const std::string &prefix = "") const;
private:
	Node(const Node &);
	Node &operator=(const Node &);
};

Q_DECLARE_METATYPE(Node *);

#endif
