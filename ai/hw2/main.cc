#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <memory>
#include <map>

using namespace std;

namespace aihw2 {

class Homework {
public:
  static void Main(vector<string> args) {
    // Crappy CSV parsing
    if (args.size() != 2) {
      cout << "usage: " << args[0] << " survey-file" << endl;
      return;
    }
    auto input = ifstream(args[1]);
    vector<int> vec; // g,h,t,m,p,c

    int lines;
    for (lines = 0; !input.eof(); lines++) {
      int entry = 0;
      char l[100];
      input.getline(l, 100);
      for (auto i = 0; i < 6; i++) {
        char c = l[(i*2)];
        int j;
        if (c == '1') j = 1;
        if (c == '0') j = 0;
        entry |= j << (5-i);
      }
      vec.push_back(entry);
    }

    vector<long double> c(8);
    vector<int> cc(8);
    vector<long double> p(4);
    vector<int> cp(8);
    vector<int> all(6);
    vector<int> call(6);
    for (auto n : vec) {
      for (int i = 0; i < 6; i++) {
        all[i] += (n & (1 << i)) >> i;
        call[i]++;
        int j, val;
        // j is c's parents
        j = ((n & (1 << 5)) >> 5) << 2 | ((n & (1 << 2)) >> 2) << 1 | ((n & (1 << 1)) >> 1);
        val = n & 1;
        c[j] += val;
        cc[j]++;
        // j is p's parents
        j = ((n & (1 << 4)) >> 4) << 1 | ((n & (1 << 3)) >> 3);
        val = (n & (1 << 1)) >> 1;
        p[j] += val;
        cp[j]++;
      }
    }
    for (int i = 0; i < c.size(); i++) c[i]/=cc[i];
    for (int i = 0; i < p.size(); i++) p[i]/=cp[i];

    // Proportion true on each
    for (int i = 0; i < 6; i++) {
      all[i] /= call[i];
    }

    long double sum = 0;
    for (int m = 0; m < 2; m++) {
      for (int g = 0; g < 2; g++) {
        for (int t = 0; t < 2; t++) {
          for (int pn = 0; pn < 2; pn++) {
            int h = 1;
            long double pm = m ? all[2] : 1-all[2],
              pg = g ? all[5] : 1-all[5],
              pt = t ? all[3] : 1-all[3],
              pp = pn ? all[1] : 1-all[1],
              pc = c[g << 2 | m << 1 | pn],
              ppp = p[h << 1 | t];
            sum += pm*pg*pt*pp*pc*ppp;
          }
        }
      }
    }
    cout << "sum: " << sum << endl;
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
