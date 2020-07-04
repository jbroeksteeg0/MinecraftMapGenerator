#include <iostream>
#include <stdio.h>
#include <vector>
#include <map>
#include <climits>
#include <fstream>
#include <algorithm>

#include "parser.h"
using namespace std;

int ubound = INT_MAX, dbound = INT_MIN, lbound = INT_MAX, rbound = INT_MIN;
int width, height;

map<pair<int,int>, string> finalMap;


void startFile(int bound) {
    ubound = (-bound * 512);
    dbound = (bound+1)*512 - 1;
    lbound = (-bound * 512);
    rbound = (bound+1)*512 - 1;

    width = (rbound-lbound+1);
    height = (dbound-ubound+1);

    ofstream file("out.ppm");
    file << "P3\n" << width << " " << height << "\n255\n";

    for (int i = 0; i < (int64_t)width*(int64_t)height; i++) {
        file << "0 0 0 ";
    }
    cout << width << "x" << height << "\n";
    cout << lbound << "x" << ubound << " -> " << rbound << "x" << dbound << "\n";
}
map<string, int> unknownFreq;

void writeData(map<vector<int>, string> blocks) {
    vector<pair<pair<int,int>, string>> toChange;

    for (pair<vector<int>, string> cb: blocks) {
        toChange.push_back({{cb.first[0], cb.first[1]}, cb.second});
    }

    sort(toChange.begin(), toChange.end(), [&](auto a, auto b) {
        return make_pair(a.first.second, a.first.first) < make_pair(b.first.second, b.first.first);
    });

    // for (auto cb: toChange)  {
    //     cout << cb.first.first << "," << cb.first.second << " " << cb.second << "\n";
    // }

    auto curr = toChange.begin();

    ofstream out("temp.ppm");
    ifstream in("out.ppm");


    {
        string temps;
        int tempi;
        getline(in, temps);
        in >> tempi >> tempi >> tempi;
    }
    out << "P3\n" << width << " " << height << "\n255\n";

    for (int y = ubound; y <= dbound; y++){
        for (int x = lbound; x <= rbound; x++){
            int a,b,c; in >> a >> b >> c;
            if (curr!=toChange.end() && curr->first == make_pair(x,y)) {
                if (toColor.find(curr->second) != toColor.end()) {
                    vector<int> co  = toColor[curr->second];
                    out << co[0] << " " << co[1] << " " << co[2] << " ";
                } else {
                    out << "255 0 0 ";
                    unknownFreq[curr->second]++;
                }
                curr++;
            } else {
                out << a << " " << b << " " << c << " ";
            }
        }
    }
    system("cp temp.ppm out.ppm");
}

void processData(map<vector<int>, string> blocks) {
    for (pair<vector<int>, string> pvis: blocks) {
        ubound = min(ubound, pvis.first[1]);
        dbound = max(dbound, pvis.first[1]);
        lbound = min(lbound, pvis.first[0]);
        rbound = max(rbound, pvis.first[0]);
        finalMap[{pvis.first[0], pvis.first[1]}] = pvis.second;
        // cout << "(" << pvis.first[0] << "," << pvis.first[1] << "): " << pvis.second << "\n";
    }
}


vector<vector<vector<int>>> picture;

void finishProcess() {
    cout << "Finishing..." << endl;
    cout << unknownFreq.size() << endl;
    vector<pair<int, string>> unknown;
    for (pair<string, int> psi: unknownFreq){
        unknown.push_back({psi.second,psi.first});
    }
    sort(unknown.begin(),unknown.end());
    for (pair<int,string> psi: unknown) {
        cout << psi.second << " " << psi.first << "\n";
    }
}
