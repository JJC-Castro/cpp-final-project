// algo.h v3 - Greedy + SmartMatch (optimized)
#pragma once
#include "part.h"

class Algorithm {
protected:
    vector<P1*>& p1s; vector<P2*>& p2s;
    vector<P3*>& p3s; vector<P4*>& p4s;
    vector<Assembly*> bestSol;
    double bestAvgGap; int bestOK, totalRuns;
public:
    Algorithm(vector<P1*>& a, vector<P2*>& b, vector<P3*>& c, vector<P4*>& d)
        : p1s(a), p2s(b), p3s(c), p4s(d), bestAvgGap(0), bestOK(0), totalRuns(0) {}
    virtual ~Algorithm() { clearSol(); }
    void clearSol() { for(auto&a:bestSol)delete a; bestSol.clear(); }
    virtual void run() = 0;
    void printResult(const string& name) const {
        cout<<"\n===== "<<name<<" =====\n";
        cout<<"Qualified: "<<bestOK<<"/100 ("<<bestOK<<"%)\n";
        cout<<"Avg Gap: "<<fixed<<setprecision(4)<<bestAvgGap<<" mm\n";
        cout<<"\nTop 20:\n| ID | C1(mm) | C2(mm) | C3(mm) | C4(mm) |  C0(mm) |Status|\n";
        cout<<"|----|--------|--------|--------|--------|---------|------|\n";
        int n=min(20,(int)bestSol.size());
        for(int i=0;i<n;i++){bestSol[i]->print();cout<<"\n";}
        if(bestSol.size()>20)cout<<"... ("<<bestSol.size()<<" total)\n";
    }
    double getAvgGap()const{return bestAvgGap;}
    int getOK()const{return bestOK;}
    vector<Assembly*>& getSol(){return bestSol;}
};

// Greedy baseline
class Greedy:public Algorithm{
public:
    Greedy(vector<P1*>&a,vector<P2*>&b,vector<P3*>&c,vector<P4*>&d):Algorithm(a,b,c,d){}
    void run()override{
        sort(p1s.begin(),p1s.end(),cmpAscP1);
        sort(p2s.begin(),p2s.end(),cmpAscP2);
        sort(p3s.begin(),p3s.end(),cmpAscP3);
        sort(p4s.begin(),p4s.end(),cmpAscP4);
        clearSol();double sg=0;bestOK=0;
        for(int i=0;i<100;i++){
            bestSol.push_back(new Assembly(i+1,p1s[i],p2s[i],p3s[i],p4s[i]));
            sg+=bestSol[i]->getGap();if(bestSol[i]->isOK())bestOK++;
        }
        bestAvgGap=sg/100;
    }
};

// SmartMatch - greedy full search + local search + distribution compression
class SmartMatch:public Algorithm{
public:
    SmartMatch(vector<P1*>&a,vector<P2*>&b,vector<P3*>&c,vector<P4*>&d):Algorithm(a,b,c,d){}

    void runSingle(){
        // P1 descending, P2/P3/P4 ascending
        sort(p1s.begin(),p1s.end(),[](P1*a,P1*b){return a->getSize()>b->getSize();});
        sort(p2s.begin(),p2s.end(),cmpAscP2);
        sort(p3s.begin(),p3s.end(),cmpAscP3);
        sort(p4s.begin(),p4s.end(),cmpAscP4);
        clearSol();

        bool u2[100]={0},u3[100]={0},u4[100]={0};

        // Phase 1: Greedy assignment (largest P1 gets first pick)
        for(int pi=0;pi<100;pi++){
            double c1=p1s[pi]->getSize(); double bestSc=-999; int bj=0,bk=0,bl=0;
            for(int j=0;j<100;j++){if(u2[j])continue;
                double c2=p2s[j]->getSize();
                for(int k=0;k<100;k++){if(u3[k])continue;
                    double c3=p3s[k]->getSize();
                    for(int l=0;l<100;l++){if(u4[l])continue;
                        double c0=c1-c2-c3-p4s[l]->getSize();
                        double sc;
                        if(c0>=0&&c0<=0.45) sc=1000-abs(c0); // in range, prefer near 0
                        else if(c0<0) sc=500+c0; // negative: -0.01→499, -0.60→-100
                        else sc=500-(c0-0.45); // too large
                        if(sc>bestSc){bestSc=sc;bj=j;bk=k;bl=l;}
                    }
                }
            }
            u2[bj]=u3[bk]=u4[bl]=true;
            bestSol.push_back(new Assembly(pi+1,p1s[pi],p2s[bj],p3s[bk],p4s[bl]));
        }

        // Phase 2: Local search - maximize qualified count
        for(int iter=0;iter<500;iter++){
            vector<Assembly*>good,bad;
            for(auto&a:bestSol){if(a->isOK())good.push_back(a);else bad.push_back(a);}
            int curOK=(int)good.size();
            if(curOK>=100||bad.empty())break;
            bool improved=false;

            // Try single-part swaps (bad-good)
            for(auto&b:bad){for(auto&g:good){
                P2*ob2=b->getP2(),*og2=g->getP2();
                P3*ob3=b->getP3(),*og3=g->getP3();
                P4*ob4=b->getP4(),*og4=g->getP4();
                // Try P2 swap
                if(ob2!=og2){b->setP2(og2);g->setP2(ob2);
                    int n=0;for(auto&a:bestSol)if(a->isOK())n++;
                    if(n>curOK){improved=true;curOK=n;goto n1;}
                    b->setP2(ob2);g->setP2(og2);}
                // Try P3 swap
                if(ob3!=og3){b->setP3(og3);g->setP3(ob3);
                    int n=0;for(auto&a:bestSol)if(a->isOK())n++;
                    if(n>curOK){improved=true;curOK=n;goto n1;}
                    b->setP3(ob3);g->setP3(og3);}
                // Try P4 swap
                if(ob4!=og4){b->setP4(og4);g->setP4(ob4);
                    int n=0;for(auto&a:bestSol)if(a->isOK())n++;
                    if(n>curOK){improved=true;curOK=n;goto n1;}
                    b->setP4(ob4);g->setP4(og4);}
                // Dual swaps
                if(ob2!=og2&&ob3!=og3){b->setP2(og2);b->setP3(og3);g->setP2(ob2);g->setP3(ob3);
                    int n=0;for(auto&a:bestSol)if(a->isOK())n++;
                    if(n>curOK){improved=true;curOK=n;goto n1;}
                    b->setP2(ob2);b->setP3(ob3);g->setP2(og2);g->setP3(og3);}
                if(ob2!=og2&&ob4!=og4){b->setP2(og2);b->setP4(og4);g->setP2(ob2);g->setP4(ob4);
                    int n=0;for(auto&a:bestSol)if(a->isOK())n++;
                    if(n>curOK){improved=true;curOK=n;goto n1;}
                    b->setP2(ob2);b->setP4(ob4);g->setP2(og2);g->setP4(og4);}
                if(ob3!=og3&&ob4!=og4){b->setP3(og3);b->setP4(og4);g->setP3(ob3);g->setP4(ob4);
                    int n=0;for(auto&a:bestSol)if(a->isOK())n++;
                    if(n>curOK){improved=true;curOK=n;goto n1;}
                    b->setP3(ob3);b->setP4(ob4);g->setP3(og3);g->setP4(og4);}
            }}
            n1:;
            if(!improved){
                // Bad-bad swap
                for(size_t i=0;i<bad.size()&&!improved;i++){for(size_t j=i+1;j<bad.size()&&!improved;j++){
                    P2*oi=bad[i]->getP2(),*oj=bad[j]->getP2();
                    bad[i]->setP2(oj);bad[j]->setP2(oi);
                    int n=0;for(auto&a:bestSol)if(a->isOK())n++;
                    if(n>curOK){improved=true;curOK=n;}
                    else{bad[i]->setP2(oi);bad[j]->setP2(oj);}
                }}
                if(!improved)break;
            }
        }

        // Phase 3: Compress qualified C0 distribution (free up budget)
        // Swap between qualified to make C0 closer to 0 where possible
        for(int iter=0;iter<100;iter++){
            vector<Assembly*>good,bad;
            for(auto&a:bestSol){if(a->isOK())good.push_back(a);else bad.push_back(a);}
            int curOK=(int)good.size();
            bool improved=false;

            // Swap P2 between two qualified to reduce their C0 sum
            for(auto&a:good){for(auto&b:good){
                if(a==b)continue;
                double oldC0a=a->getGap(),oldC0b=b->getGap();
                P2*oa=a->getP2(),*ob=b->getP2();
                a->setP2(ob);b->setP2(oa);
                double newC0a=a->getGap(),newC0b=b->getGap();
                // Keep if sum of C0 decreased (more budget) and both still qualified
                if((newC0a+newC0b)<(oldC0a+oldC0b) && a->isOK() && b->isOK()){
                    improved=true; break;
                }else{
                    a->setP2(oa);b->setP2(ob);
                }
            }if(improved)break;}
            if(!improved)break;
        }

        double sg=0;bestOK=0;
        for(auto&a:bestSol){sg+=a->getGap();if(a->isOK())bestOK++;}
        bestAvgGap=sg/100;
    }

    void run()override{runSingle();}
};
