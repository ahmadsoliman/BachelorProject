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

void generateNextStrings(int i, int lastOnBit, vi curString, int bm, vi maximalVar, vi remStrings, queue<vi> &q){
    if(i==lastOnBit){
        int curStringIndex=vti[curString];
        if(find(remStrings.begin(), remStrings.end(), curStringIndex)==remStrings.end()){
            remStrings.push_back(curStringIndex);
            q.push(remStrings);
        }
        return;
    }
    if(bm&(1<<i)){
        generateNextStrings(i+1, lastOnBit, curString, bm, maximalVar, remStrings, q);
    }else{
        for(int j=0; j<=maximalVar[i]; j++){
            curString[i]=j;
            generateNextStrings(i+1, lastOnBit, curString, bm, maximalVar, remStrings, q);
        }
    }
}

int bfs(int n, vbs A, vi f){
    vCnt=0;
    vti.clear();
    itv.clear();
    enumerateStrings(n, vi());

    queue<vi> q;
    q.push(vi());
    while(!q.empty()){
        vi remStrings = q.front(); q.pop();
        if(checkConstraints(remStrings, n, A, f)){
            return vti.size()-remStrings.size();
        }
        if(remStrings.empty()){
            vi firstString(n, 0);
            remStrings.push_back(vti[firstString]);
            q.push(remStrings);
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
                generateNextStrings(0, lastOnBit, cur, bm, maximalVar, remStrings, q);
            }
        }
    }
    return 0;
}

int main(){
    int n=3;

    vbs A;

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

    variableInstances.push_back(2);
    variableInstances.push_back(2);
    variableInstances.push_back(2);
/*
    int n=2;
    vbs A;

    bs A3(string("11"));
    A.push_back(A3);

    vi f;
    f.push_back(3);

    variableInstances.push_back(2);
    variableInstances.push_back(3);
*/
    printf("%d\n", bfs(n,A,f));

    return 0;
}
