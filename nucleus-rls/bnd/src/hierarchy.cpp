#include "main.h"

inline void assignToRoot (int* ch, vector<subcore>& skeleton) {
	vector<int> acc;
	int s = *ch;
	while (skeleton[s].root != -1) {
		acc.push_back (s);
		s = skeleton[s].root;
	}
	for (auto i : acc)
		skeleton[i].root = s;
	*ch = s;
}

inline void assignToRepresentative(int* ch, vector<subcore>& skeleton) { // 2-pass path compression
	int u = *ch;
	vector<int> vs;
	while (skeleton[u].parent != -1) {
		int n = skeleton[u].parent;
		if (skeleton[n].K == skeleton[u].K) {
			vs.push_back(u);
			u = n;
		} else
			break;
	}
	*ch = u;
	for (int i : vs) {
		if (i != u)
			skeleton[i].parent = u;
	}
}

inline void store(int uComp, int vComp, vector<int>& unassigned,
		vector<llp>& relations) {
	llp c(vComp, uComp);
	if (uComp == -1) // it is possible that u didn't get an id yet
		unassigned.push_back(relations.size()); // keep those indices to process after the loop, below
	relations.push_back(c);
}

inline void merge(vertex u, vertex v, vector<int>& component, vector<subcore>& skeleton, int* nSubcores) {

	if (component[u] == -1) {
		component[u] = component[v];
		skeleton.erase(skeleton.end() - 1);
	}
	else { // merge component[u] and component[v] nodes
		int child = component[u];
		int parent = component[v];
		assignToRepresentative(&child, skeleton);
		assignToRepresentative(&parent, skeleton);
		if (child != parent) {
			if (skeleton[child].rank > skeleton[parent].rank)
				swap(child, parent);
			skeleton[child].parent = parent;
			skeleton[child].visible = false;
			if (skeleton[parent].rank == skeleton[child].rank)
				skeleton[parent].rank++;
			*nSubcores--;
		}
	}
}

void createSkeleton(vertex u, initializer_list<vertex> neighbors, vertex* nSubcores, vector<vertex>& K,
		vector<subcore>& skeleton, vector<int>& component, vector<int>& unassigned, vector<llp>& relations) {
	vertex smallest = -1, minK = INT_MAX;
	for (auto i : neighbors)
		if (K[i] != -1 && K[i] < minK) {
			smallest = i;
			minK = K[i];
		}
	if (smallest == -1)
		return;

	if (K[smallest] == K[u])
		merge(u, smallest, component, skeleton, nSubcores);
	else
		store(component[u], component[smallest], unassigned, relations);
}

void updateUnassigned (vertex t, vector<int>& component, int* cid, vector<llp>& relations, vector<int>& unassigned) {
	if (component[t] == -1) { // if e didn't get a component, give her a new one
		component[t] = *cid;
		++(*cid);
	}

	// update the unassigned components that are in the relations
	for (int i : unassigned)
		relations[i] = make_pair (relations[i].first, component[t]);
}

void buildHierarchy (vertex cn, vector<llp>& relations, vector<subcore>& skeleton, int* nSubcores, edge nEdge, vertex rightnVtx, vertex leftnVtx) {

	// bin the relations w.r.t. first's K
	vector<vector<llp>> binnedRelations (cn + 1);

	for (int i = 0; i < relations.size(); i++) {
		int a = relations[i].first;
		int b = relations[i].second;
		assignToRepresentative (&a, skeleton);
		assignToRepresentative (&b, skeleton);
		if (a == b)
			continue;
		llp c (a, b);
		binnedRelations[skeleton[a].K].push_back (c);
	}

	// process binnedRelations in reverse order
	for (int i = binnedRelations.size() - 1; i >= 0; i--) {
		vector<llp> mergeList;
		for (llp br : binnedRelations[i]) {
			int a = br.first;
			int root = br.second;
			assignToRoot (&root, skeleton);
			if (a != root) {
				if (skeleton[a].K < skeleton[root].K) {
					skeleton[root].parent = a;
					skeleton[root].root = a;
				}
				else { // skeleton[root].K == skeleton[a].K
					llp c = (root < a) ? make_pair (root, a) : make_pair (a, root);
					mergeList.push_back (c);
				}
			}
		}

		// handle merges
		for (auto sc : mergeList) {
			int child = sc.first;
			int parent = sc.second;
			assignToRepresentative (&child, skeleton);
			assignToRepresentative (&parent, skeleton);
			if (child != parent) {
				if (skeleton[child].rank > skeleton[parent].rank)
					swap (child, parent);
				skeleton[child].parent = parent;
				skeleton[child].root = parent;
				skeleton[child].visible = false;
				if (skeleton[parent].rank == skeleton[child].rank)
					skeleton[parent].rank++;
			}
		}
	}

	*nSubcores += skeleton.size();

	// root core
	int nid = skeleton.size();
	subcore sc (0);
	for (auto i = 0; i < skeleton.size(); i++)
		if (skeleton[i].parent == -1)
			skeleton[i].parent = nid;

	sc.primarySize = rightnVtx;
	sc.secondarySize = leftnVtx;
	sc.nEdge = nEdge;
	sc.ed = nEdge / double (rightnVtx * leftnVtx);
	skeleton.push_back (sc);
}
