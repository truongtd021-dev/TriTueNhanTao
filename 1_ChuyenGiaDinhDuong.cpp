#include <bits/stdc++.h>
using namespace std;

// ======= MO HINH DU LIEU =======
enum TimeOfDay { MORNING, AFTERNOON, NIGHT };

struct Context {
    TimeOfDay timeOfDay;
    int needFocus;          // 0..10
    bool hotWeather;
    bool justExercised;
    bool caffeineSensitive;
    bool sleptLt6h;
    int hungry;             // 0..10
    bool stomachIssue;
    bool budgetLow;
};

struct Recommendation {
    string label;
    int score;
    string explanation;
};

struct Rule {
    string name;
    function<bool(const Context&)> when;
    function<Recommendation(const Context&)> then;
};

struct Ranked {
    string label;
    int totalScore;
    vector<string> explanations;
};

struct InferResult {
    vector<Ranked> ranked;
    vector<string> trace;
};

struct Agg {
    int totalScore = 0;
    vector<string> explanations;
    unordered_set<string> seen;
    void addExplanation(const string& e) {
        if (!e.empty() && !seen.count(e)) {
            seen.insert(e);
            explanations.push_back(e);
        }
    }
};

// ======= CO SO TRI THUC =======
vector<Rule> buildKnowledgeBase() {
    vector<Rule> R;

    R.push_back({
        "HOT_SWEAT_ELECTROLYTE",
        [](const Context& c) { return c.hotWeather && c.justExercised; },
        [](const Context& c) {
            return Recommendation{"Nuoc dien giai", 9,
                "Troi nong va vua van dong: bo sung nuoc + muoi khoang."};
        }
    });

    R.push_back({
        "FOCUS_COFFEE",
        [](const Context& c) {
            return c.needFocus >= 7 && !c.caffeineSensitive &&
                   (c.timeOfDay == MORNING || c.timeOfDay == AFTERNOON);
        },
        [](const Context& c) {
            return Recommendation{"Ca phe den", 8,
                "Ban can tap trung cao, thoi diem phu hop, khong nhay caffeine."};
        }
    });

    R.push_back({
        "FOCUS_TEA",
        [](const Context& c) { return c.needFocus >= 4 && c.needFocus <= 6 && c.caffeineSensitive; },
        [](const Context& c) {
            return Recommendation{"Tra xanh", 6,
                "Can tinh tao nhe, ban hoi nhay caffeine nen chon tra."};
        }
    });

    R.push_back({
        "NIGHT_RELAX",
        [](const Context& c) { return c.timeOfDay == NIGHT && c.needFocus <= 3; },
        [](const Context& c) {
            return Recommendation{"Sua am/Tra hoa cuc (decaf)", 7,
                "Ban dem nen thu gian, tranh caffeine de ngu tot."};
        }
    });

    R.push_back({
        "STOMACH_SAFE",
        [](const Context& c) { return c.stomachIssue; },
        [](const Context& c) {
            return Recommendation{"Sua am hoac nuoc am", 7,
                "Da day nhay cam: tranh do chua va caffeine manh."};
        }
    });

    R.push_back({
        "HUNGRY_JUICE",
        [](const Context& c) { return c.hungry >= 5 && !c.stomachIssue; },
        [](const Context& c) {
            return Recommendation{"Nuoc cam", 6,
                "Ban hoi doi, bo sung vitamin C giup tinh tao nhe."};
        }
    });

    R.push_back({
        "BUDGET_WATER",
        [](const Context& c) { return c.budgetLow || c.needFocus <= 3; },
        [](const Context& c) {
            return Recommendation{"Nuoc loc", 5,
                "Giai phap tot-don gian-tiet kiem, phu hop moi luc."};
        }
    });

    R.push_back({
        "SLEEP_LOW_TEA",
        [](const Context& c) { return c.timeOfDay == MORNING && c.sleptLt6h && c.caffeineSensitive; },
        [](const Context& c) {
            return Recommendation{"Tra am", 6,
                "Thieu ngu nhung nhay caffeine: tra am nhe nhang hon ca phe."};
        }
    });

    R.push_back({
        "SLEEP_LOW_COFFEE_MILK",
        [](const Context& c) { return c.timeOfDay == MORNING && c.sleptLt6h && !c.caffeineSensitive; },
        [](const Context& c) {
            return Recommendation{"Ca phe sua", 7,
                "Thieu ngu sang som: them chut sua cho em da."};
        }
    });

    R.push_back({
        "FALLBACK_WATER",
        [](const Context& c) { return true; },
        [](const Context& c) {
            return Recommendation{"Nuoc loc", 4,
                "Luon la lua chon lanh manh va an toan."};
        }
    });

    return R;
}

// ======= MAY SUY LUAN =======
InferResult infer(const Context& ctx, const vector<Rule>& rules, int topK) {
    vector<string> trace;
    vector<Recommendation> fired;

    for (const auto& r : rules) {
        try {
            if (r.when(ctx)) {
                Recommendation rec = r.then(ctx);
                fired.push_back(rec);
                trace.push_back("Kich hoat " + r.name + " -> +" + to_string(rec.score) + " diem -> " + rec.label);
            }
        } catch (exception& ex) {
            trace.push_back("[WARN] Loi luat " + r.name + ": " + ex.what());
        }
    }

    unordered_map<string, Agg> agg;
    for (auto& rec : fired) {
        Agg& a = agg[rec.label];
        a.totalScore += rec.score;
        a.addExplanation(rec.explanation);
    }

    vector<Ranked> ranked;
    for (auto& kv : agg) {
        ranked.push_back({kv.first, kv.second.totalScore, kv.second.explanations});
    }

    sort(ranked.begin(), ranked.end(), [](const Ranked& x, const Ranked& y) {
        if (x.totalScore != y.totalScore) return x.totalScore > y.totalScore;
        return x.label < y.label;
    });

    if ((int)ranked.size() > topK) ranked.resize(topK);

    return {ranked, trace};
}

// ======= HOI THONG TIN NGUOI DUNG =======
TimeOfDay askTimeOfDay() {
    while (true) {
        cout << "Bay gio la buoi (m/a/n) [morning/afternoon/night]: " << endl;
        string s; getline(cin, s);
        for (auto& c : s) c = tolower(c);
        if (s == "m") return MORNING;
        if (s == "a") return AFTERNOON;
        if (s == "n") return NIGHT;
        cout << "  -> Chon m/a/n nhe." << endl;
    }
}

int askInt(const string& prompt, int lo, int hi) {
    while (true) {
        cout << prompt << " [" << lo << "-" << hi << "]: " << endl;
        string s; getline(cin, s);
        try {
            int v = stoi(s);
            if (v >= lo && v <= hi) return v;
        } catch (...) {}
        cout << "  -> Nhap so hop le trong [" << lo << "," << hi << "]." << endl;
    }
}

bool askYesNo(const string& prompt) {
    while (true) {
        cout << prompt << " (y/n): " << endl;
        string s; getline(cin, s);
        for (auto& c : s) c = tolower(c);
        if (s == "y" || s == "yes") return true;
        if (s == "n" || s == "no") return false;
        cout << "  -> Tra loi y/n giup minh nhe." << endl;
    }
}

Context collectContext() {
    cout << "\nTra loi nhanh vai cau de goi y do uong:\n";
    Context c;
    c.timeOfDay = askTimeOfDay();
    c.needFocus = askInt("Ban can tap trung o muc nao", 0, 10);
    c.hotWeather = askYesNo("Troi nong/oi buc?");
    c.justExercised = askYesNo("Vua van dong/do mo hoi?");
    c.caffeineSensitive = askYesNo("Ban nhay voi caffeine?");
    c.sleptLt6h = askYesNo("Dem qua ngu < 6 tieng?");
    c.hungry = askInt("Muc doi hien tai", 0, 10);
    c.stomachIssue = askYesNo("Da day dang kho chiu/dau?");
    c.budgetLow = askYesNo("Ban muon tiet kiem chi phi toi da?");
    return c;
}

// ======= MAIN =======
int main() {
    cout << "=== HE CHUYEN GIA MINI: CHON DO UONG (C++) ===" << endl;
    vector<Rule> rules = buildKnowledgeBase();

    Context ctx = collectContext();
    InferResult res = infer(ctx, rules, 3);

    cout << "\n--- TRACE MAY SUY LUAN ---\n";
    for (auto& t : res.trace) {
        cout << "• " << t << "\n";
    }

    cout << "\n=== GOI Y HANG DAU ===\n";
    int i = 1;
    for (auto& r : res.ranked) {
        cout << i << ". " << r.label << "  [diem: " << r.totalScore << "]\n";
        for (auto& e : r.explanations) {
            cout << "   - " << e << "\n";
        }
        i++;
    }

    cout << "\nMeo: thu doi thong so (ban dem, da day yeu, can tap trung cao...) de thay he doi ket qua.\n";

    return 0;
}
