#include "MultiMapIterator.h"
#include "MultiMap.h"


MultiMapIterator::MultiMapIterator(const MultiMap& c): mm(c) {
	first();
}

TElem MultiMapIterator::getCurrent() const{
	if (!valid())
		throw std::exception{};
	int k = mm.knodes[kpos].key;
	int v = mm.knodes[kpos].vnodes[vpos].value;
	return make_pair(k, v);
}

bool MultiMapIterator::valid() const {
	return kpos != -1 && vpos != -1;
}

void MultiMapIterator::next() {
	if (!valid())
		throw std::exception{};
	vpos = mm.knodes[kpos].vnodes[vpos].next;
	if (vpos == -1) {
		kpos = mm.knodes[kpos].next;
		if (kpos != -1)
			vpos = mm.knodes[kpos].head;
	}
}

void MultiMapIterator::first() {
	kpos = mm.head;
	if (kpos != -1)
		vpos = mm.knodes[kpos].head;
}

