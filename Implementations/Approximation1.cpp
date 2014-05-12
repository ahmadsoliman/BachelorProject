#include <stdio.h>
#include <math.h>
#include <string.h>
#include <string>
#include <vector>
#include <queue>
#include <set>

#define INF 1000000000

using namespace std;

typedef vector<int> vi;
typedef vector<double> vd;
typedef vector<string> vs;
typedef vector<vs> vvs;

void printCase(vs V, vvs A, vd f){
    printf("Case Details:\n");
    printf("--------------\n");
    printf("Number Of Variables n: %d\n\n", V.size());
    printf("Rules:\n");
    printf("-------\n");
    for(int i=0; i<A.size(); i++){
        for(int j=0; j<A[i].size(); j++){
            printf("%s ", A[i][j].c_str());
        }
        printf(":: %.3lf\n", f[i]);
    }
}

double recur(vs V, vvs A, vd f){
    //printCase(V, A, f);
    if(V.size()==1) return f[0];

    string var = V[0];

    int val;
    for(int i=0; i<A.size(); i++){
        if(A[i].size()>1) continue;
        if(var==A[i][0]) {
            val = f[i];
            break;
        }
    }

    vs Vij = V;
    vvs Aij(A.size(), vs());

    //step 1
    Vij.erase(Vij.begin());
    //step 2
    for(int i=0; i<A.size(); i++){
        for(int j=0; j<A[i].size(); j++){
            if(var!=A[i][j]) Aij[i].push_back(A[i][j]);
        }
    }

    // remove empty rules
    for(int i=0; i<Aij.size(); i++){
        if(Aij[i].size()==0){
            Aij.erase(Aij.begin()+i);
            i--;
        }
    }

    // remove repeated rules
    set<vs> s(Aij.begin(), Aij.end());
    Aij.clear();
    Aij = vvs(s.begin(), s.end());

    // construct DAG
    vector<vi> graph(Aij.size(), vi());
    vector<vi> graphT(Aij.size(), vi());

    for(int i=0; i<Aij.size(); i++){
        for(int j=0; j<Aij.size(); j++){
            if(i==j||Aij[i].size()>=Aij[j].size()) continue;
            bool subset=1;
            for(int k=0; k<Aij[i].size()&&subset; k++){
                bool found=0;
                for(int l=0; l<Aij[j].size(); l++){
                    if(Aij[i][k]==Aij[j][l]){
                        found=1;
                        break;
                    }
                }
                subset&=found;
            }
            if(subset){
                graph[i].push_back(j);
                graphT[j].push_back(i);
            }
        }
    }

    // Try for all values of the number of instances of the cur variable
    double maxValue=0;
    while(val>0){
        vd fij(Aij.size(), INF);

        for(int i=0; i<Aij.size(); i++){
            vs S = Aij[i];
            vs Sv = S;
            Sv.push_back(var);
            bool foundS=false, foundSv=false;
            double foundSval, foundSvVal;

            for(int j=0; j<A.size(); j++){
                if(S.size()==A[j].size()){
                    bool sameSet=1;
                    for(int k=0; k<S.size()&&sameSet; k++){
                        bool found=0;
                        for(int l=0; l<S.size(); l++){
                            if(S[k]==A[j][l]) {
                                found = 1;
                                break;
                            }
                        }
                        sameSet&=found;
                    }
                    if(sameSet) foundS=true, foundSval=f[j];
                }
                if(Sv.size()==A[j].size()){
                    bool sameSet=1;
                    for(int k=0; k<Sv.size()&&sameSet; k++){
                        bool found=0;
                        for(int l=0; l<Sv.size(); l++){
                            if(Sv[k]==A[j][l]) {
                                found = 1;
                                break;
                            }
                        }
                        sameSet&=found;
                    }
                    if(sameSet) foundSv=true, foundSvVal=f[j]/val;
                }
            }
            if(!foundSv) fij[i]=foundSval;
            else if(!foundS) fij[i]=foundSvVal;
            else fij[i]=min(foundSval,foundSvVal);
        }

        queue<int> q;
        for(int i=0; i<Aij.size(); i++){
            if(graph[i].size()==0) q.push(i);
        }
        while(!q.empty()){
            int node = q.front(); q.pop();
            for(int i=0; i<graph[node].size(); i++){
                fij[node]=min(fij[node], fij[graph[node][i]]);
            }
            for(int i=0; i<graphT[node].size(); i++){
                q.push(graphT[node][i]);
            }
        }

        maxValue=max(maxValue, val*recur(Vij, Aij, fij));
        val--;
    }

    return maxValue;
}

int test0(){
    vs V;
    V.push_back("X");
    V.push_back("Y");
    V.push_back("Z");

    vvs A;
    vs tmpv;
    tmpv.push_back("X");
    A.push_back(tmpv);

    tmpv.clear();
    tmpv.push_back("Y");
    A.push_back(tmpv);

    tmpv.clear();
    tmpv.push_back("Z");
    A.push_back(tmpv);

    tmpv.clear();
    tmpv.push_back("X");
    tmpv.push_back("Y");
    A.push_back(tmpv);

    tmpv.clear();
    tmpv.push_back("X");
    tmpv.push_back("Z");
    A.push_back(tmpv);

    tmpv.clear();
    tmpv.push_back("Y");
    tmpv.push_back("Z");
    A.push_back(tmpv);

    vd f;
    f.push_back(4);
    f.push_back(6);
    f.push_back(3);
    f.push_back(8);
    f.push_back(8);
    f.push_back(6);

    return recur(V,A,f);
}

int test1(){
    vs V;
    V.push_back("X");
    V.push_back("Y");
    V.push_back("Z");

    vvs A;
    vs tmpv;
    tmpv.push_back("X");
    A.push_back(tmpv);

    tmpv.clear();
    tmpv.push_back("Y");
    A.push_back(tmpv);

    tmpv.clear();
    tmpv.push_back("Z");
    A.push_back(tmpv);

    tmpv.clear();
    tmpv.push_back("X");
    tmpv.push_back("Y");
    A.push_back(tmpv);

    tmpv.clear();
    tmpv.push_back("X");
    tmpv.push_back("Z");
    A.push_back(tmpv);

    tmpv.clear();
    tmpv.push_back("Y");
    tmpv.push_back("Z");
    A.push_back(tmpv);

    vd f;
    f.push_back(2);
    f.push_back(4);
    f.push_back(2);
    f.push_back(6);
    f.push_back(3);
    f.push_back(5);

    return recur(V,A,f);
}

int test2(){
    vs V;
    V.push_back("X");
    V.push_back("Y");
    V.push_back("Z");
    V.push_back("W");
    V.push_back("Q");

    vvs A;
    vs tmpv;
    tmpv.push_back("X");
    A.push_back(tmpv);

    tmpv.clear();
    tmpv.push_back("Y");
    A.push_back(tmpv);

    tmpv.clear();
    tmpv.push_back("Z");
    A.push_back(tmpv);

    tmpv.clear();
    tmpv.push_back("W");
    A.push_back(tmpv);

    tmpv.clear();
    tmpv.push_back("Q");
    A.push_back(tmpv);

    // q w z y x

    tmpv.clear();
    tmpv.push_back("Y");
    tmpv.push_back("Z");
    A.push_back(tmpv);

    tmpv.clear();
    tmpv.push_back("Z");
    tmpv.push_back("Q");
    A.push_back(tmpv);

    tmpv.clear();
    tmpv.push_back("Z");
    tmpv.push_back("W");
    A.push_back(tmpv);

    tmpv.clear();
    tmpv.push_back("X");
    tmpv.push_back("Y");
    A.push_back(tmpv);

    tmpv.clear();
    tmpv.push_back("X");
    tmpv.push_back("Z");
    A.push_back(tmpv);

    tmpv.clear();
    tmpv.push_back("X");
    tmpv.push_back("Y");
    tmpv.push_back("W");
    A.push_back(tmpv);

    tmpv.clear();
    tmpv.push_back("X");
    tmpv.push_back("Y");
    tmpv.push_back("Z");
    A.push_back(tmpv);

    vd f;
    f.push_back(3);
    f.push_back(2);
    f.push_back(2);
    f.push_back(2);
    f.push_back(4);

    f.push_back(3);
    f.push_back(7);
    f.push_back(4);
    f.push_back(4);
    f.push_back(6);
    f.push_back(6);
    f.push_back(12);

    return recur(V,A,f);
}

int test3(){
    vs V;
    V.push_back("Z");
    V.push_back("Y");
    V.push_back("X");


    vvs A;
    vs tmpv;
    tmpv.push_back("X");
    A.push_back(tmpv);

    tmpv.clear();
    tmpv.push_back("Y");
    A.push_back(tmpv);

    tmpv.clear();
    tmpv.push_back("Z");
    A.push_back(tmpv);

    tmpv.clear();
    tmpv.push_back("X");
    tmpv.push_back("Y");
    A.push_back(tmpv);

    tmpv.clear();
    tmpv.push_back("X");
    tmpv.push_back("Z");
    A.push_back(tmpv);

    tmpv.clear();
    tmpv.push_back("Y");
    tmpv.push_back("Z");
    A.push_back(tmpv);

    vd f;
    f.push_back(2);
    f.push_back(4);
    f.push_back(2);
    f.push_back(6);
    f.push_back(3);
    f.push_back(5);

    return recur(V,A,f);
}

int main(){
    //printf("Test 0: %d\n", test0());
    printf("Test 1: %d\n", test1());
    //printf("Test 2: %d\n", test2());
    printf("Test 2: %d\n", test3());

}
