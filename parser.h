#include <iostream>
#include <vector>
#include <map>
#include <set>
using namespace std;
extern map<string, vector<int>> toColor;
extern set<string> skipBlocks;
void processData(map<vector<int>, string>);
void writeData(map<vector<int>, string>);
void finishProcess();
void startFile(int a);