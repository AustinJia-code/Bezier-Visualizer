#include "Slider.hpp"

Slider::Slider (float x, float y, float width, float height, 
                float min, float max, float initial, float scale) 
                : min (min), max(max), val (initial), is_dragging (false)
{
  width *= scale;
  height *= scale;

  // Create the track
  track.setPosition (x, y + height / 2 - 2);
  track.setSize (sf::Vector2f (width, 4 * scale));
  track.setFillColor (sf::Color (100, 100, 100));
  
  // Create the handle
  handle.setRadius (height/2);
  handle.setFillColor (sf::Color (200, 200, 200));
  handle.setOrigin (height / 2, height / 2);
  
  // Set initial handle position
  float handle_x = x + (val - min) / (max - min) * width;
  handle.setPosition (handle_x, y + height / 2);
}

bool Slider::handle_event (const sf::Event& event, const sf::RenderWindow& window)
{
  sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
  
  // Check for mouse press on the handle
  if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
    if (handle.getGlobalBounds().contains (mousePos)) {
      is_dragging = true;
      return true;
    }
  }
  
  // Check for mouse release
  if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
    is_dragging = false;
  }
  
  // Handle dragging
  if (is_dragging && event.type == sf::Event::MouseMoved) {
    // Calculate new handle position, keeping it within the track bounds
    float x = std::max(track.getPosition().x, 
              std::min(mousePos.x, track.getPosition().x + track.getSize().x));
    handle.setPosition(x, handle.getPosition().y);
    
    // Update the current value based on handle position
    float ratio = (x - track.getPosition().x) / track.getSize().x;
    val = min + ratio * (max - min);
    return true;
  }
  
  return false;
}

float Slider::get_value() const
{
  return val;
}

void Slider::draw (sf::RenderTarget& target) const
{
  target.draw(track);
  target.draw(handle);
}