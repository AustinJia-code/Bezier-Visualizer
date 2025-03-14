#include <SFML/Graphics.hpp>

class Slider {
  private:
    sf::RectangleShape track;
    sf::CircleShape handle;
    float min;
    float max;
    float val;
    bool is_dragging;
      
  public:
    Slider (float x, float y, float width, float height, float min, float max, float initial, float scale);
    bool handle_event (const sf::Event& event, const sf::RenderWindow& window);
    float get_value() const;
    void draw (sf::RenderTarget& target) const;
};