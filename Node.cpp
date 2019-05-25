#include "Node.h"

#include <string.h>
/*
#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <algorithm>
*/

MemPool<char> string_pool;
MemPool<Node> NodePoolAlloc::pool;

Node::Node(const char *n, size_t n_size) :
	child(NULL), sibling(NULL), last_child(false), has_new_children(false)
{
	name = string_pool.alloc(n_size + 1);
	memcpy(name, n, n_size);
	name[n_size] = '\0';
}

Node::~Node() {
	string_pool.free(name);
}

Node *Node::insert_node(const char *path)
{
	Node *cur_node = this;
	size_t path_len = strlen(path);
	const char *path_ptr = path;
	while (path_ptr) {
		size_t subpath_len = path_len - (path_ptr - path);

		const char *slash_ptr = strchr(path_ptr, '/');
		if (slash_ptr) {
			subpath_len = slash_ptr - path_ptr;
		}

		Node *insert_after_point = NULL;
		bool insert_last_child = false;

		Node *found_p = NULL;
		if (cur_node->child) {
			Node *p = cur_node->child;
			Node *prev_p = NULL;
			while (!found_p && !insert_after_point) {
				int cmp_res = strncmp(path_ptr, p->name, subpath_len);
				if (cmp_res == 0) {
					if (p->name[subpath_len] != '\0') cmp_res = -1;
				}
				if (cmp_res == 0) {
					found_p = p;
				} else if (cmp_res > 0) {
					if (p->last_child) {
						insert_after_point = p;
						insert_last_child = true;
					} else {
						prev_p = p;
						p = p->sibling;
					}
				} else if (cmp_res < 0) {
					if (prev_p) {
						insert_after_point = prev_p;
						insert_last_child = false;
					} else {
						if (p == p->sibling) {
							insert_after_point = p;
							insert_last_child = false;
						} else {
							prev_p = p;
							while (!prev_p->last_child) {
								prev_p = prev_p->sibling;
							}
							p = prev_p->sibling;
						}
					}
				}
			}
		}

		if (!found_p) {
			Node *new_p = new Node(path_ptr, subpath_len);
			if (insert_after_point) {
				if (insert_last_child) {
					new_p->last_child = true;
					insert_after_point->last_child = false;
				}
				new_p->sibling = insert_after_point->sibling;
				insert_after_point->sibling = new_p;
			} else {
				new_p->last_child = true;
				new_p->sibling = new_p;
			}
			found_p = new_p;
		}

		cur_node->has_new_children = true;
		cur_node->child = found_p;
		cur_node = found_p;
		path_ptr = slash_ptr;
		if (path_ptr) ++path_ptr;
	}

	return cur_node;
}
