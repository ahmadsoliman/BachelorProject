#include <stdio.h>
#include <math.h>
#include <string.h>
#include <string>
#include <vector>
#include <queue>
#include <set>
#include <bitset>
#include <algorithm>


#define INF 1000000000
// Variables count limit
#define NV 100

using namespace std;

typedef vector<int> vi;
typedef vector<double> vd;
typedef bitset<NV> bs;
typedef vector<bs> vbs;

void printCase(int n, vbs A, vd f){
    printf("Case Details:\n");
    printf("--------------\n");
    printf("Number Of Variables n: %d\n\n", n);
    printf("Rules:\n");
    printf("-------\n");
    for(int i=0; i<A.size(); i++){
        for(int j=0; j<n; j++){
            if(A[i][j]) printf("%c ", 'A'+j);
        }
        printf(":: %.3lf\n", f[i]);
    }
}

double recur(int n, vbs A, vd f){
    //printCase(n, A, f);
    if(n==1) return f[0];

    n--;
    int val = f[n];

    vbs Aij;
    vd tmpf;
    for(int i=0; i<A.size(); i++){
        bs rule = A[i];
        rule[n]=0;
        if(rule.count()!=0 && find(Aij.begin(), Aij.end(), rule)==Aij.end()) {
            Aij.push_back(rule);
            tmpf.push_back(f[i]);
        }
    }

    // construct DAG
    vector<vi> graph(Aij.size(), vi());
    vector<vi> graphT(Aij.size(), vi());

    for(int i=0; i<Aij.size(); i++){
        for(int j=0; j<Aij.size(); j++){
            if(i==j || Aij[i].count()>=Aij[j].count()) continue;
            bool subset=1;
            for(int k=0; k<n && subset; k++){
                if(Aij[i][k] && !Aij[j][k])
                    subset = false;
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
            bs rule = Aij[i];
            double oldf = tmpf[i];

            vbs::iterator s = find(A.begin(), A.end(), rule);

            rule[n] = 1;
            vbs::iterator sUv = find(A.begin(), A.end(), rule);

            if(sUv == A.end()) fij[i] = oldf;
            else if(s == A.end()) fij[i] = f[sUv-A.begin()]/val;
            else fij[i] = min(oldf, f[sUv-A.begin()]/val);
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

        maxValue=max(maxValue, val*recur(n, Aij, fij));
        val--;
    }

    return maxValue;
}


int test0(){
    int n=2;
    vbs A;

    bs A1(string("01"));
    A.push_back(A1);

    bs A2(string("10"));
    A.push_back(A2);

    bs A3(string("11"));
    A.push_back(A3);

    vd f;
    f.push_back(3);
    f.push_back(4);
    f.push_back(7);

    return recur(n, A, f);
}

int test1(){
    int n=3;

    vbs A;

    bs A1(string("001"));
    A.push_back(A1);

    bs A2(string("010"));
    A.push_back(A2);

    bs A3(string("100"));
    A.push_back(A3);

    bs A4(string("011"));
    A.push_back(A4);

    bs A5(string("101"));
    A.push_back(A5);

    bs A6(string("110"));
    A.push_back(A6);

    vd f;

    f.push_back(4);
    f.push_back(6);
    f.push_back(3);
    f.push_back(8);
    f.push_back(8);
    f.push_back(6);

    return recur(n,A,f);
}

int test2(){
    int n=5;
    // v w z y x
    vbs A;
    bs A1(string("00001"));
    A.push_back(A1);

    bs A2(string("00010"));
    A.push_back(A2);

    bs A3(string("00100"));
    A.push_back(A3);

    bs A4(string("01000"));
    A.push_back(A4);

    bs A5(string("10000"));
    A.push_back(A5);

    bs A6(string("00110"));
    A.push_back(A6);

    bs A7(string("10100"));
    A.push_back(A7);

    bs A8(string("01100"));
    A.push_back(A8);

    bs A9(string("00011"));
    A.push_back(A9);

    bs A10(string("00101"));
    A.push_back(A10);

    bs A11(string("01011"));
    A.push_back(A11);

    bs A12(string("00111"));
    A.push_back(A12);

    vd f;
    f.push_back(3);
    f.push_back(2);
    f.push_back(3);
    f.push_back(2);
    f.push_back(3);

    f.push_back(3);
    f.push_back(7);
    f.push_back(4);
    f.push_back(4);
    f.push_back(6);
    f.push_back(6);
    f.push_back(12);

    return recur(n,A,f);
}

int test3(){
    int n=3;

    vbs A;

    bs A1(string("001"));
    A.push_back(A1);

    bs A2(string("010"));
    A.push_back(A2);

    bs A3(string("100"));
    A.push_back(A3);

    bs A4(string("110"));
    A.push_back(A4);

    vd f;

    f.push_back(2);
    f.push_back(2);
    f.push_back(2);
    f.push_back(1);

    return recur(n, A, f);
}

int test4(){
    int n=5;
    // v w z y x
    vbs A;
    bs A1(string("00001"));
    A.push_back(A1);

    bs A2(string("00010"));
    A.push_back(A2);

    bs A3(string("00100"));
    A.push_back(A3);

    bs A4(string("01000"));
    A.push_back(A4);

    bs A5(string("10000"));
    A.push_back(A5);

    bs A6(string("00110"));
    A.push_back(A6);

    bs A7(string("10100"));
    A.push_back(A7);

    bs A8(string("01100"));
    A.push_back(A8);

    bs A9(string("00011"));
    A.push_back(A9);

    bs A10(string("00101"));
    A.push_back(A10);

    bs A11(string("01011"));
    A.push_back(A11);

    bs A12(string("00111"));
    A.push_back(A12);

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

    return recur(n,A,f);
}

int test5(){
    int n=3;

    vbs A;

    bs A1(string("001"));
    A.push_back(A1);

    bs A2(string("010"));
    A.push_back(A2);

    bs A3(string("100"));
    A.push_back(A3);

    bs A4(string("011"));
    A.push_back(A4);

    bs A5(string("101"));
    A.push_back(A5);

    bs A6(string("110"));
    A.push_back(A6);

    vd f;

    f.push_back(2);
    f.push_back(4);
    f.push_back(2);
    f.push_back(6);
    f.push_back(3);
    f.push_back(5);

    return recur(n,A,f);
}

int test6(){
    int n=3;

    vbs A;

    bs A1(string("001"));
    A.push_back(A1);

    bs A2(string("010"));
    A.push_back(A2);

    bs A3(string("100"));
    A.push_back(A3);

    bs A4(string("011"));
    A.push_back(A4);

    bs A5(string("101"));
    A.push_back(A5);

    bs A6(string("110"));
    A.push_back(A6);

    vd f;

    f.push_back(2);
    f.push_back(4);
    f.push_back(2);
    f.push_back(5);
    f.push_back(3);
    f.push_back(6);

    return recur(n, A, f);
}

int main(){
    printf("Test 0: %d\n", test0());
    printf("Test 1: %d\n", test1());
    printf("Test 2: %d\n", test2());
    printf("Test 3: %d\n", test3());
    printf("Test 4: %d\n", test4());
    printf("Test 5: %d\n", test5());
    printf("Test 6: %d\n", test6());

    return 0;
}
