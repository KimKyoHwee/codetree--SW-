#include <iostream>
#include <utility>
#include <queue>
#include <vector>
#include <algorithm>
using namespace std;
//전역변수 선언
int N, M, K;
int board[11][11];
int lastAttack[11][11]; //포탑마다 가장 최근에 공격한 턴
int rowColSum[11][11];   //각 행과 열의 합 (i행j열의 값은 i+j)
int moveY[4] = { 0,1,0,-1 };  //우하좌상
int moveX[4] = { 1,0,-1,0 };
int bombY[9] = { -1,-1,-1,0,0,0,1,1,1 };
int bombX[9] = { -1,0,1,-1,0,1,-1,0,1 };
pair<int, int> attacker;
pair<int, int> target;
bool attackInTurn[11][11] = { false };  //이번턴에 공격과 관련이 있었는지
int backX[11][11]; //역추적
int backY[11][11]; //역추적
bool visited[11][11];
bool attacked[11][11]; //이번턴에 공격과 연관이 있는지
struct Turret {
    int y, x, power, sum;
};
vector<Turret> tV;

bool cmp(Turret a, Turret b) {
    if (board[a.y][a.x] != board[b.y][b.x])return board[a.y][a.x] > board[b.y][b.x];
    if (lastAttack[a.y][a.x] != lastAttack[b.y][b.x]) return lastAttack[a.y][a.x] < lastAttack[b.y][b.x];
    if (a.sum != b.sum) return a.sum < b.sum;
    return a.x < b.x;
}


bool laserAttack() {
    bool flag = false; //레이저 공격 가능한지
    queue<pair<int, int>> q;
    q.push(attacker);
    visited[attacker.first][attacker.second] = true;
    while (!q.empty()) {
        int y, x;
        pair<int, int> p = q.front(); q.pop();
        y = p.first; x = p.second;
        
        if (y == target.first && x == target.second) {  //목적지도착
            flag = true;
            break;
        }
 
        for (int i = 0; i < 4; i++) {
            
            int ny = (y + moveY[i] + N) % N;
            if (ny == 0) ny = N; 
            int nx = (x + moveX[i] + M) % M;
            if (nx == 0) nx = M;
            //cout << ny << ", " << nx << "\n";
            if (visited[ny][nx]) continue;  //이미 방문한곳 안감
            if (board[ny][nx] == 0) continue; //죽은포탑 못지나감
            
            visited[ny][nx] = true;
            backY[ny][nx] = y;
            backX[ny][nx] = x;
            q.push({ ny, nx });
            
        }
 
    }

    if (flag) {//만약 도달했으면
 
        attacked[attacker.first][attacker.second] = true;
        int y, x;
        y = target.first; x = target.second;
        board[y][x] -= board[attacker.first][attacker.second];  //포탑공격
        if (board[y][x] < 0) board[y][x] = 0;
        attacked[y][x] = true;
        attacked[attacker.first][attacker.second] = true;
        
        while (1) {  //역추적 시작
            
            int ny = backY[y][x];
            int nx = backX[y][x];
            if (ny == attacker.first && nx == attacker.second) {  //역추적종료
                break;
            }
            board[ny][nx] -= (board[attacker.first][attacker.second] / 2);
            if (board[ny][nx] < 0) board[ny][nx] = 0;
            attacked[ny][nx] = true;
            y = ny; x = nx;
        }
    }
    return flag;
}
void bombAttack() {
    //cout << "포탄공격\n";
    int y, x;
    y = target.first; x = target.second;
    attacked[attacker.first][attacker.second] = true;
    attacked[y][x] = true;
    board[y][x] -= board[attacker.first][attacker.second];
    if (board[y][x] < 0) board[y][x] = 0;
    for (int i = 0; i < 9; i++) {
        int ny = (y + bombY[i]+N)%N; int nx = (x + bombX[i]+M)%M;
        if (ny == 0) ny = N; if (nx == 0) nx = M;
        if (ny == y && nx == x) continue; //공격받는 포탑 제외
        if (ny == attacker.first && nx == attacker.second) continue;
        board[ny][nx] -= (board[attacker.first][attacker.second]) / 2;
        if (board[ny][nx] < 0) board[ny][nx] = 0;
        attacked[ny][nx] = true;
    }
}
void attack() {
    for (int i = 1; i <= N; i++) {  //초기화
        for (int j = 1; j <= M; j++) {
            visited[i][j] = false;
            attacked[i][j] = false;
        }
    }
    if (!laserAttack()) {
        for (int i = 1; i <= N; i++) {  //초기화
            for (int j = 1; j <= M; j++) {
                visited[i][j] = false;
                attacked[i][j] = false;
            }
        }
        bombAttack();
    }
}
void last() {
    for (int i = 1; i <= N; i++) {
        for (int j = 1; j <= M; j++) {
            if (!attacked[i][j]&&board[i][j]!=0) {
                board[i][j]++;
            }
        }
    }
}
void startTurn(int turn) {
    sort(tV.begin(), tV.end(), cmp);
    target.first = tV[0].y;
    target.second = tV[0].x;
    attacker.first = tV[tV.size() - 1].y;
    attacker.second = tV[tV.size() - 1].x;
    board[attacker.first][attacker.second] += (N + M);
    lastAttack[attacker.first][attacker.second] = turn;
    attack();
    last();
}

int findStrongMan() {
    int maxAttack = 0;
    for (int i = 1; i <= N; i++) {
        for (int j = 1; j <= M; j++) {
            if (maxAttack < board[i][j]) maxAttack = board[i][j];
        }
    }
    return maxAttack;
}

int main() {
    cin >> N >> M >> K;
    for (int i = 1; i <= N; i++) {
        for (int j = 1; j <= M; j++) {
            cin >> board[i][j];
            if (board[i][j] != 0) {
                Turret turret;
                turret.y = i; turret.x = j; turret.sum = i + j;
                tV.push_back(turret);
            }
        }
    }
    for (int i = 1; i <= K; i++) {
        startTurn(i);
    
        tV.clear();
        int cnt = 0;
        for (int j = 1; j <= N; j++) {
            for (int k = 1; k <= M; k++) {
                if (board[j][k] != 0) {
                    cnt++;
                    Turret turret;
                    turret.y = j; turret.x = k; turret.sum = j + k;
                    tV.push_back(turret);
                }
            }
        }
        /*
        cout << i << "턴종료\n";
        for (int j = 1; j <= N; j++) {
            
            for (int k = 1; k <= M; k++) {
                cout << board[j][k] << " ";
            }
            cout << "\n";
        }
        */
        if (cnt <= 1) break;
    }
    cout << findStrongMan();
    return 0;
}