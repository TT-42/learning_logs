#include <bits/stdc++.h>
using namespace std;

int main() {
  int N;
  cin >> N;

  vector<int> A(N);
  int avg = 0;
  for (int i = 0; i < N; i++){
    cin >> A.at(i);
    avg += A.at(i);
  }
  avg /= N;

  for (int i = 0; i < N; i++){
    cout << abs(A.at(i) - avg) << endl;
  }
}
