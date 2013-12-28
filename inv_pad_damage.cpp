#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
using namespace std;

#define BASE 20
#define CUT_NUM 1000
#define ARRANGE_NUM_NUM 100
#define vi vector<int>
#define next atoi(argv[argi++])
enum {RED, BLUE, GREEN, WHITE, BLACK, COLOR_NUM};
string color_string[COLOR_NUM] = {"Red", "Blue", "Green", "White", "Black"};

typedef unsigned long long ull;

// damage_table[i][j]: rate of attack i->j
const double damage_table[COLOR_NUM][COLOR_NUM] = {
    {1.0, 0.5, 2.0, 1.0, 1.0, }, 
    {2.0, 1.0, 0.5, 1.0, 1.0, }, 
    {0.5, 2.0, 1.0, 1.0, 1.0, }, 
    {1.0, 1.0, 1.0, 1.0, 2.0, }, 
    {1.0, 1.0, 1.0, 2.0, 1.0, }, 
};

// ret = vector(5);
void decimal_to_base(vi& ret, ull i, int base)
{
    for (int k = 4; k >= 0; k--) {
        ret[k] = i % base; 
        i -= ret[k]; 
        i /= base;
    }
}

bool lessNum(const vi& riLeft, const vi& riRight) {
    return riLeft[7] < riRight[7];
}

void arrange_candidacy_num(vector<vi>& candidacy, int num)
{
    sort(candidacy.begin(), candidacy.end(), lessNum);
    if ((int)candidacy.size() <= num)
        return;
    auto it = candidacy.begin() + num;
    candidacy.erase(it, candidacy.end());
}

// Repertorie and compatibility
vi repertoire[BASE] = {
    {}, // 0-0
    {3}, {4}, {5}, // 1-3
    {3, 3}, {3, 4}, {3, 5}, {4, 4}, {4, 5}, {5, 5}, // 4-9
    {3, 3, 3}, {3, 3, 4}, {3, 3, 5}, {3, 4, 4}, {3, 4, 5}, 
    {3, 5, 5}, {4, 4, 4}, {4, 4, 5}, {4, 5, 5}, {5, 5, 5}, // 10-19
};
double c_attack[BASE] = {};
int c_num[BASE] = {};
int c_combo[BASE] = {};
void init_repertorie(void)
{
    for (int i = 0; i < BASE; i++) {
        for (int j = 0; j < (int)repertoire[i].size(); j++) {
            c_attack[i] += (1 + 0.25 * (repertoire[i][j] - 3));
            c_num[i] += repertoire[i][j]; 
        }
        c_combo[i] = (int)repertoire[i].size();
    }

}
bool is_same_sets(int i, int j) {
    if (i == 0 && j == 0) return 1;
    if (1 <= i && i <= 3 && 1 <= j && j <= 3) return 1;
    if (4 <= i && i <= 9 && 4 <= j && j <= 9) return 1;
    if (10 <= i && i <= 19 && 10 <= j && j <= 19) return 1;
    return 0;
}
// returns 1, if i is weaker than j or the same storegth.
bool check_upward_compatibility(int i, int j)
{
    return c_attack[i] >= c_attack[j];
}

int main(int argc, char** argv)
{
    int argi = 1;
    if (argc != 27) {
        cout << "Lacking for Input." << argc << endl;
        return 1;
    }
    // Objective
    int objective = next;
    int defense = next;
    // Enemy Color
    int enemy_color = next; 
    double lskill = next;
    // Attack
    vi attack(COLOR_NUM); // red, blue, gree, white, black
    attack[RED]     = next;
    attack[BLUE]    = next;
    attack[GREEN]   = next;
    attack[WHITE]   = next;
    attack[BLACK]   = next;
    // Drop Num
    vi drop_limit = {next, next, next, next, next, next};
    // Attacker Num
    vi attacknum = {next, next, next, next, next};
    // Leader Skill Condition
    vi lcond = {next, next, next, next, next, next};
    
    // Leader Skill Correction
    for (int i = 0; i < COLOR_NUM; i++) 
        attack[i] *= lskill;
    // realistic combo pattern
    init_repertorie();

    vector<vi> candidacy;
    for (int r = 0; r <= 3; r++) {
        if (r * 3 > drop_limit[COLOR_NUM])
            continue;
        for (ull d = 0; d < (ull)pow(BASE, (int)COLOR_NUM); d++) {
            vi drop(COLOR_NUM);
            // get drop cond
            decimal_to_base(drop, d, BASE);
            // LSkill Cond
            bool flag = 0;
            for (int i = 0; i < COLOR_NUM; i++) 
                if (lcond[i] > c_num[drop[i]] || drop_limit[i] < c_num[drop[i]]) {
                    flag = 1;
                    break;
                }
            if (flag)
                continue;
            // calc Damage
            int damage = 0, combo = r, num = r * 3, anum = 0;
            for (int i = 0; i < COLOR_NUM; i++) {
               damage += (int)((double)attack[i] * c_attack[drop[i]] * damage_table[i][enemy_color]);
               combo += c_combo[drop[i]];
               num += c_num[drop[i]];
               anum += (c_num[drop[i]] > 0 ? attacknum[i] : 0);
            }
            damage *= (0.75 + 0.25 * combo);
            damage -= anum * defense;
            // add candidacy
            if (damage > objective) {
                vi tmp = {drop[0], drop[1], drop[2], drop[3], drop[4], (int)damage, combo, num, r};
                candidacy.push_back(tmp);
                if (candidacy.size() > CUT_NUM)
                    arrange_candidacy_num(candidacy, ARRANGE_NUM_NUM);
            }
        }
    }

    // Compatibility
    // for i
    for (auto iti = candidacy.begin(); iti != candidacy.end();) {
        int flag = 0;
        for (auto itj = candidacy.begin(); itj != candidacy.end(); itj++) {
            if (iti == itj)
                continue;
            int faf = 1;
            for (int h = 0; h < COLOR_NUM; h++) {
                if (!((*iti)[h] >= (*itj)[h])) {
                    faf = 0;
                    break;
                }
            }
            if (!((*iti)[8] >= (*itj)[8]))
                faf = 0;
            if (!faf) {
                continue;
            } else if (faf) {
                flag = 1;
                break;
            }
        }
        if (flag)
            iti = candidacy.erase(iti, iti + 1);
        else
            iti++;
    }

    // Output 
    arrange_candidacy_num(candidacy, ARRANGE_NUM_NUM);
    if (candidacy.size() == 0) {
        cout << "No Result!" << endl;
        return 0;
    }
    for (int c = 0; c < (int)candidacy.size(); c++) {
        int damage = candidacy[c][5], combo = candidacy[c][6], num = candidacy[c][7], r = candidacy[c][8];
        for (int i = 0; i < COLOR_NUM; i++) {
            cout << color_string[i];
            for (int j = 0; j < (int)repertoire[candidacy[c][i]].size(); j++) 
                cout << repertoire[candidacy[c][i]][j];
            cout << "\t";
        }
        cout << "Heart" << r << "(set)" << "\t";
        cout << "(combo: " << combo << ", num: " << num << ", damage: " << damage << ")" << endl;
    }

    return 0;
}
