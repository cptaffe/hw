#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <memory>

using namespace std;

namespace aihw2 {

// Inverse graph,
// Directed connections are stored as references
// to parents instead of children.
class Node {
public:
  Node(string n) : _name(n) {}
  void addParent(shared_ptr<Node> p) {
    parents.push_back(p);
  }
  string name() const { return _name; }
  long double value() const { return _value; }
  void set_value(long double val) { _value = val; }
private:
  string _name;
  long double _value = 0;
  vector<shared_ptr<Node>> parents;
};

ostream& operator<<(ostream& os, const Node& node) {
  os << "node(" << node.name() << "): " << node.value();
  return os;
}

class Network {
public:
  void addNode(shared_ptr<Node> n) {
    nodes.push_back(n);
  }
private:
  vector<shared_ptr<Node>> nodes;
};

class Homework {
public:
  static void Main(vector<string> args) {
    // Crappy CSV parsing
    if (args.size() != 2) {
      cout << "usage: " << args[0] << " survey-file" << endl;
      return;
    }
    auto input = ifstream(args[1]);
    auto gn = shared_ptr<Node>(new Node("G")),
      hn = shared_ptr<Node>(new Node("H")),
      tn = shared_ptr<Node>(new Node("T")),
      mn = shared_ptr<Node>(new Node("M")),
      pn = shared_ptr<Node>(new Node("P")),
      cn = shared_ptr<Node>(new Node("C"));
    cn->addParent(gn);
    cn->addParent(pn);
    cn->addParent(mn);
    pn->addParent(hn);
    pn->addParent(tn);
    vector<shared_ptr<Node>> vec = { gn, hn, tn, mn, pn, cn };

    int lines;
    for (lines = 0; !input.eof(); lines++) {
      for (auto i = 0; i < vec.size(); i++) {
        int j;
        input >> j;
        vec[i]->set_value(vec[i]->value() + j);
      }
    }
    for (n : vec) {
      n->set_value(n->value()/lines);
    }
    cout << "Got the following values" << endl;
    for (n : vec) cout << *n << endl;

    // Create Network.
    auto net = new Network();
    for (n : vec) net->addNode(n);
  }
};

} // namespace aihw2

// Program entry point
int main(int argc, const char *argv[]) {
  vector<string> vec;
  for (auto i = 0; i < argc; i++) {
    vec.push_back(std::string(argv[i]));
  }
  aihw2::Homework::Main(vec);
}
