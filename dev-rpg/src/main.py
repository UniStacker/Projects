from datetime import datetime, timedelta
from collections import defaultdict

# ----------------------------
# XP RULES (Minimal v1)
# ----------------------------

XP_RULES = {
    "merged_pr_base": 15,
    "merged_pr_loc_bonus": 50,   # 1 XP per 50 LOC (capped)
    "merged_pr_loc_cap": 30,

    "commits_per_chunk": 5,      # every 5 valid commits → XP
    "commit_chunk_xp": 10,
    "commit_min_loc": 5,         # ignore commits < 5 LOC

    "review_xp": 10,             # actionable review

    "leetcode_easy": 5,
    "leetcode_medium": 10,
    "leetcode_hard": 15,
    "leetcode_contest_bonus": 10,

    "xp_per_level": 100,
    "decay_days": 30,
    "decay_percent": 0.05,
    "streak_bonus": 0.05,
    "streak_cap": 0.15,
}

# ----------------------------
# CHARACTER SHEET
# ----------------------------

class Character:
    def __init__(self, name):
        self.name = name
        self.stats = defaultdict(int)   # XP storage per stat
        self.levels = defaultdict(int)  # cached levels
        self.last_activity = defaultdict(lambda: datetime.now())
        self.streak_days = 0
        self.rating_ps = 0  # Problem-Solving rating (from LC/CF)

    # -------------------
    # APPLY XP
    # -------------------
    def add_xp(self, stat, xp):
        if xp <= 0:
            return
        # streak bonus
        streak_multiplier = 1.0 + min(self.streak_days // 7 * XP_RULES["streak_bonus"], XP_RULES["streak_cap"])
        gained = int(xp * streak_multiplier)
        self.stats[stat] += gained
        self.last_activity[stat] = datetime.now()
        self.levels[stat] = self.stats[stat] // XP_RULES["xp_per_level"]

    # -------------------
    # DECAY SYSTEM
    # -------------------
    def apply_decay(self):
        now = datetime.now()
        for stat, xp in self.stats.items():
            if (now - self.last_activity[stat]).days >= XP_RULES["decay_days"]:
                lost = int(xp * XP_RULES["decay_percent"])
                self.stats[stat] = max(0, xp - lost)
                self.levels[stat] = self.stats[stat] // XP_RULES["xp_per_level"]

    # -------------------
    # STATUS WINDOW
    # -------------------
    def status(self):
        global_level = sum(self.levels.values()) // max(1, len(self.levels))
        lines = [f"[{self.name} — Global Lv {global_level} | R_ps: {self.rating_ps}]"]
        for stat in ["Languages", "Problem-Solving", "Execution", "Collaboration"]:
            xp = self.stats[stat]
            lv = self.levels[stat]
            bar = ("█" * (lv % 10)).ljust(10, "░")
            lines.append(f"{stat:<15} Lv {lv} {bar} ({xp} XP)")
        lines.append(f"Streak: {self.streak_days} days")
        return "\n".join(lines)

# ----------------------------
# SCORING HELPERS
# ----------------------------

def score_github_pr(char: Character, loc_changed: int):
    base = XP_RULES["merged_pr_base"]
    bonus = min(loc_changed // XP_RULES["merged_pr_loc_bonus"], XP_RULES["merged_pr_loc_cap"])
    char.add_xp("Execution", base + bonus)

def score_github_commits(char: Character, commits):
    valid = [c for c in commits if c["loc"] >= XP_RULES["commit_min_loc"]]
    chunks = len(valid) // XP_RULES["commits_per_chunk"]
    if chunks:
        char.add_xp("Languages", chunks * XP_RULES["commit_chunk_xp"])

def score_github_review(char: Character, actionable=True):
    if actionable:
        char.add_xp("Collaboration", XP_RULES["review_xp"])

def score_leetcode_problem(char: Character, difficulty: str):
    xp = XP_RULES.get(f"leetcode_{difficulty.lower()}", 0)
    char.add_xp("Problem-Solving", xp)

def score_leetcode_contest(char: Character, delta_rating: int):
    char.rating_ps += delta_rating
    char.add_xp("Problem-Solving", XP_RULES["leetcode_contest_bonus"])

# ----------------------------
# DEMO (fake data)
# ----------------------------
if __name__ == "__main__":
    shan = Character("Shan")
    
    # Simulate GitHub actions
    score_github_pr(shan, loc_changed=230)   # PR with 230 LOC
    score_github_commits(shan, commits=[{"loc": 10}, {"loc": 3}, {"loc": 25}, {"loc": 50}, {"loc": 6}])
    score_github_review(shan)

    # Simulate LeetCode
    score_leetcode_problem(shan, "medium")
    score_leetcode_problem(shan, "hard")
    score_leetcode_problem(shan, "hard")
    score_leetcode_problem(shan, "hard")
    score_leetcode_problem(shan, "hard")
    score_leetcode_problem(shan, "hard")
    score_leetcode_problem(shan, "hard")
    score_leetcode_problem(shan, "hard")
    score_leetcode_problem(shan, "hard")
    score_leetcode_problem(shan, "hard")
    score_leetcode_contest(shan, delta_rating=42)

    # Show status
    print(shan.status())

