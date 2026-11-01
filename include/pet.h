#pragma once
#include <Arduino.h>
#include <Preferences.h>
#include "time_utils.h"
#include "dex.h"

// 1 tick = 1 minute of game time. Lower this value to test faster
// (e.g. 5000UL = stats drop 12x faster).
#define PET_TICK_MS 60000UL
// Game minutes per level. With 60, CHARMANDER evolves at ~16 h
// of play with perfect care. Lower to 1 to see evolutions instantly.
#define MINUTES_PER_LEVEL 60
#define EAT_ANIM_MS 2500UL
#define HEART_MS 1500UL
#define EVOLVE_ANIM_MS 5200UL              // evolution animation (longer = more epic)
#define CEREMONY_MS 10000UL                // duration of the farewell on screen
#define FAREWELL_AGE_MIN (3UL * 24 * 60)   // says farewell after 3 days of play (final form)
#define RUNAWAY_TICKS 60                   // runs away after 1 h with EVERYTHING at zero

// end-of-cycle ceremonies
enum : uint8_t { CER_NONE = 0, CER_FAREWELL, CER_RUNAWAY, CER_RELEASE };

enum PetMood : uint8_t { MOOD_HAPPY, MOOD_SAD, MOOD_EATING, MOOD_SLEEPING };
enum PetEventType : uint8_t { PET_EVENT_BERRY = 0, PET_EVENT_HEART, PET_EVENT_SPARKLE };
enum PetPersonality : uint8_t {
  PERS_BALANCED = 0,
  PERS_PLAYFUL,
  PERS_BRAVE,
  PERS_CALM,
  PERS_LAZY,
};

enum PetInteractResult : uint8_t {
  PET_INTERACT_NONE = 0,
  PET_INTERACT_JOY = 1 << 0,
  PET_INTERACT_BOND = 1 << 1,
  PET_INTERACT_ENERGY = 1 << 2,
};

enum DailyGoalType : uint8_t {
  DAILY_GOAL_CARE = 0,
  DAILY_GOAL_PLAY,
  DAILY_GOAL_BATTLE,
  DAILY_GOAL_CATCH,
  DAILY_GOAL_MEMO,
};
#define DAILY_GOAL_COUNT 3

enum ExpeditionItem : uint8_t {
  EXP_ITEM_SNACK = 0,
  EXP_ITEM_ENERGY,
  EXP_ITEM_CARE,
  EXP_ITEM_TRAIN,
  EXP_ITEM_NONE = 0xFF,
};
#define EXP_ITEM_COUNT 4
#define EXP_ITEM_MAX 3

// Rewards of the daily step milestones. The values are only
// stored briefly for the display; the actual progress lives
// in stepDailyRewardMask/stepMilestoneMask.
enum StepRewardEvent : uint8_t {
  STEP_REWARD_NONE = 0,
  STEP_REWARD_SNACK,
  STEP_REWARD_ENERGY,
  STEP_REWARD_TRAIN,
  STEP_REWARD_TRAIL_RANK,
};
#define STEP_DAILY_GOAL_COUNT 3

// State for the small expedition hint on the main screen.
enum ExpeditionHudState : uint8_t {
  EXP_HUD_HIDDEN = 0,
  EXP_HUD_ACTIVE,
  EXP_HUD_READY,
  EXP_HUD_BAG,
};

enum TrainingStat : int8_t {
  TRAIN_STAT_ATK = 0,
  TRAIN_STAT_DEF,
  TRAIN_STAT_SPE,
};

// medals of the individual (bitmask)
enum : uint16_t {
  MED_LV10 = 1 << 0, MED_LV25 = 1 << 1, MED_LV50 = 1 << 2,
  MED_BERRY = 1 << 3, MED_STREAK7 = 1 << 4, MED_BOND = 1 << 5,
  MED_FINAL = 1 << 6, MED_FIT = 1 << 7,
};
#define MED_COUNT 8

enum BattleRewardStat : uint8_t {
  BATTLE_REWARD_NONE = 0,
  BATTLE_REWARD_ATK,
  BATTLE_REWARD_DEF,
  BATTLE_REWARD_SPE,
};

struct BattleReward {
  BattleRewardStat stat = BATTLE_REWARD_NONE;
  uint8_t amount = 0;
};

class Pet {
public:
  // Stats 0..100
  uint8_t fullness = 80;  // FOOD
  uint8_t joy = 80;       // JOY
  uint8_t energy = 80;    // energy
  uint8_t hygiene = 100;  // hygiene
  uint8_t poops = 0;      // poops on screen (max 3)
  uint8_t weight = 0;     // 0-100: candies fatten, the minigame burns it off
  // genes (90-110%, rolled at hatch) and training (0-100)
  uint8_t geneAtk = 100, geneDef = 100, geneSpe = 100;
  uint8_t trAtk = 0, trDef = 0, trSpe = 0;
  bool berryKnown = false;  // already discovered its favorite berry
  bool shiny = false;       // rare color variant (rolled in the egg)
  uint32_t ageMinutes = 0;
  int16_t speciesId = -1;      // Pokedex number (1-DEX_COUNT), -1 = egg
  int16_t prevSpeciesId = -1;  // for the evolution animation
  uint8_t careMistakes = 0;   // care mistakes: each one delays evolution by 1 level
  bool sleeping = false;
  uint32_t lastSeenEpoch = 0;   // last RTC time seen (for offline progression)
  uint8_t ceremony = CER_NONE;  // farewell/runaway/release in progress
  uint8_t lastEnd = CER_NONE;   // how the previous one ended (affects the egg)
  uint8_t dexReg[DEX_BITMAP_BYTES] = { 0 };       // pokedex of raised pets
  uint8_t dexShinyReg[DEX_BITMAP_BYTES] = { 0 };  // shiny raised or caught
  uint8_t dexCaught[DEX_BITMAP_BYTES] = { 0 };    // pokedex of caught wild ones
  // daily care streak (of the player: persists across pet life cycle runs)
  uint16_t streak = 0, bestStreak = 0;
  uint32_t lastCareDay = 0;
  // bond (of the pet: rises slowly with care, resets when another is born)
  uint8_t bond = 0;
  char nick[12] = "";    // nickname (empty = species name)
  // medals: of the individual + cumulative counter across all runs
  uint16_t medals = 0, totalMedals = 0;
  uint16_t newMedal = 0;   // newly earned one(s), to celebrate
  uint16_t lastMilestone = 0;  // streak milestone already celebrated
  uint16_t gameHi = 0;     // minigame record (of the player)
  uint16_t strHi = 0;      // record of hits on the training bag
  uint16_t catchHi = 0;    // catch minigame record
  uint16_t memoHi = 0;     // memo minigame record
  uint16_t cleanHi = 0;    // clean minigame record
  uint16_t typeHi = 0;     // type match minigame record
  uint16_t battleWins = 0, battleLosses = 0;
  uint16_t battleStreak = 0, bestBattleStreak = 0;
  uint8_t collectionFrame = 0;  // 0=basic, further frames via dex milestones
  uint32_t lastPetInteractMinute = 0;
  uint16_t dexRewardMask = 0;
  uint32_t dailyGoalDay = 0;
  uint8_t dailyGoalType[DAILY_GOAL_COUNT] = { DAILY_GOAL_CARE, DAILY_GOAL_PLAY, DAILY_GOAL_CATCH };
  uint8_t dailyGoalProgress[DAILY_GOAL_COUNT] = { 0, 0, 0 };
  uint8_t dailyGoalDone = 0;
  uint8_t itemCounts[EXP_ITEM_COUNT] = { 0 };
  uint32_t expeditionEndEpoch = 0;
  uint8_t expeditionRewardItem = EXP_ITEM_NONE;
  // Player step progress: today's value resets on the RTC day,
  // the total value is kept across eggs and new Pokemon.
  uint32_t stepsToday = 0;
  uint32_t stepsTotal = 0;
  uint32_t stepDay = 0;
  uint8_t stepDailyRewardMask = 0;
  uint8_t stepMilestoneMask = 0;
  bool saveLoadedFromNvs = false;
  bool saveCreatedThisBoot = false;

  void begin();                 // loads NVS state (or creates the first egg)
  bool update(uint32_t nowMs);  // true when the pet state advanced

  // Actions (touch buttons)
  void feed();              // red berry (compatibility)
  void feedBerry(uint8_t color);  // 0 red, 1 blue, 2 green
  void feedCandy();
  bool lovesBerry(uint8_t color) const {
    return !isEgg() && (speciesId % 3) == color;  // hidden taste per species
  }
  void playResult(uint8_t score);  // minigame reward (trains SPE)
  uint8_t applyCatchResult(uint8_t score);
  uint8_t applyMemoResult(uint8_t rounds);
  uint8_t applyCleanResult(uint8_t score);
  uint8_t applyTypeResult(uint8_t score);
  bool applyPetEvent(uint8_t eventType);
  uint8_t interactPet(bool eveningBonus);
  bool applyShake();
  uint8_t applyWalk(uint16_t steps);
  uint32_t stepGoal(uint8_t index) const;
  bool stepGoalComplete(uint8_t index) const;
  uint8_t stepTrailRank() const;
  uint16_t stepShinyChancePer4096() const;
  uint8_t stepCatchBonus() const;
  bool showStepReward() const { return deadlineActive(millis(), stepRewardUntil); }
  uint8_t lastStepReward() const { return lastStepRewardEvent; }
  bool takeMorningGreeting();
  PetPersonality personality() const;
  void ensureDailyGoals();
  uint8_t dailyGoalTarget(uint8_t goalType) const;
  bool dailyGoalComplete(uint8_t index) const;
  uint8_t trainStrength(uint16_t hits);  // training bag (trains ATK)
  BattleReward applyBattleWin(int16_t wildDex, bool closeWin);
  void applyBattleLoss();

  // Expeditions: the prize is rolled and saved on exit so that a restart
  // cannot repeat the roll. The rolls are injectable for native tests.
  static uint8_t expeditionEnergyCost(uint8_t minutes);
  bool expeditionActive(uint32_t nowEpoch) const;
  bool expeditionReady(uint32_t nowEpoch) const;
  uint8_t expeditionItemCount() const;
  ExpeditionHudState expeditionHudState(uint32_t nowEpoch) const;
  bool expeditionInventoryFull() const;
  bool canStartExpedition(uint8_t minutes, uint32_t nowEpoch) const;
  uint8_t expeditionTrainingChance(uint8_t minutes) const;
  bool startExpedition(uint8_t minutes, uint32_t nowEpoch, uint8_t luckRoll, uint8_t itemRoll = 0);
  ExpeditionItem claimExpedition(uint32_t nowEpoch);
  bool useExpeditionItem(ExpeditionItem item, int8_t trainingStat = -1);

  // combat stats: real gen 1 base x genes + level + training
  uint16_t atkStat() const;
  uint16_t defStat() const;
  uint16_t speStat() const;
  void play();
  void toggleLight();  // sleep / wake
  void clean();
  void caress();  // touch the pet
  void eggTap();  // touch the egg: 3 taps and it hatches
  void newEgg();   // start over with a random starter
  void release();  // release (long press + confirm)
  void syncClock(uint32_t nowEpoch);  // applies the time elapsed while switched off
  void setClock(uint32_t nowEpoch);   // sets the time without applying progression
  void startFarewell();  // also usable from the serial console (BYE)
  void startRunaway();   // also usable from the serial console (RUN)

  bool isEgg() const { return speciesId < 0; }
  uint8_t eggCracks() const { return eggTaps; }
  bool eating() const { return deadlineActive(millis(), eatUntil); }
  bool showHeart() const { return deadlineActive(millis(), heartUntil); }
  bool evolving() const { return deadlineActive(millis(), evolveUntil); }
  float evolveT() const {     // evolution animation progress 0..1
    uint32_t n = millis();
    uint32_t left = deadlineRemaining(n, evolveUntil);
    return 1.0f - (float)left / (float)EVOLVE_ANIM_MS;
  }
  bool evolutionUnlocked() const;  // level/condition: at least one form available
  bool canEvolveNow() const;  // ready: unlocked + awake + 3 of 4 stats > 40
  uint8_t evolutionOptionCount() const;
  int16_t evolutionOption(uint8_t index) const;
  uint8_t evolutionRequiredLevel() const;
  uint8_t evolutionRequiredLevelFor(int16_t target) const;
  bool canEvolveTo(int16_t target) const;
  void evolveTo(int16_t target);  // transforms into a chosen target
  void evolve();                  // compatibility: picks an available option
  bool canFarewellNow() const;  // final form + 7 days: ready for farewell (button)
  bool canRunawayNow() const;   // total neglect 1h: ready to run away (sad button)
  // the user decides in a dialog; "keep/stay" postpones and offers again later
  bool wantEvolveButton() const;
  bool wantFarewellButton() const { return canFarewellNow() && ageMinutes >= farDeclinedAge; }
  void declineEvolve();
  void resetEvolutionDeferral();
  void declineFarewell() { farDeclinedAge = ageMinutes + 1440; } // offers again within 1 day
  // first run: the player picks a starter (Bulbasaur/Charmander/Squirtle)
  bool awaitingStarter() const { return starterPick; }
  void chooseStarter(int16_t dex) { eggTarget = dex; starterPick = false; save(); }
  void factoryReset() { prefs.clear(); }  // wipes NVS (test: serial command WIPE)
  void dbgRunawayReady() { fullness = joy = energy = hygiene = 0; neglectTicks = RUNAWAY_TICKS; }  // test
  uint8_t level() const {
    uint32_t raw = 1UL + ageMinutes / MINUTES_PER_LEVEL;
    return raw > 100UL ? 100 : (uint8_t)raw;
  }
  bool isRegistered(int16_t dex) const {
    return dex >= 1 && dex <= DEX_COUNT && (dexReg[(dex - 1) >> 3] & (1 << ((dex - 1) & 7)));
  }
  bool isCaught(int16_t dex) const {
    return dex >= 1 && dex <= DEX_COUNT && (dexCaught[(dex - 1) >> 3] & (1 << ((dex - 1) & 7)));
  }
  bool isShinyRegistered(int16_t dex) const {
    return dex >= 1 && dex <= DEX_COUNT && (dexShinyReg[(dex - 1) >> 3] & (1 << ((dex - 1) & 7)));
  }
  uint16_t registeredCount() const;
  uint16_t caughtCount() const;
  uint16_t knownDexCount() const;
  uint8_t collectionRank() const;
  uint8_t unlockedCollectionFrameCount() const;
  bool setCollectionFrame(uint8_t frame);
  void registerCaught(int16_t dex, bool shinyVariant = false);
  uint16_t nextDexGoal() const;
  uint16_t applyDexRewards();
  uint8_t catchChanceForWild(int16_t wildDex, uint8_t wildLevel, uint8_t petLevel, bool closeWin) const;
  uint8_t respectCatchChanceForWild(int16_t wildDex, uint8_t wildLevel, uint8_t petLevel) const;
  bool tryCatchWild(int16_t wildDex, uint8_t wildLevel, uint8_t petLevel, bool closeWin,
                    uint8_t luckRoll, bool shinyVariant = false);
  bool tryRespectCatchWild(int16_t wildDex, uint8_t wildLevel, uint8_t petLevel,
                           uint8_t luckRoll, bool shinyVariant = false);
  bool lineHasUnregistered(int16_t base) const;
  bool hasEvolutionPath(int16_t dex) const;
  uint8_t eggRarity() const;       // rarity of the current egg (species kept hidden)
  int16_t pickEggSpecies();        // public so rolls can be simulated (EGGS)
  uint8_t healthyStatCount() const {
    return (uint8_t)((fullness > 40 ? 1 : 0) + (joy > 40 ? 1 : 0) +
                     (energy > 40 ? 1 : 0) + (hygiene > 40 ? 1 : 0));
  }
  uint8_t lowestStat() const { return min(min(fullness, joy), min(energy, hygiene)); }
  PetMood mood() const;
  // farewell/runaway ceremony progress, 0..1 (to animate it)
  float ceremonyT() const {
    if (ceremony == CER_NONE) return 0.0f;
    uint32_t n = millis();
    uint32_t left = deadlineRemaining(n, ceremonyUntil);
    return 1.0f - (float)left / (float)CEREMONY_MS;
  }

  // streak / bond / medals / name
  void rename(const char *name);
  bool hasMedal(uint16_t m) const { return medals & m; }
  bool showMedal() const { return deadlineActive(millis(), medalUntil); }
  bool showMilestone() const { return deadlineActive(millis(), milestoneUntil); }
  bool showDexReward() const { return deadlineActive(millis(), dexRewardUntil); }
  uint16_t lastDexRewardGoal() const { return lastDexReward; }
  int careBonus() const;  // egg bonus from streak + bond

  // deferred periodic save: tick() marks pending and the loop flushes it
  // when the screen is dimmed/off (the flash write freezes
  // both cores for ~1s: that way it is not seen and touch is not cut)
  bool savePending() const { return pendingSave; }
  void flushSave();
  // Versioned game-state backup for the browser USB tool; never includes SD data.
  static constexpr size_t BACKUP_MAX_BYTES = 512;
  size_t exportBackup(uint8_t *out, size_t capacity) const;
  bool importBackup(const uint8_t *data, size_t length);

private:
  Preferences prefs;
  uint32_t lastTick = 0;
  uint32_t eatUntil = 0;
  uint32_t heartUntil = 0;
  uint32_t evolveUntil = 0;
  int16_t eggTarget = 1;       // hidden dex that will come out of the egg
  bool eggShiny = false;       // surprise rolled when creating the egg
  uint8_t eggTaps = 0;
  uint8_t mistakeCooldown = 0;
  uint8_t ticksSinceSave = 0;
  bool pendingSave = false;     // periodic save pending flush
  uint8_t evoDeclinedLv = 0;    // "keep form": do not offer until the next level
  uint32_t evoDeclinedAge = 0;  // at Lv.100: offer again from this age on
  uint32_t farDeclinedAge = 0;  // "stay together": do not offer farewell until this age
  bool starterPick = false;     // first run: waiting for the player to pick a starter
  uint8_t neglectTicks = 0;
  uint16_t goodTicks = 0;  // well cared streak: forges DEF
  uint32_t ceremonyUntil = 0;
  uint8_t bondToday = 0;       // daily cap for bond gain
  uint32_t medalUntil = 0;     // medal celebration on screen
  uint32_t milestoneUntil = 0; // streak milestone celebration
  uint32_t dexRewardUntil = 0;
  uint16_t lastDexReward = 0;
  uint32_t lastMorningDay = 0;
  uint32_t shakeReadyAt = 0;
  uint32_t shakeDay = 0;
  uint8_t shakeCountToday = 0;
  uint16_t walkJoyBank = 0;
  uint16_t walkBondBank = 0;
  uint32_t walkDay = 0;
  uint32_t walkHour = 0;
  uint8_t walkJoyToday = 0;
  uint8_t walkJoyHour = 0;
  uint8_t walkBondToday = 0;
  uint32_t stepRewardUntil = 0;
  uint8_t lastStepRewardEvent = STEP_REWARD_NONE;
  uint8_t pendingStepRewardMask = 0;

  uint32_t today() const { return lastSeenEpoch ? lastSeenEpoch / 86400 : 0; }
  void registerCare();   // first care of the day: streak + bond
  void addBond(uint8_t amt);
  void noteDailyGoal(uint8_t goalType, uint8_t amount);
  void applyDailyReward();
  void recordStepReward(uint8_t index);
  void applyPendingStepRewards();
  void ensureStepDay();
  void checkMedals();
  void tick();
  void hatch();
  void registerSpecies(int16_t dex);
  bool canReceiveExpeditionItem(ExpeditionItem item) const;
  void save();
  void load();
  static uint8_t clamp100(int v) { return v < 0 ? 0 : (v > 100 ? 100 : v); }
};
