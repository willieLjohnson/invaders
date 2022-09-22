#include "raylib.h"
#include <math.h>

// Player
typedef struct Player {
  Rectangle rect;
  Vector2 speed;
  Color color;
} Player;

// Projectiles
typedef struct Bullet {
  Rectangle rect;
  Vector2 speed;
  bool active;
  Color color;
} Bullet;

// Entities
typedef struct Enemy {
  Rectangle rect;
  Vector2 speed;
  bool active;
  Color color;
} Enemy;

#define MAX_BULLETS 50
#define MAX_ENEMIES 50

// Screen
static int screenWidth = 800;
static int screenHeight = 450;
// Player
static Player player;

// Bullets
static Bullet bullets[MAX_BULLETS];
static int fireRate;

// Entities
static Enemy enemies[MAX_ENEMIES];
static int activeEnemies;

// SFX
static Sound sfxShoot;
static Sound sfxExplode;

// Engine
static void InitGame(void);
static void UpdateGame(void);
static void DrawGame(void);
static void UnloadGame(void);

int main() {
  InitWindow(screenWidth, screenHeight, "INVADERS!");
  InitAudioDevice();
  InitGame();
  SetTargetFPS(60);

  // Main loop
  while (!WindowShouldClose()) {
    UpdateGame();
    DrawGame();
  }

  UnloadGame();

  CloseWindow();
  return 0;
}

void InitGame(void) {
  // Bullets and firerate
  fireRate = 0;
  // Enemies
  activeEnemies = 10;
  // Sound
  sfxShoot = LoadSound("shoot.ogg");
  sfxExplode = LoadSound("explode.ogg");

  // Init player
  player.rect.x = screenWidth / 2.0f;
  player.rect.y = screenHeight - 20;
  player.rect.width = 20;
  player.rect.height = 20;
  player.speed.x = 5;
  player.speed.y = 5;
  player.color = BLUE;

  for (int i = 0; i < MAX_ENEMIES; i++) {
    enemies[i].rect.width = 20;
    enemies[i].rect.height = 20;
    enemies[i].rect.x = GetRandomValue(0, screenWidth);
    enemies[i].rect.y = GetRandomValue(-screenHeight, -20);
    enemies[i].speed.x = 5;
    enemies[i].speed.y = 5;
    enemies[i].active = true;
    enemies[i].color = MAROON;
  }
  // Initialize Bullets
  for (int i = 0; i < MAX_BULLETS; i++) {
    bullets[i].rect.x = player.rect.x;
    bullets[i].rect.y = player.rect.y + player.rect.height / 4;
    bullets[i].rect.width = 5;
    bullets[i].rect.height = 10;
    bullets[i].speed.x = 0;
    bullets[i].speed.y = -10;
    bullets[i].active = false;
    bullets[i].color = BLUE;
  }
}

void UpdateGame(void) {
  // Movement
  if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
    player.rect.x += player.speed.x;
  if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
    player.rect.x -= player.speed.x;

  // Firing
  if (IsKeyDown(KEY_SPACE)) {
    fireRate += 5;
    for (int i = 0; i < MAX_BULLETS; i++) {
      if (!bullets[i].active && fireRate % 40 == 0) {
        PlaySound(sfxShoot);
        bullets[i].rect.x = player.rect.x;
        bullets[i].rect.y = player.rect.y + player.rect.height / 4;
        bullets[i].active = true;
        break;
      }
    }
  }

  // Player wall collision
  if (player.rect.x <= 0)
    player.rect.x = 0;
  if (player.rect.x + player.rect.width >= screenWidth)
    player.rect.x = screenWidth - player.rect.width;

  // Enemy movement
  for (int i = 0; i < activeEnemies; i++) {
    if (enemies[i].active) {
      enemies[i].rect.y += enemies[i].speed.y;
      // reset enemy at bottom of screen
      if (enemies[i].rect.y > screenHeight) {
        enemies[i].rect.x = GetRandomValue(0, screenWidth);
        enemies[i].rect.y = GetRandomValue(-screenHeight, -20);
      }
    }
  }

  // Bullet collision
  for (int i = 0; i < MAX_BULLETS; i++) {
    if (bullets[i].active) {
      bullets[i].rect.y += bullets[i].speed.y;
      // Collision with Enemy
      for (int j = 0; j < activeEnemies; j++) {
        if (enemies[j].active) {
          if (CheckCollisionRecs(bullets[i].rect, enemies[j].rect)) {
            bullets[i].active = false;
            enemies[j].rect.x = GetRandomValue(screenWidth, screenWidth + 1000);
            enemies[j].rect.y =
                GetRandomValue(0, screenHeight - enemies[j].rect.height);
            fireRate = 0;
            PlaySound(sfxExplode);
            // enemiesKilled++;
            // score += 5;
          }
          // Top of screen
          if (bullets[i].rect.y <= 0) {
            bullets[i].active = false;
            fireRate = 0;
          }
        }
      }
    }
  }
}

void DrawGame(void) {
  BeginDrawing();
  ClearBackground(BLACK);
  // Draw player
  DrawRectangleRec(player.rect, player.color);
  // Draw enemies
  for (int i = 0; i < activeEnemies; i++) {
    if (enemies[i].active) {
      DrawRectangleRec(enemies[i].rect, enemies[i].color);
    }
  }

  // Draw Bullets
  for (int i = 0; i < MAX_BULLETS; i++) {
    if (bullets[i].active)
      DrawRectangleRec(bullets[i].rect, bullets[i].color);
  }
  EndDrawing();
}

void UnloadGame(void) {
  UnloadSound(sfxShoot);
  UnloadSound(sfxExplode);
}
