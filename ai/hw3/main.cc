#include <iostream>
#include <string>
#include <array>
#include <vector>
#include <map>

using namespace std;

namespace {
const size_t elems = 6;
using matrix = array<array<double, elems>, elems>;
using vec = array<double, elems>;
} // namespace

template <size_t elems>
class Prior {
public:
  Prior(vec prior) : _prior(prior) {}

  void Predict(matrix transition) {
    vec np;
    for (int i = 0; i < transition.size(); i++) {
      for (int j = 0; j < transition[i].size(); j++) {
        np[j] += _prior[j] * transition[i][j];
      }
    }
    _prior = np;
  }

  void Refine(vec observation) {
    auto row = 0.0;
    for (int i = 0; i < observation.size(); i++) {
      row += _prior[i] * observation[i];
    }
    vec np;
    for (int i = 0; i < observation.size(); i++) {
      np[i] = observation[i]*_prior[i] / row;
    }
    _prior = np;
  }

  double Prob() {
    double row = 0;
    for (int i = 0; i < _prior.size(); i++) {
      row += i*_prior[i];
    }
    return row;
  }
private:
  vec _prior;
};

template <size_t elems>
class Model {
public:
  Model(matrix transition, matrix observation)
    : _transition(transition), _observation(observation) {}
  void Update(Prior<elems> *p, int performance) {
    p->Predict(_transition);
    p->Refine(_observation[performance]);
  }
private:
  matrix _transition;
  matrix _observation;
};

template<size_t elems>
class Horse {
public:
  Horse(string name, Prior<elems> prior, Model<elems> *model)
    : _name(name), _prior(prior), _model(model) {}
  void Update(int performance) {
    _model->Update(&_prior, performance);
  }
  double Prob() {
    return _prior.Prob();
  }
  string Name() const { return _name; }
private:
  string _name;
  Prior<elems> _prior;
  Model<elems> *_model;
};

class Homework {
public:
  Homework()
    : _model({{
      {{ 0.3, 0.7, 0, 0, 0, 0 }},
      {{ 0, 0.3, 0.7, 0, 0, 0 }},
      {{ 0, 0, 0.3, 0.7, 0, 0 }},
      {{ 0, 0, 0, 0.3, 0.7, 0 }},
      {{ 0, 0, 0, 0, 0.3, 0.7 }},
      {{ 0.7, 0, 0, 0, 0, 0.3 }}
    }},
    {{
      {{ 0.9, 0.1, 0, 0, 0, 0 }},
      {{ 0.1, 0.8, 0.1, 0, 0, 0 }},
      {{ 0, 0.1, 0.8, 0.1, 0, 0 }},
      {{ 0, 0, 0.1, 0.8, 0.1, 0 }},
      {{ 0, 0, 0, 0.1, 0.8, 0.1 }},
      {{ 0, 0, 0, 0, 0.1, 0.9 }}
    }}), _horses([=]()->vector<Horse<elems>>{
      vector<Horse<elems>> horses;
      vector<string> names {
        "DinklageLives",
        "Strider",
        "Thundercat",
        "Sigaldry",
        "LaborDay"
      };
      for (auto name : names) {
        horses.push_back(Horse<elems>(name, [=]()->array<double, elems>{
          array<double, elems> p;
          p.fill(1.0/p.size());
          return p;
        }(), &_model));
      }
      return horses;
    }()) {}

  vector<Horse<elems>> Do() {
    // Observed Races
    vector<vector<int>> races {
      { 2, 3, 3, 4, 3 },
      { 3, 3, 5, 5, 4 },
      { 3, 3, 1, 0, 5 },
      { 4, 4, 1, 0, 0 },
      { 4, 0, 2, 1, 1 },
      { 5, 1, 3, 2, 1 },
      { 0, 2, 4, 3, 1 },
      { 1, 3, 5, 3, 2 },
      { 1, 4, 0, 4, 2 },
      { 2, 4, 0, 3, 2 }
    };
    for (int i = 0; i < races.size(); i++) {
      for (int j = 0; j < races[i].size(); j++) {
        _horses[j].Update(races[i][j]);
      }
    }
    return _horses;
  }
private:
  Model<elems> _model;
  vector<Horse<elems>> _horses;
};

int main() {
  for (auto h : Homework().Do()) {
    cout << h.Name() << ": " << h.Prob() << endl;
  }
}
