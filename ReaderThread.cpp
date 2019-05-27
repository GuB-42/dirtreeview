#include "ReaderThread.h"

#include <QMutex>
#include <QSet>
#include <QTime>
#include <stdio.h>
#include "Node.h"

ReaderThread::ReaderThread(Node *root_node, QMutex *node_tree_mutex,
                           QObject *parent) :
	QThread(parent),
	mRootNode(root_node), mNodeTreeMutex(node_tree_mutex)
{
}


void ReaderThread::read_file(FILE *stream)
{
	size_t read_buf_size = 65536;
	char *read_buf = (char *)malloc(read_buf_size);
	char *read_p = read_buf;
	char *next_p = read_p;
	QTime t;
	bool update_done = false;

	t.start();
	do {
		size_t sz = fread(next_p, 1, read_buf_size - (next_p - read_buf) - 1, stream);
		while (sz > 0) {
			while (sz > 0 && *next_p != '\n' && *next_p != '\r') {
				--sz;
				++next_p;
			}
			if (sz > 0) {
				if (next_p != read_p) {
					*next_p = '\0';
					mNodeTreeMutex->lock();
					mRootNode->insert_node(read_p);
					mNodeTreeMutex->unlock();
					update_done = true;
				}
				--sz;
				read_p = ++next_p;
			}
		}
		if (next_p >= read_buf + read_buf_size - 1) {
			if (read_p != read_buf) {
				memmove(read_buf, read_p, read_buf_size - (read_p - read_buf));
				next_p -= read_p - read_buf;
				read_p = read_buf;
			} else {
				read_buf_size *= 2;
				char *new_read_buf = (char *)realloc(read_buf, read_buf_size);
				next_p = new_read_buf + (next_p - read_buf);
				read_p = read_buf = new_read_buf;
			}
		}
		if (update_done && t.elapsed() > 250) {
			t.restart();
			update_done = false;
			emit treeUpdated();
		}
	} while (!feof(stream) && !ferror(stream));

	if (next_p != read_p) {
		*next_p = '\0';
		mNodeTreeMutex->lock();
		mRootNode->insert_node(read_p);
		mNodeTreeMutex->unlock();
		emit treeUpdated();
	}

	free(read_buf);
}

void ReaderThread::run() {

	read_file(stdin);
}
