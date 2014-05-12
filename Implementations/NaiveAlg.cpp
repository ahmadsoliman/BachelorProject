#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include <queue>
#include <set>

using namespace std;

struct var{
    string name;
    int id;
    var(string n){
        name = n;
        id = 0;
    }
    var(string n, int i){
        name = n;
        id = i;
    }
};

inline bool operator<(const var& lhs, const var& rhs){
    if(lhs.name==rhs.name) return lhs.id<rhs.id;
    return lhs.name < rhs.name;
}

typedef vector<int> vi;
typedef vector<var> vs;
typedef vector<vs> vvs;

vvs strings;
vs variables;
vi variableInstances;

void initVariables(vs V, vvs A, vi f){
    variables.clear();
    variableInstances.clear();
    for(int i=0; i<V.size(); i++){
        for(int j=0; j<A.size(); j++){
            if(A[j].size()>1) continue;
            if(V[i].name==A[j][0].name){
                variables.push_back(V[i]);
                variableInstances.push_back(f[j]);
                break;
            }
        }
    }
}

void enumerateStrings(vs curV){
    int curI = curV.size();
    if(curV.size()==variables.size()){
        strings.push_back(curV);
        return;
    }
    for(int i=1; i<=variableInstances[curI]; i++){
        var v = variables[curI];
        v.id = i;
        curV.push_back(v);
        enumerateStrings(curV);
        curV.pop_back();
    }
}

bool checkConstraints(vvs strings, vvs A, vi f){
    for(int i=0; i<A.size(); i++){
        if(A[i].size()==1) continue;
        set<vs> s;
        for(int j=0; j<strings.size(); j++){
            vs usedInstances;
            for(int k=0; k<A[i].size(); k++){
                for(int l=0; l<strings[j].size(); l++){
                    if(A[i][k].name==strings[j][l].name){
                        usedInstances.push_back(strings[j][l]);
                        break;
                    }
                }
            }
            s.insert(usedInstances);
            if(s.size()>f[i]) return false;
        }
    }
    return true;
}

int bfs(vs V, vvs A, vi f){
    strings.clear();
    initVariables(V, A, f);
    enumerateStrings(vs());

    queue<vvs> q;
    q.push(strings);
    while(!q.empty()){
        vvs curStrings = q.front(); q.pop();
        if(checkConstraints(curStrings, A, f)){
            return curStrings.size();
        }
        for(int i=0; i<curStrings.size(); i++){
            vvs v = curStrings;
            v.erase(v.begin()+i);
            q.push(v);
        }
    }
    return 0;
}

int main(){
    vector<var> V;
    V.push_back(var("X"));
    V.push_back(var("Y"));

    vvs A;
    vector<var> tmpv;
    tmpv.push_back(var("X"));
    A.push_back(tmpv);

    tmpv.clear();
    tmpv.push_back(var("Y"));
    A.push_back(tmpv);

    tmpv.clear();
    tmpv.push_back(var("X"));
    tmpv.push_back(var("Y"));
    A.push_back(tmpv);

    vi f;
    f.push_back(4);
    f.push_back(3);
    f.push_back(6);

    printf("%d\n", bfs(V,A,f));

    /*
    for(int i=0; i<strings.size(); i++){
        for(int j=0; j<strings[i].size(); j++){
            printf("%s: %d, ", strings[i][j].name.c_str(), strings[i][j].id);
        }
        printf("\n");
    }*/

    return 0;
}
