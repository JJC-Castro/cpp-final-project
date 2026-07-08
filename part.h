// part.h - Part classes v2.1
#pragma once
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <chrono>
#include <set>
using namespace std;

class Part {
protected:
    int id; double size; string type;
public:
    Part(int i, double s, string t) : id(i), size(s), type(t) {}
    virtual ~Part() {}
    int getId() const { return id; }
    double getSize() const { return size; }
    string getType() const { return type; }
    virtual void print() const {
        cout << "| " << setw(2) << type << " | " << setw(3) << id
             << " | " << fixed << setprecision(3) << setw(7) << size << " |";
    }
};

class P1 : public Part { public:
    P1(int i, double s) : Part(i, s, "P1") {}
    void print() const override { cout<<"| P1 | "<<setw(3)<<id<<" | "<<setw(7)<<size<<" |  +  |"; }
};
class P2 : public Part { public:
    P2(int i, double s) : Part(i, s, "P2") {}
    void print() const override { cout<<"| P2 | "<<setw(3)<<id<<" | "<<setw(7)<<size<<" |  -  |"; }
};
class P3 : public Part { public:
    P3(int i, double s) : Part(i, s, "P3") {}
    void print() const override { cout<<"| P3 | "<<setw(3)<<id<<" | "<<setw(7)<<size<<" |  -  |"; }
};
class P4 : public Part { public:
    P4(int i, double s) : Part(i, s, "P4") {}
    void print() const override { cout<<"| P4 | "<<setw(3)<<id<<" | "<<setw(7)<<size<<" |  -  |"; }
};

class Assembly {
private:
    int id; P1* p1; P2* p2; P3* p3; P4* p4; double gapC0;
    void calcGap() { gapC0 = p1->getSize()-p2->getSize()-p3->getSize()-p4->getSize(); }
public:
    Assembly(int i, P1* a, P2* b, P3* c, P4* d) : id(i),p1(a),p2(b),p3(c),p4(d) { calcGap(); }
    int getId() const { return id; }
    double getGap() const { return gapC0; }
    bool isOK() const { return gapC0>=-1e-9 && gapC0<=0.45; }
    double getC1() const { return p1->getSize(); }
    P2* getP2() const { return p2; }
    P3* getP3() const { return p3; }
    P4* getP4() const { return p4; }
    void setP2(P2* p) { p2=p; calcGap(); }
    void setP3(P3* p) { p3=p; calcGap(); }
    void setP4(P4* p) { p4=p; calcGap(); }
    void print() const {
        cout<<"| "<<setw(4)<<id<<" | "<<setw(7)<<p1->getSize()
            <<" | "<<setw(7)<<p2->getSize()<<" | "<<setw(7)<<p3->getSize()
            <<" | "<<setw(7)<<p4->getSize()<<" | "<<setw(8)<<setprecision(4)<<fixed<<gapC0
            <<" | "<<(isOK()?"  OK  ":" BAD  ")<<" |";
    }
};

static mt19937 rng(chrono::system_clock::now().time_since_epoch().count());
static double randIn(double lo, double hi) {
    return uniform_real_distribution<double>(lo, hi)(rng);
}

// Generate N unique parts (no duplicate sizes)
template<class T>
static int genUnique(vector<T*>& v, double lo, double hi, int n) {
    v.clear();
    set<double> seen;
    int dupAttempts = 0;
    while ((int)v.size() < n) {
        double val = randIn(lo, hi);
        double r = round(val * 10000.0) / 10000.0;
        if (seen.find(r) != seen.end()) { dupAttempts++; continue; }
        seen.insert(r);
        v.push_back(new T((int)v.size()+1, val));
    }
    return dupAttempts;
}

static void genP1(vector<P1*>& v) { genUnique(v, 42.85, 43.20, 100); }
static void genP2(vector<P2*>& v) { genUnique(v, 9.90, 10.15, 100); }
static void genP3(vector<P3*>& v) { genUnique(v, 22.95, 23.15, 100); }
static void genP4(vector<P4*>& v) { genUnique(v, 9.90, 10.15, 100); }

static bool cmpAscP1(P1* a, P1* b) { return a->getSize() < b->getSize(); }
static bool cmpAscP2(P2* a, P2* b) { return a->getSize() < b->getSize(); }
static bool cmpAscP3(P3* a, P3* b) { return a->getSize() < b->getSize(); }
static bool cmpAscP4(P4* a, P4* b) { return a->getSize() < b->getSize(); }
