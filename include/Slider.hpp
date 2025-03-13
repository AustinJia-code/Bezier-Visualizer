#include <SFML/Graphics.hpp>

class Slider {
  private:
    sf::RectangleShape track;
    sf::CircleShape handle;
    float min;
    float max;
    float val;
    bool isDragging;
      
  public:
    Slider (float x, float y, float width, float height, float min, float max, float initial, float scale);
    bool handleEvent (const sf::Event& event, const sf::RenderWindow& window);
    float getValue() const;
    void draw (sf::RenderTarget& target) const;
};