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
typedef vector<vi> vvi;
typedef vector<double> vd;
typedef vector<vd> vvd;
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

struct node{
    int var;
    vector<node> chlds;
    node(int var1, vector<node> chlds1){
        var=var1;
        chlds=chlds1;
    }
};

void printTree(node n){
    printf("%c ", n.var+'A');
    for(int i=0; i<n.chlds.size(); i++) printTree(n.chlds[i]);
}

int countLeaves(node n, int var){
    if(n.chlds.size()==0) {
        if(var==0) return 1;
        return 0;
    }
    int sum=0;
    for(int i=0; i<n.chlds.size(); i++) sum+=countLeaves(n.chlds[i], var-1);
    return sum;
}

void add(node *n, int var, int last, vvd values){
    if(var+1==n->var){
        n->chlds.push_back(node(var, vector<node>()));
        return;
    }
    double val = values[var][(n->var)-1]-floor(values[var][(n->var)-1]);
    if(!last) val=1;
    if(val<1e-9){
        val=1;
    }

    if(val>1e-9){
        val = round(val*n->chlds.size());
        for(int i=0; i<val; i++) add(&n->chlds[i], var, last, values);
    }
}

int calc(int n, vvd values){
    node root(n, vector<node>());
    for(int i=0; i<values[n-1][n-1]; i++) add(&root, n-1, 0, values);
    for(int i=n-2; i>=0; i--){
        double val = values[i][i];
        while(val>1 || fabs(val-1)<1e-9){
            val--;
            add(&root, i, !(val>1 || fabs(val-1)<1e-9), values);
        }
    }
 //   printTree(root);
    return countLeaves(root, n);
}

double recur(int n, vbs A, vd f, vvd values){
    //printCase(n, A, f);
    if(n==1) {
        values[0][0] = f[0];
        for(int i=0; i<values.size(); i++) values[0][i]=min(values[0][i], f[0]);

        for(int i=0; i<values.size(); i++){
            for(int j=i+1; j<values.size(); j++){
                if(values[i][j]<1e-9) values[i][j]=values[i][i];
            }
        }

        printf("KASE _ _ _ __ _ _ _ _ \n");
        for(int i=0; i<values.size(); i++){
            printf("%c ::", 'A'+i);
            for(int j=0; j<values[i].size(); j++){
                printf(" %.3lf", values[i][j]);
            }
            printf("\n");
        }
//
//        double res=1;
//        for(int i=0; i<values.size(); i++){
//            vvd table(values[i].size());
//            int minLen=10;
//            for(int j=0; j<values[i].size(); j++){
//                double val = values[i][j];
//                while(val>1){
//                    table[j].push_back(1);
//                    val-=1;
//                }
//                table[j].push_back(val);
//                minLen = min(minLen, (int)table[j].size());
//            }
//
//            double val=0;
//            for(int j=0; j<minLen; j++){
//                double val2=1;
//                for(int k=0; k<table.size(); k++){
//                    //if(table[k].size()==minLen)
//                        val2*=table[k][j];
//                }
//                val += val2;
//            }
//            res *= val;
//        }

//        printf("RES1:: %.3lf\n", res);

        double res = calc(values.size(), values);

        printf("RES2:: %.3lf\n", res);

        return res;
    }

    n--;
    double val = f[n];

    // Try for all values of the number of instances of the cur variable
    double maxValue=0;
    while(val>1e-9){
        vvd tmpVals = values;
        tmpVals[n][n]=val;
        for(int i=n; i<values.size(); i++) tmpVals[n][i]=min(tmpVals[n][i], val);


        vbs newA;
        vd newf;
        for(int i=0; i<A.size(); i++){
            bs rule = A[i];
            double val2 = f[i];
            if(rule[n]){
                val2/=val;
            }
            rule[n]=0;
            if(rule.count()!=0) {
                vbs::iterator it = find(newA.begin(), newA.end(), rule);
                if(it==newA.end()) {
                    newA.push_back(rule);
                    newf.push_back(ceil(val2));
                }else{
                    newf[it-newA.begin()] = min(newf[it-newA.begin()], ceil(val2));
                    if(rule.count()==1){
                        int ind;
                        for(ind=0; ind<n && !rule[ind]; ind++);
                        tmpVals[ind][n]=min(val2, newf[it-newA.begin()]);
                    }
                }
            }
        }

        maxValue=max(maxValue, recur(n, newA, newf, tmpVals));
        val = floor(val)-1;
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

    return recur(n, A, f, vvd(n, vd(n)));
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

    return recur(n, A, f, vvd(n, vd(n)));
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

    return recur(n, A, f, vvd(n, vd(n)));
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

    bs A4(string("011"));
    A.push_back(A4);

    bs A5(string("101"));
    A.push_back(A5);

    bs A6(string("110"));
    A.push_back(A6);

    vd f;

    f.push_back(3);
    f.push_back(3);
    f.push_back(3);
    f.push_back(8);
    f.push_back(8);
    f.push_back(6);

    return recur(n, A, f, vvd(n, vd(n)));
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

    return recur(n, A, f, vvd(n, vd(n)));
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

    return recur(n, A, f, vvd(n, vd(n)));
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

    return recur(n, A, f, vvd(n, vd(n)));
}

int test7(){
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

    return recur(n, A, f, vvd(n, vd(n)));
}

int main(){
//    printf("Test 0: %d\n", test0());
//    printf("Test 1: %d\n", test1());
//    printf("Test 3: %d\n", test3());
//    printf("Test 4: %d\n", test4());
//    printf("Test 5: %d\n", test5());
//    printf("Test 6: %d\n", test6());
    printf("Test 7: %d\n", test7());

    return 0;
}
