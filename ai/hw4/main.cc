
#include <array>
#include <vector>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <map>

using namespace std;

class Point {
public:
  Point(vector<long double> p)
    : _point(p) {}
  long double Distance(const Point &other) const {
    long double p(0);
    for (size_t i = 0; i < _point.size(); i++) {
      p += pow(_point[i]-other._point[i], 2);
    }
    return sqrt(p);
  }
  friend ostream &operator<<(ostream &os, const Point &p);
private:
  vector<long double> _point;
};

ostream &operator<<(ostream &os, const Point &p) {
  os << "[ ";
  for (auto l : p._point) {
    os << l << " ";
  }
  os << "]";
  return os;
}

class Label {
public:
  Label(bool d, Point p) : _label(d), _point(p) {}
  Point point() const { return _point; }
  bool label() const { return _label; }
  friend ostream &operator<<(ostream &os, const Label &l);
private:
  bool _label;
  Point _point;
};

ostream &operator<<(ostream &os, const Label &l) {
  os << "[ label: " << l.label() << ", point: " << l.point() << " ]";
  return os;
}

static ostream &operator<<(ostream &os, const vector<Label> &labels) {
  for (auto l : labels) {
    os << l << " ";
  }
  return os;
}

static ostream &operator<<(ostream &os, const vector<size_t> &labels) {
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
  KNearestNeighbor(size_t k, vector<Label> labels) : _labels(labels), _k(k) {}
  long double ComputeLabel(const Point &p) {
    // Search labels for nearest n points.
    sort(_labels.begin(), _labels.end(), [&](Label a, Label b){
      return p.Distance(a.point()) < p.Distance(b.point());
    });
    long double sum = 0;
    vector<Label> fk(&_labels[0], &_labels[_k]);
    for (auto l : fk) {
      sum += l.label();
    }
    cout << sum/_k << ": " << fk << endl;
    return sum/_k;
  }
private:
  vector<Label> _labels;
  size_t _k;
};

static vector<Label> readDataSet(string lf, string xf, string yf, string zf) {
  vector<Label> data;
  fstream fileL(lf, std::ios_base::in);
  fstream fileX(xf, std::ios_base::in);
  fstream fileY(yf, std::ios_base::in);
  fstream fileZ(zf, std::ios_base::in);

  vector<bool> labels;
  vector<long double> xs;
  vector<long double> ys;
  vector<long double> zs;

  bool l;
  while (fileL >> l) {
    labels.push_back(l);
  }

  long double i;
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
    data.push_back(Label(labels[j], Point(vector<long double>{xs[j], ys[j], zs[j]})));
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
  map<long double, vector<size_t>> score;
  for (size_t i = 1; i <= 20; i++) {
    KNearestNeighbor n(i, training);
    long double accuracy = 0;
    for (auto l : testing) {
      accuracy += static_cast<int>(round(n.ComputeLabel(l.point()))) == l.label();
    }
    score[accuracy/testing.size()].push_back(i);
  }
  for (auto s : score) {
    cout << get<0>(s) << ": " << get<1>(s) << endl;
  }
}
