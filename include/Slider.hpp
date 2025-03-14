#include <SFML/Graphics.hpp>

class Slider {
  private:
    sf::RectangleShape track;                   // Background track
    sf::CircleShape handle;                     // Draggable circle
    float min;                                  // Value at full left
    float max;                                  // Value at full right
    float val;                                  // Current value
    bool is_dragging;                           // Is dragging
      
  public:
    /**
     * Constructor
     * @param x,y slider's left-most pixel
     * @param width slider width
     * @param height slider height
     * @param min value at full left
     * @param max value at full right
     * @param initial default position
     * @param scale display scale
     */
    Slider (float x, float y, float width, float height, float min, float max, float initial, float scale);

    /**
     * Called when an event is triggered on the slider
     * @param event event to evaluate
     * @param window window in which event is generated
     */
    bool handle_event (const sf::Event& event, const sf::RenderWindow& window);

    /**
     * Returns the current value
     */
    float get_value() const;

    /**
     * Draws the slider
     * @param target RenderTarget to draw the slider on
     */
    void draw (sf::RenderTarget& target) const;
};