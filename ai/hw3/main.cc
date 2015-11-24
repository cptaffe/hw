#include <iostream>
#include <string>
#include <array>
#include <map>

using namespace std;

namespace {

const size_t elems = 6;
using Model = array<array<double, elems>, elems>;
using Prior = array<double, elems>;

} // namespace

class Filterer {
public:
  static Filterer *Instance();
  Prior Predict(Prior p, Model t);
  Prior Refine(Prior p, Model t, int ei);
private:
  Filterer(){}
  Filterer(Filterer const&){}
  Filterer& operator=(Filterer const&) = default;
  static Filterer *_instance;
};

Filterer *Filterer::_instance = new Filterer();

Filterer *Filterer::Instance() {
  return _instance;
}

// Returns new prior
Prior Filterer::Predict(Prior p, Model t) {
  Prior np;
  for (int i = 0; i < t.size(); i++) {
    for (int j = 0; j < t[i].size(); j++) {
      np[j] += p[j] * t[i][j];
    }
  }
  return np;
}

Prior Filterer::Refine(Prior p, Model t, int ei) {
  auto row = 0.0;
  for (int i = 0; i < t[ei].size(); i++) {
    row += p[i] * t[ei][i];
  }
  Prior np;
  for (int i = 0; i < t.size(); i++) {
    np[i] = t[ei][i]*p[i] / row;
  }
  return np;
}

int main() {
  // std::arrays require extra braces.
  // Transition model
  Model tm {{
    {{ 0.3, 0.7, 0, 0, 0, 0 }},
    {{ 0, 0.3, 0.7, 0, 0, 0 }},
    {{ 0, 0, 0.3, 0.7, 0, 0 }},
    {{ 0, 0, 0, 0.3, 0.7, 0 }},
    {{ 0, 0, 0, 0, 0.3, 0.7 }},
    {{ 0.7, 0, 0, 0, 0, 0.3 }}
  }};
  // Observation model
  Model om {{
    {{ 0.9, 0.1, 0, 0, 0, 0 }},
    {{ 0.1, 0.8, 0.1, 0, 0, 0 }},
    {{ 0, 0.1, 0.8, 0.1, 0, 0 }},
    {{ 0, 0, 0.1, 0.8, 0.1, 0 }},
    {{ 0, 0, 0, 0.1, 0.8, 0.1 }},
    {{ 0, 0, 0, 0, 0.1, 0.9 }}
  }};

  const size_t racesn = 10;
  const size_t horsen = 5;

  // Observed Races
  array<array<int, horsen>, racesn> races {{
    {{ 2, 3, 3, 4, 3 }},
    {{ 3, 3, 5, 5, 4 }},
    {{ 3, 3, 1, 0, 5 }},
    {{ 4, 4, 1, 0, 0 }},
    {{ 4, 0, 2, 1, 1 }},
    {{ 5, 1, 3, 2, 1 }},
    {{ 0, 2, 4, 3, 1 }},
    {{ 1, 3, 5, 3, 2 }},
    {{ 1, 4, 0, 4, 2 }},
    {{ 2, 4, 0, 3, 2 }}
  }};

  // Horse priors
  array<Prior, horsen> horses;
  horses.fill([=]()->Prior {
    Prior p;
    p.fill(1.0/p.size());
    return p;
  }());

  // Horse probabilities
  array<double, horsen> horseProb;
  horseProb.fill(0);

  // Horse names
  array<string, horsen> horseNames {{
    "DinklageLives",
    "Strider",
    "Thundercat",
    "Sigaldry",
    "LaborDay"
  }};

  // Predict horse outcomes from Observed Races
  for (int i = 0; i < horses.size(); i++) {
    for (int j = 0; j < races.size(); j++) {
      // Update horses prior for this race observation
      // Predict->Refine
      horses[i] = Filterer::Instance()->Refine(Filterer::Instance()->Predict(horses[i], tm), om, races[i][j]);
    }
    // Create Horse probabilities
    for (int k = 0; k < horses[i].size(); k++) {
      horseProb[i] += k*horses[i][k];
    }
  }

  // Print predicted to std log,
  // so redirection of std out will only output
  // the required expected obsBuSF
  clog << "Predicted:" << endl;
  for (int i = 0; i < horses.size(); i++) {
    clog << "Horse: " << horseNames[i] << endl;
    for (int j = 0; j < horses[i].size(); j++) {
      clog << j << ": " << horses[i][j] << endl;
    }
  }

  // Map double->string, for sorting
  map<double, string> prediction;
  for (int i = 0; i < horseProb.size(); i++) {
    prediction[horseProb[i]] = horseNames[i];
  }
  // Print horse name : horse probabilities
  int i = horsen;
  for (auto h : prediction) {
    cout << i-- << " " << h.second << " " << h.first << endl;
  }
}
