
#include <array>
#include <vector>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <map>

using namespace std;

// I don't like the std library's naming scheme.
template <typename T>
using Vec = std::vector<T>;
template <typename K, typename V>
using Map = std::map<K, V>;
using Float = long double;

class Point {
public:
  Point(Vec<Float> p) : _point(p) {}

  // Returns distance between n dimensional Points,
  // where n is equivalent between both Points.
  long double Distance(const Point &other) const {
    long double p(0);
    for (size_t i = 0; i < _point.size(); i++) {
      p += pow(_point[i]-other._point[i], 2);
    }
    return sqrt(p);
  }

  // Pretty printing
  friend ostream &operator<<(ostream &os, const Point &p);
private:
  Vec<Float> _point;
};

// Pretty printing for Point
ostream &operator<<(ostream &os, const Point &p) {
  os << "[ ";
  for (auto l : p._point) {
    os << l << " ";
  }
  os << "]";
  return os;
}

// Label contains a point and labels it.
// It provides accessors to the point and label.
class Label {
public:
  // Constructed by providing a label and a point.
  Label(bool d, Point p) : _label(d), _point(p) {}

  // Accessors for point and label.
  Point point() const { return _point; }
  bool label() const { return _label; }

  // Pretty printing
  friend ostream &operator<<(ostream &os, const Label &l);
private:
  bool _label;
  Point _point;
};

// Pretty printing for Label
ostream &operator<<(ostream &os, const Label &l) {
  os << "[ label: " << l.label() << ", point: " << l.point() << " ]";
  return os;
}

// Pretty print vectors
template <typename T>
static ostream &operator<<(ostream &os, const Vec<T> &labels) {
  os << "[ ";
  for (auto l : labels) {
    os << l << " ";
  }
  os << "]";
  return os;
}

class KNearestNeighbor {
public:
  // Construct KNearestNeighbor with the testing dataset.
  KNearestNeighbor(size_t k, Vec<Label> labels) : _labels(labels), _k(k) {}

  // Returns the probability for the label 'true'.
  Float ComputeLabel(const Point &p) {

    // Search labels for nearest n points.
    sort(_labels.begin(), _labels.end(), [&](Label &a, Label &b){
      return p.Distance(a.point()) <= p.Distance(b.point());
    });

    // Sum over the members of the first k nearest neighbors.
    Float sum = 0;
    for (auto l : Vec<Label>(&_labels[0], &_labels[_k])) {
      sum += l.label();
    }

    // Return the probability that the label 'true' is correct,
    // e.g. 0.05 would mean the label 'false' has a 95% consensus.
    return sum/_k;
  }
private:
  Vec<Label> _labels;
  size_t _k;
};

// Read data from four provided data files into a vector of labels.
static Vec<Label> readDataSet(string lf, string xf, string yf, string zf) {
  Vec<Label> data;
  fstream fileL(lf, std::ios_base::in);
  fstream fileX(xf, std::ios_base::in);
  fstream fileY(yf, std::ios_base::in);
  fstream fileZ(zf, std::ios_base::in);

  Vec<bool> labels;
  Vec<Float> xs;
  Vec<Float> ys;
  Vec<Float> zs;

  bool l;
  while (fileL >> l) {
    labels.push_back(l);
  }

  Float i;
  while (fileX >> i) {
    xs.push_back(i);
  }
  while (fileY >> i) {
    ys.push_back(i);
  }
  while (fileZ >> i) {
    zs.push_back(i);
  }

  for (size_t j = 0; j < labels.size(); j++) {
    data.push_back(Label(labels[j], Point(Vec<Float>{xs[j], ys[j], zs[j]})));
  }
  return data;
}

int main() {
  // Read training dataset
  auto training = readDataSet(
    "./data/trainL.txt",
    "./data/trainX.txt",
    "./data/trainY.txt",
    "./data/trainZ.txt"
  );

  // Read testing dataset
  auto testing = readDataSet(
    "./data/testL.txt",
    "./data/testX.txt",
    "./data/testY.txt",
    "./data/testZ.txt"
  );

  // Test KNearestNeighbor with K [1, 20]
  Map<Float, Vec<size_t>> score;
  for (size_t i = 1; i <= 20; i++) {
    KNearestNeighbor n(i, training);
    Float accuracy = 0;
    for (auto l : testing) {
      accuracy += static_cast<bool>(round(n.ComputeLabel(l.point()))) == l.label();
    }
    score[accuracy/testing.size()].push_back(i);
  }
  for (auto s : score) {
    cout << get<0>(s) << ": " << get<1>(s) << endl;
  }
}
