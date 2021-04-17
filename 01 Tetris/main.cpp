#include <SFML/Graphics.hpp>
#include <algorithm>
#include <array>
#include <ctime>
#include <iostream>

using namespace sf;

constexpr int kRowsNum = 20;
constexpr int kColumnsNum = 10;
constexpr int kFiguresNum = 7;

int field[kRowsNum][kColumnsNum] = {0};

constexpr int kBlockNumOfPiece = 4;
struct Point {
  int x, y;
};

using PointsType = std::array<Point, kBlockNumOfPiece>;
PointsType next_pos, tmp_pos;

constexpr std::array<PointsType, kFiguresNum> figures = {{
    {{{1, 0}, {1, 1}, {1, 2}, {1, 3}}},  // I
    {{{0, 1}, {0, 2}, {1, 2}, {1, 3}}},  // Z
    {{{1, 1}, {1, 2}, {0, 2}, {0, 3}}},  // S
    {{{1, 1}, {1, 2}, {0, 2}, {1, 3}}},  // T
    {{{0, 1}, {1, 1}, {1, 2}, {1, 3}}},  // L
    {{{1, 1}, {1, 2}, {1, 3}, {0, 3}}},  // J
    {{{0, 1}, {1, 1}, {0, 2}, {1, 2}}}   // O
}};

bool escapeButtonStatus = false;
bool pause = false;
unsigned score = 0;

bool check() {
  for (int i = 0; i < kBlockNumOfPiece; i++) {
    if (next_pos[i].x < 0 || next_pos[i].x >= kColumnsNum ||
        next_pos[i].y >= kRowsNum) {
      return false;
    }
    if (field[next_pos[i].y][next_pos[i].x]) {
      return false;
    }
  }
  return true;
};

int main() {
  srand(time(0));

  RenderWindow window(VideoMode(320, 480), "The Game!");

  Texture t1, t2, t3;
  t1.loadFromFile("images/tiles.png");
  t2.loadFromFile("images/background.png");
  t3.loadFromFile("images/frame.png");

  Sprite tiles_sprite(t1), background(t2), frame(t3);

  int dx = 0;
  bool rotate = 0;
  int colorNum = 1;
  float timer = 0, delay = 0.3;

  Clock clock;

  {
    const int firstFigure = rand() % kFiguresNum;
    for (int i = 0; i < kBlockNumOfPiece; i++) {
      next_pos[i].x = figures[firstFigure][i].x;
      next_pos[i].y = figures[firstFigure][i].y;
    }
  }

  while (window.isOpen()) {
    if (!pause || !escapeButtonStatus) timer += clock.restart().asSeconds();

    Event e;
    while (window.pollEvent(e)) {
      if (e.type == Event::Closed) window.close();
      if (e.type == Event::LostFocus) pause = true;
      if (e.type == Event::GainedFocus) pause = false;

      if (e.type == Event::KeyPressed) {
        switch (e.key.code) {
          case Keyboard::Escape:
            escapeButtonStatus = !escapeButtonStatus;
            break;
          case Keyboard::Up:
            rotate = true;
            break;
          case Keyboard::Left:
            dx = -1;
            break;
          case Keyboard::Right:
            dx = 1;
            break;
          default:;
        }
      }
    }

    if (pause || escapeButtonStatus) continue;

    if (Keyboard::isKeyPressed(Keyboard::Down)) delay = 0.05;

    //// <- Move -> ///
    for (int i = 0; i < kBlockNumOfPiece; i++) {
      tmp_pos[i] = next_pos[i];
      next_pos[i].x += dx;
    }
    if (!check()) next_pos = tmp_pos;

    //////Rotate//////
    if (rotate) {
      Point p = next_pos[1];  // center of rotation
      for (int i = 0; i < kBlockNumOfPiece; i++) {
        int x = next_pos[i].y - p.y;
        int y = next_pos[i].x - p.x;
        next_pos[i].x = p.x - x;
        next_pos[i].y = p.y + y;
      }
      if (!check()) next_pos = tmp_pos;
    }

    ///////Tick//////
    if (timer > delay) {
      // 1 shift down
      for (int i = 0; i < kBlockNumOfPiece; i++) {
        tmp_pos[i] = next_pos[i];
        next_pos[i].y += 1;
      }

      // can't go down further
      if (!check()) {
        // pin the piece
        for (int i = 0; i < kBlockNumOfPiece; i++) {
          field[tmp_pos[i].y][tmp_pos[i].x] = colorNum;
        }

        colorNum = 1 + rand() % kFiguresNum;
        // generate next rand piece
        int n = rand() % kFiguresNum;
        for (int i = 0; i < kBlockNumOfPiece; i++) {
          next_pos[i].x = figures[n][i].x;
          next_pos[i].y = figures[n][i].y;
        }
        // failed
        if (!check()) {
          window.close();
          break;
        }
      }

      timer = 0;
    }

    ///////check lines//////////
    int k = kRowsNum - 1;
    for (int i = kRowsNum - 1; i > 0; i--) {
      int count = 0;
      for (int j = 0; j < kColumnsNum; j++) {
        if (field[i][j]) {
          count++;
        }
        field[k][j] = field[i][j];
      }
      if (count < kColumnsNum) {
        k--;
      } else {
        // increase user's score
        ++score;
      }
    }
    if (k != 0) {
      std::fill(field[0], field[0] + (kColumnsNum * (k + 1)), 0);
    }

    dx = 0;
    rotate = 0;
    delay = 0.3;

    /////////draw//////////
    window.clear(Color::White);
    window.draw(background);

    auto draw_spirit = [&](int color, int x, int y) {
      tiles_sprite.setTextureRect(IntRect(color * 18, 0, 18, 18));
      tiles_sprite.setPosition(x * 18, y * 18);
      tiles_sprite.move(28, 31);  // offset
      window.draw(tiles_sprite);
    };

    for (int i = 0; i < kRowsNum; i++) {
      for (int j = 0; j < kColumnsNum; j++) {
        if (field[i][j] == 0) continue;
        draw_spirit(field[i][j], j, i);
      }
    }

    for (int i = 0; i < kBlockNumOfPiece; i++) {
      draw_spirit(colorNum, next_pos[i].x, next_pos[i].y);
    }

    window.draw(frame);
    window.display();
  }

  std::cout << "Game Over\n";
  std::cout << "Your score : " << score << std::endl;

  return 0;
}
