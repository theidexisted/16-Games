#include <SFML/Graphics.hpp>
#include <algorithm>
#include <ctime>
#include <iostream>

using namespace sf;

constexpr int RowsNum = 20;
constexpr int ColumnsNum = 10;

int field[RowsNum][ColumnsNum] = {0};

struct Point {
  int x, y;
} next_pos[4], tmp_pos[4];

int figures[7][4] = {
    1, 3, 5, 7,  // I
    2, 4, 5, 7,  // Z
    3, 5, 4, 6,  // S
    3, 5, 4, 7,  // T
    2, 3, 5, 7,  // L
    3, 5, 7, 6,  // J
    2, 3, 4, 5,  // O
};

bool escapeButtonStatus = false;
bool pause = false;
unsigned score = 0;

bool check() {
  for (int i = 0; i < 4; i++) {
    if (next_pos[i].x < 0 || next_pos[i].x >= ColumnsNum ||
        next_pos[i].y >= RowsNum) {
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
    const int firstFigure = rand() % 7;
    for (int i = 0; i < 4; i++) {
      next_pos[i].x = figures[firstFigure][i] % 2;
      next_pos[i].y = figures[firstFigure][i] / 2;
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
    for (int i = 0; i < 4; i++) {
      tmp_pos[i] = next_pos[i];
      next_pos[i].x += dx;
    }
    if (!check())
      for (int i = 0; i < 4; i++) next_pos[i] = tmp_pos[i];

    //////Rotate//////
    if (rotate) {
      Point p = next_pos[1];  // center of rotation
      for (int i = 0; i < 4; i++) {
        int x = next_pos[i].y - p.y;
        int y = next_pos[i].x - p.x;
        next_pos[i].x = p.x - x;
        next_pos[i].y = p.y + y;
      }
      if (!check())
        for (int i = 0; i < 4; i++) next_pos[i] = tmp_pos[i];
    }

    ///////Tick//////
    if (timer > delay) {
      for (int i = 0; i < 4; i++) {
        tmp_pos[i] = next_pos[i];
        next_pos[i].y += 1;
      }

      if (!check()) {
        for (int i = 0; i < 4; i++)
          field[tmp_pos[i].y][tmp_pos[i].x] = colorNum;

        colorNum = 1 + rand() % 7;
        int n = rand() % 7;
        for (int i = 0; i < 4; i++) {
          next_pos[i].x = figures[n][i] % 2;
          next_pos[i].y = figures[n][i] / 2;
        }
        if (!check()) {
          window.close();
          break;
        }
      }

      timer = 0;
    }

    ///////check lines//////////
    int k = RowsNum - 1;
    for (int i = RowsNum - 1; i > 0; i--) {
      int count = 0;
      for (int j = 0; j < ColumnsNum; j++) {
        if (field[i][j]) {
          count++;
        }
        field[k][j] = field[i][j];
      }
      if (count < ColumnsNum) {
        k--;
      } else {
        // increase user's score
        ++score;
      }
    }
    if (k != 0) {
      std::fill(field[0], field[0] + (ColumnsNum * (k + 1)), 0);
    }

    dx = 0;
    rotate = 0;
    delay = 0.3;

    /////////draw//////////
    window.clear(Color::White);
    window.draw(background);

    for (int i = 0; i < RowsNum; i++)
      for (int j = 0; j < ColumnsNum; j++) {
        if (field[i][j] == 0) continue;
        tiles_sprite.setTextureRect(IntRect(field[i][j] * 18, 0, 18, 18));
        tiles_sprite.setPosition(j * 18, i * 18);
        tiles_sprite.move(28, 31);  // offset
        window.draw(tiles_sprite);
      }

    for (int i = 0; i < 4; i++) {
      tiles_sprite.setTextureRect(IntRect(colorNum * 18, 0, 18, 18));
      tiles_sprite.setPosition(next_pos[i].x * 18, next_pos[i].y * 18);
      tiles_sprite.move(28, 31);  // offset
      window.draw(tiles_sprite);
    }

    window.draw(frame);
    window.display();
  }

  std::cout << "Game Over\n";
  std::cout << "Your score : " << score << std::endl;

  return 0;
}
