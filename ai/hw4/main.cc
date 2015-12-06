
#include <array>
#include <vector>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <map>

using namespace std;

template <typename T, int dimensions>
class Point {
public:
  Point(array<T, dimensions> a) : _arr(a) {}
  T Dimension(int dim) const {
    return _arr[dim];
  }
private:
  array<T, dimensions> _arr;
};

class Point3D {
public:
  Point3D(long double x, long double y, long double z)
    : _point(array<long double, 3>{{ x, y, z }}) {}
  long double X() const { return _point.Dimension(0); }
  long double Y() const { return _point.Dimension(1); }
  long double Z() const { return _point.Dimension(2); }
  long double Distance(const Point3D &other) const {
    return sqrt(pow(X()-other.X(), 2) + pow(Y()-other.Y(), 2) + pow(Z()-other.Z(), 2));
  }
  friend ostream &operator<<(ostream &os, const Point3D &p);
private:
  Point<long double, 3> _point;
};

ostream &operator<<(ostream &os, const Point3D &p) {
  os << "[ x: " << p.X() << ", y: " << p.Y() << ", z: " << p.Z() << " ]";
  return os;
}

class Label {
public:
  Label(bool d, Point3D p) : _label(d), _point(p) {}
  Point3D point() const { return _point; }
  bool label() const { return _label; }
  friend ostream &operator<<(ostream &os, const Label &l);
private:
  bool _label;
  Point3D _point;
};

ostream &operator<<(ostream &os, const Label &l) {
  os << "[ label: " << l._label << " ]";
  return os;
}

class KNearestNeighbor {
public:
  // Construct KNearestNeighbor with the testing dataset.
  KNearestNeighbor(int k, vector<Label> labels) : _labels(labels), _k(k) {}
  bool ComputeLabel(const Point3D &p) {
    // Search labels for nearest n points.
    sort(_labels.begin(), _labels.end(), [&](Label a, Label b){
      return p.Distance(a.point()) >= p.Distance(b.point());
    });
    long double sum(0);
    for (int i = 0; i < _k; i++) {
      sum += _labels[i].label();
    }
    return round(sum/_k);
  }
private:
  vector<Label> _labels;
  int _k;
};

vector<Label> readDataSet(string lf, string xf, string yf, string zf) {
  vector<Label> data;
  fstream trainL(lf, std::ios_base::in);
  fstream trainX(xf, std::ios_base::in);
  fstream trainY(yf, std::ios_base::in);
  fstream trainZ(zf, std::ios_base::in);

  vector<bool> labels;
  vector<long double> xs;
  vector<long double> ys;
  vector<long double> zs;

  bool l;
  while (trainL >> l) {
    labels.push_back(l);
  }

  long double i;
  while (trainX >> i) {
    xs.push_back(i);
  }
  while (trainY >> i) {
    ys.push_back(i);
  }
  while (trainZ >> i) {
    zs.push_back(i);
  }

  for (int j = 0; j < labels.size(); j++) {
    data.push_back(Label(labels[j], Point3D(xs[j], ys[j], zs[j])));
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
  map<long double, int> score;
  for (int i = 1; i <= 20; i++) {
    KNearestNeighbor n(i, training);
    long double accuracy = 0;
    for (auto l : testing) {
      accuracy += n.ComputeLabel(l.point()) == l.label();
    }
    score[accuracy/testing.size()] = i;
  }
  for (auto s : score) {
    cout << get<0>(s) << ": " << get<1>(s) << endl;
  }
}
