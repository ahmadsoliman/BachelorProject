#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include <queue>
#include <set>
#include <bitset>
#include <map>
#include <algorithm>

// Variables count limit
#define NV 100

using namespace std;

typedef vector<int> vi;
typedef bitset<NV> bs;
typedef vector<bs> vbs;

int vCnt;
map<vi, int> vti; // maps vectors which represent a string to an int
map<int, vi> itv; // maps vectors which represent a string to an int

vi variableInstances; // the number of instances of each variable

void enumerateStrings(int n, vi curV){
    if(curV.size()==n){
        vti[curV]=vCnt;
        itv[vCnt++]=curV;
        return;
    }
    int curI = curV.size();
    for(int i=0; i<variableInstances[curI]; i++){
        curV.push_back(i);
        enumerateStrings(n, curV);
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

void generateNextStrings(int i, int lastOnBit, vi curString, int bm, vi maximalVar, vi remStrings, vector<vi> &remStringsList){
    if(i==lastOnBit){
        int curStringIndex=vti[curString];
        if(find(remStrings.begin(), remStrings.end(), curStringIndex)==remStrings.end()){
            remStrings.push_back(curStringIndex);
            remStringsList.push_back(remStrings);
        }
        return;
    }
    if(bm&(1<<i)){
        generateNextStrings(i+1, lastOnBit, curString, bm, maximalVar, remStrings, remStringsList);
    }else{
        for(int j=0; j<=maximalVar[i]; j++){
            curString[i]=j;
            generateNextStrings(i+1, lastOnBit, curString, bm, maximalVar, remStrings, remStringsList);
        }
    }
}

int recur(int d, vi remStrings, int n, vbs A, vi f){
    if(d==0) return -1;
    if(checkConstraints(remStrings, n, A, f)){
        return vti.size()-remStrings.size();
    }
    int maxVal=-1;
    if(remStrings.empty()){
        vi firstString(n, 0);
        remStrings.push_back(vti[firstString]);
        return recur(d-1, remStrings, n, A, f);
    }else{
        vi maximalVar(n, 0);
        for(int i=0; i<remStrings.size(); i++){
            vi str = itv[remStrings[i]];
            for(int j=0; j<n; j++){
                maximalVar[j]=max(maximalVar[j], str[j]);
            }
        }
        vi lastRemoved = itv[remStrings.back()];
        for(int bm=0; bm<(1<<n); bm++){
            int lastOnBit=0;
            vi cur;
            for(int i=0; i<n; i++){
                if((bm&(1<<i)) && lastRemoved[i]<variableInstances[i]-1){
                    cur.push_back(lastRemoved[i]+1);
                    lastOnBit=i;
                }else{
                    cur.push_back(lastRemoved[i]);
                }
            }
            vector<vi> remStringsList;
            generateNextStrings(0, lastOnBit, cur, bm, maximalVar, remStrings, remStringsList);

            for(int i=0; i<remStringsList.size(); i++){
                maxVal = max(maxVal, recur(d-1, remStringsList[i], n, A, f));
            }
        }
    }
    return maxVal;
}

int iterativeDeepening(int n, vbs A, vi f){
    vCnt=0;
    vti.clear();
    itv.clear();
    enumerateStrings(n, vi());

    int ans=-1;
    for(int i=1; ans==-1 && i<10; i++){
        ans=recur(i, vi(), n, A, f);
    }
    return ans;
}

int main(){
 /*   int n=3;

    vbs A;
    bs A4(string("011"));
    A.push_back(A4);

    bs A5(string("101"));
    A.push_back(A5);

    bs A6(string("110"));
    A.push_back(A6);

    vi f;
    f.push_back(8);
    f.push_back(8);
    f.push_back(6);

    variableInstances.push_back(2);
    variableInstances.push_back(3);
    variableInstances.push_back(2);
*/
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

    printf("Ans: %d\n", iterativeDeepening(n,A,f));

    return 0;
}
