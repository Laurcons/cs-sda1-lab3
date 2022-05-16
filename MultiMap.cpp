#include "MultiMap.h"
#include "MultiMapIterator.h"
#include <exception>
#include <iostream>

using namespace std;
#define CAP 1000

// theta(1)
MultiMap::MultiMap() {
	cap = CAP;
	knodes = new KNode[cap];
	for (int i = 0; i < cap; i++) {
		auto& knode = knodes[i];
		// initialize the sublist
		knode.cap = CAP;
		knode.vnodes = new VNode[knode.cap];
		for (int j = 0; j < knode.cap; j++) {
			knode.vnodes[j].next = j + 1;
			knode.vnodes[j].value = NULL_TVALUE;
		}
		knode.vnodes[knode.cap - 1].next = -1;
		knode.head = -1;
		knode.firstFree = 0;
		knode.key = NULL_TVALUE;
		// link
		knode.next = i + 1;
	}
	knodes[cap - 1].next = -1;
	firstFree = 0;
	head = -1;
}

// theta(1) amortized
void MultiMap::add(TKey c, TValue v) {
	// search key
	int kpos = head;
	while (kpos != -1 && knodes[kpos].key != c)
		kpos = knodes[kpos].next;
	if (kpos == -1) {
		// make the knode
		if (firstFree == -1) {
			// allocate more in the knodes array
			KNode* oldKnodes = knodes;
			int oldCap = cap;
			cap *= 2;
			knodes = new KNode[cap];
			for (int i = 0; i < cap; i++)
				knodes[i].next = i + 1;
			knodes[cap - 1].next = -1;
			int oldKpos = head;
			kpos = 0;
			while (oldKpos != -1) {
				knodes[kpos] = oldKnodes[oldKpos];
				knodes[kpos].next = kpos + 1;
				kpos++;
				oldKpos = oldKnodes[oldKpos].next;
			}
			knodes[kpos - 1].next = -1;
			// init the sublists on free keys
			for (int i = kpos; i < cap; i++) {
				auto& knode = knodes[i];
				knode.cap = CAP;
				knode.vnodes = new VNode[knode.cap];
				for (int j = 0; j < knode.cap; j++) {
					knode.vnodes[j].next = j + 1;
					knode.vnodes[j].value = NULL_TVALUE;
				}
				knode.vnodes[knode.cap - 1].next = -1;
				knode.head = -1;
				knode.firstFree = 0;
				knode.key = NULL_TVALUE;
			}
			head = 0;
			firstFree = kpos;
			delete[] oldKnodes;
		}
		kpos = firstFree;
		firstFree = knodes[firstFree].next;
		knodes[kpos].key = c;
		knodes[kpos].next = head;
		head = kpos;
	}
	// we have the key, now add the element
	auto& knode = knodes[kpos];
	// add the element to the (front of) vlist
	if (knode.firstFree == -1) {
		// allocate more in the vnodes array
		VNode* oldVnodes = knode.vnodes;
		int oldCap = knode.cap;
		knode.cap *= 2;
		knode.vnodes = new VNode[knode.cap];
		for (int i = 0; i < knode.cap; i++) {
			knode.vnodes[i].value = NULL_TVALUE;
			knode.vnodes[i].next = i + 1;
		}
		knode.vnodes[knode.cap - 1].next = -1;
		int oldVpos = knode.head;
		int vpos = 0;
		while (oldVpos != -1) {
			knode.vnodes[vpos].value = oldVnodes[oldVpos].value;
			knode.vnodes[vpos].next = vpos + 1;
			vpos++;
			oldVpos = oldVnodes[oldVpos].next;
		}
		knode.vnodes[vpos - 1].next = -1;
		knode.head = 0;
		knode.firstFree = kpos;
		delete[] oldVnodes;
	}
	int newPos = knode.firstFree;
	knode.firstFree = knode.vnodes[knode.firstFree].next;
	knode.vnodes[newPos].value = v;
	knode.vnodes[newPos].next = knode.head;
	knode.head = newPos;
}

// theta(1) amortized?
bool MultiMap::remove(TKey c, TValue v) {
	// search key
	int kpos = head;
	while (kpos != -1 && knodes[kpos].key != c)
		kpos = knodes[kpos].next;
	if (kpos == -1)
		return false;
	// we have the key, now search for the element
	auto& knode = knodes[kpos];
	int vpos = knode.head;
	int vprev = -1;
	while (vpos != -1 && knode.vnodes[vpos].value != v) {
		vprev = vpos;
		vpos = knode.vnodes[vpos].next;
	}
	if (vpos == -1)
		return false;
	// redo links
	if (vpos == knode.head) {
		knode.head = knode.vnodes[knode.head].next;
	}
	else {
		knode.vnodes[vprev].next = knode.vnodes[vpos].next;
	}
	// "deallocate"
	knode.vnodes[vpos].next = knode.firstFree;
	knode.firstFree = vpos;
	return true;
}

// theta(1) amortized?
vector<TValue> MultiMap::search(TKey c) const {
	vector<TValue> v{};
	// search key
	int kpos = head;
	while (kpos != -1 && knodes[kpos].key != c)
		kpos = knodes[kpos].next;
	if (kpos == -1)
		return v;
	// parse elements
	auto& knode = knodes[kpos];
	int vc = knode.head;
	while (vc != -1) {
		v.push_back(knode.vnodes[vc].value);
		vc = knode.vnodes[vc].next;
	}
	return v;
}

// theta(n)
int MultiMap::size() const {
	int size = 0;
	int kc = head;
	int vc;
	while (kc != -1) {
		vc = knodes[kc].head;
		while (vc != -1) {
			size++;
			vc = knodes[kc].vnodes[vc].next;
		}
		kc = knodes[kc].next;
	}
	return size;
}

// theta(n)
bool MultiMap::isEmpty() const {
	return this->size() == 0;
}

MultiMapIterator MultiMap::iterator() const {
	return MultiMapIterator(*this);
}

// theta(n)
MultiMap::~MultiMap() {
	for (int i = 0; i < cap; i++) {
		auto& knode = knodes[i];
		delete[] knode.vnodes;
	}
	delete[] knodes;
}

// theta(1)
void MultiMap::empty() {
	// reallocate everything
	for (int i = 0; i < cap; i++) {
		auto& knode = knodes[i];
		delete[] knode.vnodes;
	}
	delete[] knodes;
	cap = CAP;
	knodes = new KNode[cap];
	for (int i = 0; i < cap; i++) {
		auto& knode = knodes[i];
		// initialize the sublist
		knode.cap = CAP;
		knode.vnodes = new VNode[knode.cap];
		for (int j = 0; j < knode.cap; j++) {
			knode.vnodes[j].next = j + 1;
			knode.vnodes[j].value = NULL_TVALUE;
		}
		knode.vnodes[knode.cap - 1].next = -1;
		knode.head = -1;
		knode.firstFree = 0;
		knode.key = NULL_TVALUE;
		// link
		knode.next = i + 1;
	}
	knodes[cap - 1].next = -1;
	firstFree = 0;
	head = -1;
}