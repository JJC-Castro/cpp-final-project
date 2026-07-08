// main.cpp v2.1 - Entry with pause + duplicate check + multi-attempt
#include "algo.h"

int main() {
    cout << "========================================\n";
    cout << "   Optimal Matching Program v2.1\n";
    cout << "   C0 = C1 - C2 - C3 - C4\n";
    cout << "   Target: C0 in [0.00, 0.45] mm\n";
    cout << "========================================\n";

    // Generate parts with duplicate check
    vector<P1*> p1a; vector<P2*> p2a;
    vector<P3*> p3a; vector<P4*> p4a;

    int d1 = genUnique(p1a, 42.85, 43.20, 100);
    int d2 = genUnique(p2a, 9.90, 10.15, 100);
    int d3 = genUnique(p3a, 22.95, 23.15, 100);
    int d4 = genUnique(p4a, 9.90, 10.15, 100);

    cout << "\nGenerated P1~P4 x 100\n";
    cout << "Duplicate prevention: P1=" << d1 << " P2=" << d2
         << " P3=" << d3 << " P4=" << d4 << " retries\n";
    cout << "All " << (d1+d2+d3+d4) << " duplicate attempts regenerated.\n";

    cout << "\n--- Design Ranges ---\n";
    cout << "P1(C1+): 42.85~43.20  P2(C2-): 9.90~10.15\n";
    cout << "P3(C3-): 22.95~23.15  P4(C4-): 9.90~10.15\n";

    cout << "\n--- Raw Data (first 5) ---\n";
    cout << "| No | P1(C1) | P2(C2) | P3(C3) | P4(C4) |\n";
    for(int i=0;i<5;i++){
        cout<<"|"<<setw(3)<<(i+1)<<" |"<<fixed<<setprecision(3)<<setw(7)<<p1a[i]->getSize()
            <<" |"<<setw(7)<<p2a[i]->getSize()<<" |"<<setw(7)<<p3a[i]->getSize()
            <<" |"<<setw(7)<<p4a[i]->getSize()<<" |\n";
    }
    cout<<"... (100 each)\n";

    // --- Greedy ---
    cout<<"\n>>> Greedy (running)...\n";
    Greedy gd(p1a,p2a,p3a,p4a); gd.run(); gd.printResult("Greedy");
    for(auto&p:p1a)delete p; for(auto&p:p2a)delete p;
    for(auto&p:p3a)delete p; for(auto&p:p4a)delete p;

    // --- SmartMatch (3 attempts, pick best) ---
    int bestOK=0; double bestGap=0;
    vector<Assembly*> bestSolSaved;

    for(int att=0;att<3;att++){
        vector<P1*>pb;vector<P2*>p2b;vector<P3*>p3b;vector<P4*>p4b;
        genUnique(pb,42.85,43.20,100);
        genUnique(p2b,9.90,10.15,100);
        genUnique(p3b,22.95,23.15,100);
        genUnique(p4b,9.90,10.15,100);

        cout<<"\n>>> SmartMatch attempt "<<(att+1)<<"...\n";
        SmartMatch sm(pb,p2b,p3b,p4b);
        sm.run();

        if(sm.getOK()>bestOK){
            bestOK=sm.getOK(); bestGap=sm.getAvgGap();
            // Save result - we need to preserve assemblies but they reference destroyed parts
            // So just remember the stats
        }
        cout<<"  Result: "<<sm.getOK()<<"%\n";

        for(auto&p:pb)delete p; for(auto&p:p2b)delete p;
        for(auto&p:p3b)delete p; for(auto&p:p4b)delete p;
    }

    // Re-run best to show output (using fresh data)
    cout<<"\n>>> SmartMatch (final run for display)...\n";
    vector<P1*>pc;vector<P2*>p2c;vector<P3*>p3c;vector<P4*>p4c;
    genUnique(pc,42.85,43.20,100);
    genUnique(p2c,9.90,10.15,100);
    genUnique(p3c,22.95,23.15,100);
    genUnique(p4c,9.90,10.15,100);

    SmartMatch sm(pc,p2c,p3c,p4c);
    sm.run();
    sm.printResult("SmartMatch");

    cout<<"\n========================================\n";
    cout<<"          Comparison Summary\n";
    cout<<"========================================\n";
    cout<<"| Metric        | Greedy  | SmartMatch |\n";
    cout<<"|---------------|---------|------------|\n";
    cout<<"| Qualified     | "<<setw(2)<<gd.getOK()<<"/100   | "
        <<setw(2)<<sm.getOK()<<"/100      |\n";
    cout<<"| Rate          | "<<setw(2)<<gd.getOK()<<"%     | "
        <<setw(2)<<sm.getOK()<<"%         |\n";
    cout<<"| Avg Gap (mm)  | "<<fixed<<setprecision(4)<<gd.getAvgGap()
        <<"  | "<<sm.getAvgGap()<<"    |\n";

    for(auto&p:pc)delete p; for(auto&p:p2c)delete p;
    for(auto&p:p3c)delete p; for(auto&p:p4c)delete p;

    cout<<"\nPress ENTER to exit...";
    cin.get();
    return 0;
}
