// SweepLine Tree with Custom Leaf-Only Segment Storage
#include <iostream>
#include <cmath>
#include <set>
#include <vector>
#include <algorithm>
#include <queue>
#include <unordered_set>

using namespace std;

double sweepY = 0; // Global y-coordinate of sweep line

struct Point {
    double x, y;
    bool operator<(const Point& o) const {
        if (fabs(y - o.y) > 1e-9) return y > o.y;
        return x < o.x;
    }
};

struct Segment {
    Point p, q;
    int id;

    Segment(Point p_, Point q_, int id_) : p(p_), q(q_), id(id_) {
        if (p.y < q.y || (p.y == q.y && p.x > q.x)) swap(p, q);
    }

    double getXatY(double y) const {
        if (fabs(p.y - q.y) < 1e-9) return min(p.x, q.x);
        return p.x + (q.x - p.x) * ((p.y - y) / (p.y - q.y));
    }
};

bool segmentLess(const Segment* a, const Segment* b, bool noid = false) {
    if(!noid){
        double ax = a->getXatY(sweepY);
        double bx = b->getXatY(sweepY);
        return ax < bx || (fabs(ax - bx) < 1e-9 && a->id > b->id);
    }
    else{
        double ax = a->getXatY(sweepY);
        double bx = b->getXatY(sweepY);
        return ax < bx || fabs(ax - bx) < 1e-9;
    }
}

int getTypePriority(int t){
    switch (t) {
        case 2: return 3; // highest priority
        case 0: return 2;
        case 1: return 1;
    }
    return 0;
}

struct Event {
    Point p;
    int type; // 0 = insert, 1 = remove, 2 = intersection
    Segment *s1, *s2;

    bool operator<(const Event& e) const {
        if (p.y != e.p.y) return p.y < e.p.y;
        if(type != e.type) return getTypePriority(type) < getTypePriority(e.type);
        return p.x > e.p.x;
    }
};

struct AVLNode {
    Segment* segment = nullptr;
    Segment* maxLeft = nullptr;
    AVLNode* left = nullptr;
    AVLNode* right = nullptr;
    int height = 1;

    bool isLeaf() const {
        return !left && !right;
    }
};


struct PointHash {
    size_t operator()(const Point& p) const {
        return hash<double>()(p.x) ^ hash<double>()(p.y);
    }
};

bool operator==(const Point& a, const Point& b) {
    return fabs(a.x - b.x) < 1e-9 && fabs(a.y - b.y) < 1e-9;
}

int height(AVLNode* n) {
    return n ? n->height : 0;
}

void update(AVLNode* node) {
    if (!node) return;
    node->height = max(height(node->left), height(node->right)) + 1;
    if (node->left && node->left->isLeaf())
        node->maxLeft = node->left->segment;
    else if (node->left){
        AVLNode* rightmost = node->left;
        while (rightmost->right) rightmost = rightmost->right;
        node->maxLeft = rightmost->segment;
    }  
    else
        node->maxLeft = node->segment;
}

AVLNode* rotateRight(AVLNode* y) {
    AVLNode* x = y->left;
    y->left = x->right;
    x->right = y;
    update(y);
    update(x);
    return x;
}

AVLNode* rotateLeft(AVLNode* x) {
    AVLNode* y = x->right;
    x->right = y->left;
    y->left = x;
    update(x);
    update(y);
    return y;
}

int balanceFactor(AVLNode* n) {
    return n ? height(n->left) - height(n->right) : 0;
}

void printTree(AVLNode* node, int depth = 0, char prefix = '-') {
    if (!node) return;

    for (int i = 0; i < depth; ++i) cout << "    ";
    cout << prefix << " ";

    if (node->isLeaf()) {
        cout << "[Leaf] Segment ID: " << node->segment->id << endl;
    } else {
        if (node->maxLeft)
            cout << "[Internal] maxLeft ID: " << node->maxLeft->id << endl;
        else
            cout << "[Internal] maxLeft: null" << endl;
    }

    printTree(node->left, depth + 1, 'L');
    printTree(node->right, depth + 1, 'R');
}

AVLNode* insert(AVLNode* node, Segment* s) {
    if (!node) return new AVLNode{.segment = s, .maxLeft = s};
    if (node->isLeaf()) {
        AVLNode* newLeaf = new AVLNode{.segment = s, .maxLeft = s};
        AVLNode* parent = new AVLNode{.segment = nullptr};
        if (segmentLess(s, node->segment)) {
            parent->left = newLeaf;
            parent->right = node;
        } else {
            parent->left = node;
            parent->right = newLeaf;
        }
        update(parent);
        return parent;
    }
    if (segmentLess(s, node->maxLeft)) {
        node->left = insert(node->left, s);
    } else {
        node->right = insert(node->right, s);
    }
    update(node);

    int balance = balanceFactor(node);
    if (balance > 1) {
        if (balanceFactor(node->left) >= 0) return rotateRight(node);
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }
    if (balance < -1) {
        if (balanceFactor(node->right) <= 0) return rotateLeft(node);
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }

    return node;
}

AVLNode* erase(AVLNode* node, Segment* s) {
    if (!node) return nullptr;

    if (node->isLeaf()) {
        if (node->segment == s) {
            delete node;
            return nullptr;
        }
        return node;
    }

    node->left = erase(node->left, s);
    node->right = erase(node->right, s);

    // Handle degenerate internal node cases
    if (!node->left && !node->right) {
        delete node;
        return nullptr;
    }
    if (!node->left) {
        AVLNode* temp = node->right;
        delete node;
        return temp;
    }
    if (!node->right) {
        AVLNode* temp = node->left;
        delete node;
        return temp;
    }

    update(node);

    // AVL balance
    int balance = balanceFactor(node);
    if (balance > 1) {
        if (balanceFactor(node->left) >= 0) return rotateRight(node);
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }
    if (balance < -1) {
        if (balanceFactor(node->right) <= 0) return rotateLeft(node);
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }

    return node;
}

bool isPointOnSegment(const Point& pt, const Segment* s) {
    double cross = (pt.x - s->p.x) * (s->q.y - s->p.y) - (pt.y - s->p.y) * (s->q.x - s->p.x);
    if (fabs(cross) > 1e-9) return false;

    double dot = (pt.x - s->p.x) * (s->q.x - s->p.x) + (pt.y - s->p.y) * (s->q.y - s->p.y);
    if (dot < -1e-9) return false;

    double sqLen = (s->q.x - s->p.x) * (s->q.x - s->p.x) + (s->q.y - s->p.y) * (s->q.y - s->p.y);
    return dot <= sqLen + 1e-9;
}

Segment* above(AVLNode* root, Segment* s, bool noid = false) {
    Segment* succ = nullptr;
    while (root) {
        if(noid){
            if (root->isLeaf()) {
                if (segmentLess(s, root->segment, noid)) succ = root->segment;
                break;
            }
            if (segmentLess(s, root->maxLeft, noid)) {
                succ = root->maxLeft;
                root = root->left;
            } else {
                root = root->right;
            }
        }
        else{
            if (root->isLeaf()) {
                if (segmentLess(s, root->segment)) succ = root->segment;
                break;
            }
            if (segmentLess(s, root->maxLeft)) {
                succ = root->maxLeft;
                root = root->left;
            } else {
                root = root->right;
            }
        }     
    }
    return succ;
}

Segment* below(AVLNode* root, Segment* s, bool noid = false) {
    Segment* pred = nullptr;
    while (root) {
        if(noid){
            if (root->isLeaf()) {
                if (segmentLess(root->segment, s, noid)) pred = root->segment;
                break;
            }
            if (segmentLess(root->maxLeft, s, noid)) {
                pred = root->maxLeft;
                root = root->right;
            } else {
                root = root->left;
            }
        }
        else
        {
            if (root->isLeaf()) {
                if (segmentLess(root->segment, s)) pred = root->segment;
                break;
            }
            if (segmentLess(root->maxLeft, s)) {
                pred = root->maxLeft;
                root = root->right;
            } else {
                root = root->left;
            }
        }
    }
    return pred;
}

void inorder(AVLNode* node) {
    if (!node) return;
    inorder(node->left);
    if (node->isLeaf()) cout << node->segment->id << " ";
    inorder(node->right);
}

bool getIntersection(const Segment* a, const Segment* b, Point& ip) {
    double A1 = a->q.y - a->p.y;
    double B1 = a->p.x - a->q.x;
    double C1 = A1 * a->p.x + B1 * a->p.y;
    double A2 = b->q.y - b->p.y;
    double B2 = b->p.x - b->q.x;
    double C2 = A2 * b->p.x + B2 * b->p.y;
    double det = A1 * B2 - A2 * B1;
    if (fabs(det) < 1e-9) return false;
    ip.x = (B2 * C1 - B1 * C2) / det;
    ip.y = (A1 * C2 - A2 * C1) / det;
    if (min(a->p.x, a->q.x) - 1e-9 <= ip.x && ip.x <= max(a->p.x, a->q.x) + 1e-9 &&
        min(a->p.y, a->q.y) - 1e-9 <= ip.y && ip.y <= max(a->p.y, a->q.y) + 1e-9 &&
        min(b->p.x, b->q.x) - 1e-9 <= ip.x && ip.x <= max(b->p.x, b->q.x) + 1e-9 &&
        min(b->p.y, b->q.y) - 1e-9 <= ip.y && ip.y <= max(b->p.y, b->q.y) + 1e-9) return true;
    return false;
}

void addEvent(priority_queue<Event>& events, Segment* a, Segment* b, const Point& p, unordered_set<Point, PointHash>& seen) {
    if (!a || !b || a==b) return;
    // cout << "Checking event: " << a->id << " " << b->id << endl;
    Point ip;
    if (getIntersection(a, b, ip) && (ip.y < sweepY - 1e-9 || (fabs(ip.y - sweepY) < 1e-9 && ip.x > p.x + 1e-9))) {
        if (seen.count(ip) == 0) {
            seen.insert(ip);
            // cout << "Intersection found between " << a->id << " and " << b->id << " at: " << ip.x << ", " << ip.y << endl;
            events.push({ip, 2, a, b});
        }
    }
}

void findIntersections(vector<Segment>& segments, vector<Point>& output) {
    priority_queue<Event> events;
    unordered_set<Point, PointHash> seen;
    for (auto& s : segments) {
        if (fabs(s.p.y - s.q.y) < 1e-9) {
            // Horizontal segment — nudge insert above by epsilon
            Point pin = s.p.x < s.q.x ? s.p : s.q;
            Point pout = s.p.x < s.q.x ? s.q : s.p;
    
            Point insert_pt = {pin.x, pin.y + 1e-6};  // slightly above
            Point remove_pt = {pout.x, pout.y - 1e-6}; // slightly below
    
            events.push({insert_pt, 0, &s, nullptr});
            events.push({remove_pt, 1, &s, nullptr});
        } else {
            events.push({s.p, 0, &s, nullptr});
            events.push({s.q, 1, &s, nullptr});
        }
    }

    AVLNode* root = nullptr;

    while (!events.empty()) {
        Event e = events.top(); events.pop();
        sweepY = e.p.y;

        if (e.type == 0) {
            // cout << "Type 0 event: " << e.p.x << " " << e.p.y << endl;
            // cout << "Before" << endl;
            // printTree(root); // Debugging line to check AVL tree structure
            // inorder(root); // Debugging line to check AVL tree structure
            // cout << endl;

            for (Segment* other : {above(root, e.s1, true), below(root, e.s1, true)}) {
                // cout << "Checking segment: " << e.s1->id << " with " << (other ? other->id : -1) << endl;
                if (other && isPointOnSegment(e.p, other) && other->id != e.s1->id) {
                    if (seen.count(e.p) == 0) {
                        seen.insert(e.p);
                        // cout << "Point-on-segment intersection between " << e.s1->id << " and " << other->id
                        //      << " at: " << e.p.x << ", " << e.p.y << endl;
                        output.push_back(e.p);
                    }
                }
            }

            sweepY -= 1e-6;
            root = insert(root, e.s1);
            Segment* ab = above(root, e.s1);
            Segment* bl = below(root, e.s1);
            addEvent(events, ab, e.s1, e.p, seen);
            addEvent(events, e.s1, bl, e.p, seen);

            // cout << "After" << endl;
            // printTree(root); // Debugging line to check AVL tree structure
            // inorder(root); // Debugging line to check AVL tree structure
            // cout << endl;
        } else if (e.type == 1) {
            // cout << "Type 1 event: " << e.p.x << " " << e.p.y << endl;	
            // cout << "Before" << endl;
            // printTree(root); // Debugging line to check AVL tree structure
            // inorder(root); // Debugging line to check AVL tree structure
            // cout << endl;

            for (Segment* other : {above(root, e.s1, true), below(root, e.s1, true)}) {
                if (other && isPointOnSegment(e.p, other) && other->id != e.s1->id) {
                    // cout << "Checking segment: " << e.s1->id << " with " << (other ? other->id : -1) << endl;
                    if (seen.count(e.p) == 0) {
                        seen.insert(e.p);
                        // cout << "Point-on-segment intersection between " << e.s1->id << " and " << other->id
                        //      << " at: " << e.p.x << ", " << e.p.y << endl;
                        output.push_back(e.p);
                    }
                }
            }
            
            Segment* ab = above(root, e.s1);
            Segment* bl = below(root, e.s1);
            root = erase(root, e.s1);
            addEvent(events, ab, bl, e.p, seen);

            // cout << "After" << endl;
            // printTree(root); // Debugging line to check AVL tree structure
            // inorder(root); // Debugging line to check AVL tree structure
            // cout << endl;

        } else if (e.type == 2) {
            // cout << "Type 2 event: " << e.p.x << " " << e.p.y << endl;
            // cout << "Before" << endl;
            // printTree(root); // Debugging line to check AVL tree structure
            // inorder(root); // Debugging line to check AVL tree structure
            // cout << endl;
            output.push_back(e.p);
            root = erase(root, e.s1);
            root = erase(root, e.s2);
            // cout << "After erasing" << endl;
            // printTree(root); // Debugging line to check AVL tree structure
            // cout << endl;
            sweepY -= 1e-6;
            root = insert(root, e.s2);
            root = insert(root, e.s1);

            Segment *top, *bottom;
            if (segmentLess(e.s1, e.s2)) {
                bottom = e.s1;
                top = e.s2;
            } else {
                bottom = e.s2;
                top = e.s1;
            }

            Segment* above_top = above(root, top);
            Segment* below_bottom = below(root, bottom);

            addEvent(events, above_top, top, e.p, seen);
            addEvent(events, bottom, below_bottom, e.p, seen);

            // cout << "After" << endl;
            // printTree(root); // Debugging line to check AVL tree structure
            // inorder(root); // Debugging line to check AVL tree structure
            // cout << endl;
        }
    }
}

int main() {
    int n;
    cin >> n;
    vector<Segment> segments;
    for (int i = 0; i < n; ++i) {
        Point p, q;
        cin >> p.x >> p.y >> q.x >> q.y;
        segments.emplace_back(p, q, i);
    }

    vector<Point> intersections;
    findIntersections(segments, intersections);

    for (auto& p : intersections) {
        cout << p.x << " " << p.y << endl;
    }
    return 0;
}