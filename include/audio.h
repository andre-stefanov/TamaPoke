#pragma once
#include <stdint.h>

// Game sound effects (queued, non-blocking). The order matches the SFX
// table in audio.cpp.
enum Sfx : uint8_t {
  SFX_TAP = 0,  // tap / button
  SFX_EAT,      // eating
  SFX_PLAY,     // minigame point / hit
  SFX_HEART,    // likes it / petting
  SFX_HATCH,    // hatching
  SFX_EVOLVE,   // evolution
  SFX_MEDAL,    // medal / milestone
  SFX_DENY,     // action not allowed
  SFX_BYE,      // farewell
  SFX_LEVEL,    // level up
  SFX_BATTLE_WIN,
  SFX_BATTLE_LOSS,
  SFX_CATCH_OK,
  SFX_CATCH_FAIL,
  SFX_DAILY_GOAL,
  SFX_EVENT_SPARKLE,
  SFX_REST,
  SFX_COUNTER,
  SFX_MENU,
  SFX_GAME_START,
  SFX_BALL_BOUNCE,
  SFX_BALL_MISS,
  SFX_MEMO_STEP,
  SFX_MEMO_PAD_0,
  SFX_MEMO_PAD_1,
  SFX_MEMO_PAD_2,
  SFX_MEMO_PAD_3,
  SFX_ATTACK_QUICK,
  SFX_ATTACK_HEAVY,
  SFX_ENEMY_HIT,
  SFX_EFFECTIVE,
  SFX_WEAK_HIT,
  SFX_MINIGAME_OK,
  SFX_MINIGAME_BAD,
  SFX_LOW_HP,
  SFX_EXPEDITION_START,
  SFX_EXPEDITION_FOUND,
  SFX_EXPEDITION_CLAIM,
  SFX_ITEM_USE,
  SFX_COUNT
};

enum SoundMode : uint8_t {
  SOUND_OFF = 0,
  SOUND_LOW,
  SOUND_MED,
  SOUND_FULL,
};

void audioBegin();          // init ES8311 + I2S + amplifier + audio task
void sfxPlay(uint8_t id);   // queues an effect (does not block the loop)
void audioSetEnabled(bool on);
bool audioEnabled();
void audioSetMode(uint8_t mode);
uint8_t audioMode();
bool audioBusy();
void speciesChirpPlay(int16_t dex, bool force = false);  // custom, synthesized species chirp
