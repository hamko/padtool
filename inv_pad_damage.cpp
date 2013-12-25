#include <hamkoutil.hpp>
#define BASE 20
#define CUT_NUM 1000
#define OUTPUT_NUM 100
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

class lessCombo {
    public:
        bool operator()(const vi& riLeft, const vi& riRight) const {
            return riLeft[6] < riRight[6];
        }
};
class lessNum {
    public:
        bool operator()(const vi& riLeft, const vi& riRight) const {
            return riLeft[7] < riRight[7];
        }
};

void arrange_candidacy(vector<vi>& candidacy, int num)
{
    sort(candidacy.begin(), candidacy.end(), lessNum());
    if ((int)candidacy.size() <= num)
        return;
    auto it = candidacy.begin() + num;
    candidacy.erase(it, candidacy.end());
}

int main(int argc, char** argv)
{
    // Attack
    vi attack(COLOR_NUM); // red, blue, gree, white, black
    attack[RED]     = 1000;
    attack[BLUE]    = 1000;
    attack[GREEN]   = 1000;
    attack[WHITE]   = 1000;
    attack[BLACK]   = 1000;
    // Drop Num
    // Leader Skill Correction
    double lskill = 25.0;
    for (int i = 0; i < COLOR_NUM; i++) 
        attack[i] *= lskill;
    // Leader Skill Condition
    vi lcond = {1, 1, 1, 1, 0};
    // Objective
    int objective = argc > 1 ? atoi(argv[1]) : 100000;
    // Drop Num
    vi drop_limit = {3, 3, 5, 5, 5, 8};
    // Enemy Color
    int enemy_color = RED;
    
    // realistic combo pattern
    vi repertoire[BASE] {
        {},
        {3}, {4}, {5},
        {3, 3}, {3, 4}, {3, 5}, {4, 4}, {4, 5}, {5, 5},
        {3, 3, 3}, {3, 3, 4}, {3, 3, 5}, {3, 4, 4}, {3, 4, 5},
        {3, 5, 5}, {4, 4, 4}, {4, 4, 5}, {4, 5, 5}, {5, 5, 5},
    };
    double c_attack[BASE] = {};
    int c_num[BASE] = {};
    int c_combo[BASE] = {};
    for (int i = 0; i < BASE; i++) {
        for (int j = 0; j < (int)repertoire[i].size(); j++) {
            c_attack[i] += (1 + 0.25 * (repertoire[i][j] - 3));
            c_num[i] += repertoire[i][j]; 
        }
        c_combo[i] = (int)repertoire[i].size();
    }

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
            double damage = 0;
            int combo = r;
            int num = r * 3;
            for (int i = 0; i < COLOR_NUM; i++) {
               damage += (double)attack[i] * c_attack[drop[i]] * damage_table[i][enemy_color];
               combo += c_combo[drop[i]];
               num += c_num[drop[i]];
            }
            damage *= (0.75 + 0.25 * combo);
            // add candidacy
            if (damage > objective) {
                vi tmp = {drop[0], drop[1], drop[2], drop[3], drop[4], (int)damage, combo, num, r};
                candidacy.pb(tmp);
                if (candidacy.size() > CUT_NUM)
                    arrange_candidacy(candidacy, OUTPUT_NUM);
            }
        }
    }

    arrange_candidacy(candidacy, OUTPUT_NUM);
    for (int c = 0; c < (int)candidacy.size(); c++) {
        int damage = candidacy[c][5], combo = candidacy[c][6], num = candidacy[c][7], r = candidacy[c][8];
        cout << "Damage: " << damage << " (combo: " << combo << ", num: " << num << ")" << endl;
        for (int i = 0; i < COLOR_NUM; i++) {
            cout << color_string[i];
            for (int j = 0; j < (int)repertoire[candidacy[c][i]].size(); j++) {
                cout << repertoire[candidacy[c][i]][j];
            }
            cout << "\t";
        }
        cout << "Heart" << r << "(set)" << "\t";
        cout << endl;
    }

    return 0;
}
