#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include <queue>
#include <set>
#include <bitset>
#include <map>
#include <algorithm>

using namespace std;

//enable debugging
#define debug 0

// Variables count limit
#define NV 100

typedef pair<int,int> ii;
typedef pair<double,int> di;
typedef vector<int> vi;
typedef bitset<NV> bs;
typedef vector<bs> vbs;

int vCnt;
map<vi, int> vti; // maps vectors which represent a string to an int
map<int, vi> itv;

vi variableInstances; // the number of instances of each variable

void enumerateAllStrings(int n, vi curV){
    if(curV.size()==n){
        vti[curV]=vCnt;
        itv[vCnt++]=curV;
        return;
    }
    int curI = curV.size();
    for(int i=0; i<variableInstances[curI]; i++){
        curV.push_back(i);
        enumerateAllStrings(n, curV);
        curV.pop_back();
    }
}

bool checkConstraints(vi remStrings, int n, vbs A, vi f){
    sort(remStrings.begin(), remStrings.end());
    vector<vi> strings;
    for(int i=0; i<itv.size(); i++){
        vi cur = itv[i];
        if(binary_search(remStrings.begin(), remStrings.end(), i)){
            continue;
        }
        strings.push_back(cur);
    }

    for(int i=0; i<A.size(); i++){
        if(A[i].size()==1) continue;
        set<vi> s;
        for(int j=0; j<strings.size(); j++){
            vi curV;
            for(int k=0; k<n; k++){
                if(!A[i][k]) continue;
                curV.push_back(strings[j][k]);
            }
            s.insert(curV);
            if(s.size()>f[i]) return false;
        }
    }
    return true;
}

void enumerateStrings(int var, int n, vi curV, bs rule, vector<vi> &strings){
    if(var==n){
        strings.push_back(curV);
        return;
    }
    if(!rule[var]){
        enumerateStrings(var+1, n, curV, rule, strings);
        return;
    }
    for(int i=0; i<variableInstances[var]; i++){
        curV.push_back(i);
        enumerateStrings(var+1, n, curV, rule, strings);
        curV.pop_back();
    }
}

void removeString(int var, vi curString, bs rule, int curRuleVar, vi ruleVars, int n, set<int> &remStrings){
    if(var==n) {
        remStrings.insert(vti[curString]);

        if(debug){
            printf("%s::%d:: ", rule.to_string().substr(rule.size()-n).c_str(), remStrings.size());
            for(int i=0; i<n; i++){
                printf("V%d: %d, ", i, curString[i]+1);
            }
            printf("\n");
        }
        return;
    }
    if(rule[var]) {
        curString.push_back(ruleVars[curRuleVar]);
        removeString(var+1, curString, rule, curRuleVar+1, ruleVars, n, remStrings);
        return;
    }
    for(int i=0; i<variableInstances[var]; i++){
        curString.push_back(i);
        removeString(var+1, curString, rule, curRuleVar, ruleVars, n, remStrings);
        curString.pop_back();
    }
}

int solve(int n, vbs A, vi f){
    vCnt=0;
    vti.clear();
    itv.clear();
    enumerateAllStrings(n, vi());


    vi nStrings(A.size());
    // init total number of strings
    for(int i=0; i<A.size(); i++){
        nStrings[i]=1;
        for(int j=0; j<n; j++){
            if(A[i][j]) nStrings[i]*=variableInstances[j];
        }
    }

    vector<di> tmp;
    for(int i=0; i<A.size(); i++){
        if(A[i].count()==1) continue;
        tmp.push_back(di((double)(nStrings[i]-f[i])/nStrings[i],i));
    }
    sort(tmp.rbegin(), tmp.rend());

    vbs newA;
    vi newNStrings;
    for(int i=0; i<tmp.size(); i++){
        newA.push_back(A[tmp[i].second]);
        newNStrings.push_back(nStrings[tmp[i].second]-f[tmp[i].second]);
    }

    set<int> remStrings;

    for(int rule=0; rule<newA.size(); rule++){
        int left = newNStrings[rule];

        vector<vi> patterns;
        enumerateStrings(0, n, vi(), newA[rule], patterns);

        vector<ii> tmp;

        for(int i=0; i<patterns.size(); i++){
            int usedStrings=0;
            for(set<int>::iterator it=remStrings.begin(); it!=remStrings.end(); it++){
                vi remString = itv[*it];
                bool match=true;
                for(int j=0, k=0; j<n; j++){
                    if(newA[rule][j] && patterns[i][k++]!=remString[j]){
                        match=false;
                        break;
                    }
                }
                if(match){
                    usedStrings++;
                }
            }
            tmp.push_back(ii(usedStrings, patterns.size()-i));
        }
        sort(tmp.rbegin(), tmp.rend());

        if(debug){
            printf("%s\n", newA[rule].to_string().substr(NV-n).c_str());
            for(int i=0; i<tmp.size(); i++){
                for(int j=0; j<patterns[patterns.size()-tmp[i].second].size(); j++) printf("%d ", patterns[patterns.size()-tmp[i].second][j]+1);
                printf(":: %d \n", tmp[i].first);
            }
        }

        for(int i=0; i<left; i++){
            removeString(0, vi(), newA[rule], 0, patterns[patterns.size()-tmp[i].second], n, remStrings);
        }
    }
    if(checkConstraints(vi(remStrings.begin(), remStrings.end()), n, A, f)) return vti.size()-remStrings.size();
    return -1;
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

    vi f;
    f.push_back(3);
    f.push_back(4);
    f.push_back(7);

    variableInstances.clear();
    variableInstances.push_back(3);
    variableInstances.push_back(4);

    return solve(n, A, f);
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

    vi f;

    f.push_back(4);
    f.push_back(6);
    f.push_back(3);
    f.push_back(8);
    f.push_back(8);
    f.push_back(6);

    variableInstances.clear();
    variableInstances.push_back(4);
    variableInstances.push_back(6);
    variableInstances.push_back(3);

    return solve(n,A,f);
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

    vi f;
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

    variableInstances.clear();
    variableInstances.push_back(3);
    variableInstances.push_back(2);
    variableInstances.push_back(3);
    variableInstances.push_back(2);
    variableInstances.push_back(3);

    return solve(n,A,f);
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

    vi f;

    f.push_back(3);
    f.push_back(2);
    f.push_back(2);
    f.push_back(8);
    f.push_back(8);
    f.push_back(6);

    variableInstances.clear();
    variableInstances.push_back(3);
    variableInstances.push_back(2);
    variableInstances.push_back(2);

    return solve(n, A, f);
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

    vi f;
    f.push_back(3);
    f.push_back(1);
    f.push_back(1);
    f.push_back(1);
    f.push_back(4);

    f.push_back(3);
    f.push_back(7);
    f.push_back(4);
    f.push_back(4);
    f.push_back(6);
    f.push_back(6);
    f.push_back(12);

    variableInstances.clear();
    variableInstances.push_back(3);
    variableInstances.push_back(1);
    variableInstances.push_back(1);
    variableInstances.push_back(1);
    variableInstances.push_back(4);

    return solve(n,A,f);
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

    vi f;

    f.push_back(2);
    f.push_back(4);
    f.push_back(2);
    f.push_back(6);
    f.push_back(3);
    f.push_back(5);

    variableInstances.clear();
    variableInstances.push_back(2);
    variableInstances.push_back(4);
    variableInstances.push_back(2);

    return solve(n,A,f);
}

int main(){
    printf("Test 0: %d\n", test0());
    printf("Test 1: %d\n", test1());
    printf("Test 2: %d\n", test2());
    printf("Test 3: %d\n", test3());
    printf("Test 4: %d\n", test4());
    printf("Test 5: %d\n", test5());

    return 0;
}
