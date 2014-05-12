#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
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

#define INF 1000000000

typedef pair<int,int> ii;
typedef pair<double,int> di;
typedef vector<int> vi;
typedef vector<double> vd;
typedef vector<vd> vvd;
typedef bitset<NV> bs;
typedef vector<bs> vbs;
typedef vector<vi> vvi;

bool compareBs(const bs& lhs, const bs& rhs){
    for(int i=NV-5; i>=0; i--){
        if(lhs[i]<rhs[i]) return true;
        else if(lhs[i]>rhs[i]) return false;
    }
    return false;
}

int vCnt;
map<vi, int> vti; // maps vectors which represent a string to an int
map<int, vi> itv;

vi variableInstances, variableInstancesOrig; // the number of instances of each variable

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

int solve1(int n, vbs A, vi f, vi nStrings){
    set<int> remStrings;

    for(int rule=0; rule<A.size(); rule++){
        int left = nStrings[rule]-f[rule];

        vector<vi> patterns;
        enumerateStrings(0, n, vi(), A[rule], patterns);

        vector<ii> tmp;

        for(int i=0; i<patterns.size(); i++){
            int usedStrings=0;
            for(set<int>::iterator it=remStrings.begin(); it!=remStrings.end(); it++){
                vi remString = itv[*it];
                bool match=true;
                for(int j=0, k=0; j<n; j++){
                    if(A[rule][j] && patterns[i][k++]!=remString[j]){
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
            printf("%s\n", A[rule].to_string().substr(NV-n).c_str());
            for(int i=0; i<tmp.size(); i++){
                for(int j=0; j<patterns[patterns.size()-tmp[i].second].size(); j++) printf("%d ", patterns[patterns.size()-tmp[i].second][j]+1);
                printf(":: %d \n", tmp[i].first);
            }
        }

        for(int i=0; i<left; i++){
            removeString(0, vi(), A[rule], 0, patterns[patterns.size()-tmp[i].second], n, remStrings);
        }
    }
    if(checkConstraints(vi(remStrings.begin(), remStrings.end()), n, A, f)) return vti.size()-remStrings.size();
    return -1;
}

int greedyPermutations(int n, vbs A, vi f){
    vCnt=0;
    vti.clear();
    itv.clear();
    enumerateAllStrings(n, vi());

    vbs newA;
    vi newf;
    for(int i=0; i<A.size(); i++){
        if(A[i].count()>1){
            newA.push_back(A[i]);
            newf.push_back(f[i]);
        }
    }

    vi nStrings(newA.size());
    // init total number of strings
    for(int i=0; i<newA.size(); i++){
        nStrings[i]=1;
        for(int j=0; j<n; j++){
            if(newA[i][j]) nStrings[i]*=variableInstances[j];
        }
    }

    int maxVal=0;

    vi perm(newA.size());
    for(int i=0; i<newA.size(); i++) perm[i]=i;

    do{
        vbs curA;
        vi curf, curNStrings;
        for(int i=0; i<perm.size(); i++){
            curA.push_back(newA[perm[i]]);
            curf.push_back(newf[perm[i]]);
            curNStrings.push_back(nStrings[perm[i]]);
        }
        int cur=solve1(n, curA, curf, curNStrings);
        if(debug) printf("cur val: %d\n", cur);
        maxVal = max(maxVal, cur);
    }while(next_permutation(perm.begin(), perm.end()));

    return maxVal;
}

// ----------------------------------------------------------------------------------------------

int greedyPermutations2(int n, vbs A, vi f){
    variableInstancesOrig = variableInstances;

    vbs newA;
    vi newf;
    for(int i=0; i<A.size(); i++){
        if(A[i].count()>1){
            newA.push_back(A[i]);
            newf.push_back(f[i]);
        }
    }

    vi nStrings(newA.size());
    // init total number of strings
    for(int i=0; i<newA.size(); i++){
        nStrings[i]=1;
        for(int j=0; j<n; j++){
            if(newA[i][j]) nStrings[i]*=variableInstances[j];
        }
    }

    int maxVal=0;

    vi perm(newA.size());
    for(int i=0; i<newA.size(); i++) perm[i]=i;

    do{
        vbs curA;
        vi curf, curNStrings;
        for(int i=0; i<perm.size(); i++){
            curA.push_back(newA[perm[i]]);
            curf.push_back(newf[perm[i]]);
            curNStrings.push_back(nStrings[perm[i]]);
        }

        vi perm2(n);
        for(int i=0; i<n; i++) perm2[i]=i;
        do{
            vbs curA2;
            for(int i=0; i<curA.size(); i++){
                bs curRule;
                for(int j=0; j<n; j++){
                    curRule[j]=curA[i][perm2[j]];
                    variableInstances[j] = variableInstancesOrig[perm2[j]];
                }
                curA2.push_back(curRule);
            }
            vCnt=0;
            vti.clear();
            itv.clear();
            enumerateAllStrings(n, vi());

            int cur=solve1(n, curA2, curf, curNStrings);
            if(debug) printf("cur val: %d\n", cur);
            maxVal = max(maxVal, cur);
        }while(next_permutation(perm2.begin(), perm2.end()));
    }while(next_permutation(perm.begin(), perm.end()));

    variableInstances = variableInstancesOrig;
    return maxVal;
}

// ----------------------------------------------------------------------------------------------

int recur(int rule, int left, int lastPattern, vector<vi> patterns, vi maxVarVal, set<int> remStrings, int n, vbs A, vi f, vi nStrings){
    if(left > patterns.size() - lastPattern - 1) return -1;
    if(left==0){
        rule++;
        if(rule==A.size()){
            if(checkConstraints(vi(remStrings.begin(), remStrings.end()), n, A, f)) {
                if(debug) printf("%d %d %d\n", vti.size(), remStrings.size(), vti.size()-remStrings.size());
                if(debug){
                    printf("START\n");
                    for(set<int>::iterator it=remStrings.begin(); it!=remStrings.end(); it++){
                        vi curString = itv[*it];
                        for(int i=0; i<n; i++){
                            printf("V%d: %d, ", i, curString[i]+1);
                        }
                        printf("\n");
                    }
                }
                return vti.size()-remStrings.size();
            }
            return -1;
        }
        vector<vi> patterns;
        enumerateStrings(0, n, vi(), A[rule], patterns);
        return recur(rule, nStrings[rule]-f[rule], -1, patterns, maxVarVal, remStrings, n, A, f, nStrings);
    }
    if(lastPattern==-1){
        removeString(0, vi(), A[rule], 0, patterns[0], n, remStrings);
        return recur(rule, left-1, 0, patterns, maxVarVal, remStrings, n, A, f, nStrings);
    }
    int maxVal = 0;
    for(int i=lastPattern+1; i<patterns.size(); i++){
        vi tmpMaxVarVal = maxVarVal;
        bool valid=1;
        for(int j=0, k=0; j<n; j++){
            if(A[rule][j]){
                if(patterns[i][k]>maxVarVal[j]+1){
                    valid = 0;
                    break;
                }
                tmpMaxVarVal[j] = max(tmpMaxVarVal[j], patterns[i][k++]);
            }
        }
        if(valid){
            set<int> tmpRemStrings = remStrings;
            removeString(0, vi(), A[rule], 0, patterns[i], n, tmpRemStrings);
            maxVal = max(maxVal, recur(rule, left-1, i, patterns, tmpMaxVarVal, tmpRemStrings, n, A, f, nStrings));
        }
    }
    return maxVal;
}

int patternBased(int n, vbs A, vi f){
    vCnt=0;
    vti.clear();
    itv.clear();
    enumerateAllStrings(n, vi());

    vbs newA;
    vi newf;
    for(int i=0; i<A.size(); i++){
        if(A[i].count()>1){
            newA.push_back(A[i]);
            newf.push_back(f[i]);
        }
    }

    vi nStrings(newA.size());
    // init total number of strings
    for(int i=0; i<newA.size(); i++){
        nStrings[i]=1;
        for(int j=0; j<n; j++){
            if(newA[i][j]) nStrings[i]*=variableInstances[j];
        }
    }

    if(newA.size()==0) return 0;

    vector<vi> patterns;
    enumerateStrings(0, n, vi(), newA[0], patterns);

    return recur(0, nStrings[0]-newf[0], -1, patterns, vi(n, 0), set<int>(), n, newA, newf, nStrings);
}

// ----------------------------------------------------------------------------------------------

int recur2(int rule, int left, int lastPattern, vector<vi> patterns, vi maxVarVal, set<int> remStrings, int n, vbs A, vi f, vi nStrings){
    if(left > patterns.size() - lastPattern - 1) return -1;
    if(left==0){
        rule++;
        if(rule==A.size()){
            if(checkConstraints(vi(remStrings.begin(), remStrings.end()), n, A, f)) {
                if(debug) printf("%d %d %d\n", vti.size(), remStrings.size(), vti.size()-remStrings.size());
                if(debug){
                    printf("START\n");
                    for(set<int>::iterator it=remStrings.begin(); it!=remStrings.end(); it++){
                        vi curString = itv[*it];
                        for(int i=0; i<n; i++){
                            printf("V%d: %d, ", i, curString[i]+1);
                        }
                        printf("\n");
                    }
                }
                return vti.size()-remStrings.size();
            }
            return -1;
        }
        vector<vi> patterns;
        enumerateStrings(0, n, vi(), A[rule], patterns);
        return recur2(rule, nStrings[rule]-f[rule], -1, patterns, maxVarVal, remStrings, n, A, f, nStrings);
    }
    if(lastPattern==-1){
        removeString(0, vi(), A[rule], 0, patterns[0], n, remStrings);
        return recur2(rule, left-1, 0, patterns, maxVarVal, remStrings, n, A, f, nStrings);
    }
    int maxVal = 0;
    for(int i=lastPattern+1; i<patterns.size(); i++){
        /*vi tmpMaxVarVal = maxVarVal;
        bool valid=1;
        for(int j=0, k=0; j<n; j++){
            if(A[rule][j]){
                if(patterns[i][k]>maxVarVal[j]+1){
                    valid = 0;
                    break;
                }
                tmpMaxVarVal[j] = max(tmpMaxVarVal[j], patterns[i][k++]);
            }
        }*/
        //if(valid){
        set<int> tmpRemStrings = remStrings;
        removeString(0, vi(), A[rule], 0, patterns[i], n, tmpRemStrings);
        maxVal = max(maxVal, recur2(rule, left-1, i, patterns, maxVarVal, tmpRemStrings, n, A, f, nStrings));
        //}
    }
    return maxVal;
}

int patternBasedBF(int n, vbs A, vi f){
    vCnt=0;
    vti.clear();
    itv.clear();
    enumerateAllStrings(n, vi());

    vbs newA;
    vi newf;
    for(int i=0; i<A.size(); i++){
        if(A[i].count()>1){
            newA.push_back(A[i]);
            newf.push_back(f[i]);
        }
    }

    vi nStrings(newA.size());
    // init total number of strings
    for(int i=0; i<newA.size(); i++){
        nStrings[i]=1;
        for(int j=0; j<n; j++){
            if(newA[i][j]) nStrings[i]*=variableInstances[j];
        }
    }

    if(newA.size()==0) return 0;

    vector<vi> patterns;
    enumerateStrings(0, n, vi(), newA[0], patterns);

    return recur2(0, nStrings[0]-newf[0], -1, patterns, vi(n, 0), set<int>(), n, newA, newf, nStrings);
}

// ----------------------------------------------------------------------------------------------


double approximation1Recur(int n, vbs A, vd f){
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

        maxValue=max(maxValue, val*approximation1Recur(n, Aij, fij));
        val--;
    }

    return maxValue;
}

int approximation1(int n, vbs A, vi f){
    vbs newA;
    vd fd;
    for(int j=0; j<n; j++) {
        bs rule;
        rule[j]=1;

        newA.push_back(rule);
        fd.push_back(variableInstances[j]);
    }

    for(int j=0; j<f.size(); j++) newA.push_back(A[j]), fd.push_back(f[j]);

    return approximation1Recur(n, newA, fd);
}
// ----------------------------------------------------------------------------------------------
struct node{
    int var;
    vector<node> chlds;
    node(int var1, vector<node> chlds1){
        var=var1;
        chlds=chlds1;
    }
};

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
    return countLeaves(root, n);
}

double approximation2Recur(int n, vbs A, vd f, vvd values){
    if(n==1) {
        values[0][0] = f[0];
        for(int i=0; i<values.size(); i++) values[0][i]=min(values[0][i], f[0]);

        for(int i=0; i<values.size(); i++){
            for(int j=i+1; j<values.size(); j++){
                if(values[i][j]<1e-9) values[i][j]=values[i][i];
            }
        }

        return calc(values.size(), values);
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

        maxValue=max(maxValue, approximation2Recur(n, newA, newf, tmpVals));
        val = floor(val)-1;
    }

    return maxValue;
}

int approximation2(int n, vbs A, vi f){
    vbs newA;
    vd fd;
    for(int j=0; j<n; j++) {
        bs rule;
        rule[j]=1;

        newA.push_back(rule);
        fd.push_back(variableInstances[j]);
    }

    for(int j=0; j<f.size(); j++) newA.push_back(A[j]), fd.push_back(f[j]);

    return approximation2Recur(n, newA, fd, vvd(n, vd(n)));
}
// ----------------------------------------------------------------------------------------------

int randomInt(int maxV){
    return (1.0*rand()/RAND_MAX)*maxV;
}

bs getRandomRule(int n){
    bs rule;
    while(rule.count()<2 || rule.count()==n){
        for(int i=0; i<n; i++){
            rule[i]=randomInt(2);
        }
    }
    return rule;
}

void printCase(int n, vbs A, vi f){
    printf("Case Details:\n");
    printf("--------------\n");
    printf("Number Of Variables n: %d\n\n", n);
    printf("Rules:\n");
    printf("-------\n");
    for(int i=0; i<n; i++){
        printf("%c :: %d\n", 'A'+i, variableInstances[i]);
    }
    for(int i=0; i<A.size(); i++){
        for(int j=0; j<n; j++){
            if(A[i][j]) printf("%c ", 'A'+j);
        }
        printf(":: %d\n", f[i]);
    }
}

void generateTestCase(int n, int nRules, int maxF, vbs &A, vi &f){
    variableInstances = vi(n);
    int allN=1;
    for(int i=0; i<n; i++){
        variableInstances[i] = 2+randomInt(maxF-2);
        allN *= variableInstances[i];
    }
    if(allN>=35) { generateTestCase(n, nRules, maxF, A, f); return; }

    vector<bs> tmpA;
    while(tmpA.size()<nRules){
        bs randR = getRandomRule(n);
        if(find(tmpA.begin(), tmpA.end(), randR) == tmpA.end())
            tmpA.push_back(randR);
    }
    A = vbs(tmpA.begin(), tmpA.end());
    sort(A.begin(), A.end(), compareBs);

    f = vi(nRules);
    for(int i=0; i<nRules; i++){
        int nStrings=1;
        for(int j=0; j<n; j++){
            if(A[i][j]) nStrings*=variableInstances[j];
        }
        f[i] = 1+randomInt(nStrings-1);
    }
}

void randomTestCase(int &n, vbs &A, vi &f){
    //int n = 2+randomInt(2);
    n = 3;//3+randomInt(1);
    int nRules;
    do{
        nRules = 1+randomInt((int)pow(2,n)-1-n-1);
    }while(nRules>5 || (nRules<4 && n==4));

    int maxF = randomInt(5);
    generateTestCase(n, nRules, 6, A, f);
}

void test(){
    int n;
    vbs A;
    vi f;

    // Approximation Test
    int cummDiff=0, cummSum=0, noOfExact=0;
    double cummPer=0;
    for(int i=0; i<1000; i++) {
        randomTestCase(n, A, f);

        int approxAns = approximation2(n, A, f), ans=patternBased(n, A, f);
        noOfExact += approxAns==ans;

        if(approxAns!=ans){
            printf("Approximation 2: %d\n", approxAns);
            printf("Pattern Based Search Ans: %d\n", ans);
            printCase(n, A, f);
        }else printf("done\n");

        cummSum += ans;
        cummDiff += abs(approxAns-ans);
        cummPer += (double)abs(approxAns-ans)/ans;
    }

    printf("%lf\n", 100.0*cummDiff/cummSum); // 3.44 with 1000 test cases
    printf("%lf\n", cummPer/10.0); // 2.14 with 1000 test cases
    printf("%lf\n", noOfExact/10.0); //91% with 1000 test cases

/*
    for(int i=0; i<100; i++) {
        randomTestCase(n, A, f);

        int a=patternBasedBF(n, A, f), b=patternBased(n, A, f);
        if(a==b){
            printf("Done\n");
        }else{
            printCase(n, A, f);
            printf("--------------------------\n");
            printf("Pattern Based Brute Force Ans: %d\n", a);
            printf("Pattern Based Search Ans: %d\n", b);
            break;
        }
    }
    // exact for 10000 cases finished in 8 minutes
*/
}

int main(){
    srand(time(0));

    test();

    return 0;
}
