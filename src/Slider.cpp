#include "Slider.hpp"

Slider::Slider (float x, float y, float width, float height, 
                float min, float max, float initial, float scale) 
                : _min (min), _max(max), _val (initial), _is_dragging (false)
{
  width *= scale;
  height *= scale;

  // Create the track
  _track.setPosition (x, y + height / 2 - 2);
  _track.setSize (sf::Vector2f (width, 4 * scale));
  _track.setFillColor (sf::Color (100, 100, 100));
  
  // Create the handle
  _handle.setRadius (height/2);
  _handle.setFillColor (sf::Color (200, 200, 200));
  _handle.setOrigin (height / 2, height / 2);
  
  // Set initial handle position
  float handle_x = x + (_val - _min) / (_max - _min) * width;
  _handle.setPosition (handle_x, y + height / 2);
}

bool Slider::handle_event (const sf::Event& event, 
                           const sf::RenderWindow& window)
{
  sf::Vector2f mousePos = window.mapPixelToCoords (
                                  sf::Mouse::getPosition (window));
  
  // Check for mouse press on the handle
  if (event.type == sf::Event::MouseButtonPressed && 
      event.mouseButton.button == sf::Mouse::Left) 
    {
      if (_handle.getGlobalBounds ()
                .contains (mousePos))
        {
          _is_dragging = true;

          return true;
        }
    }
  
  // Check for mouse release
  if (event.type == sf::Event::MouseButtonReleased && 
      event.mouseButton.button == sf::Mouse::Left)
    _is_dragging = false;
  
  // Handle dragging
  if (_is_dragging && event.type == sf::Event::MouseMoved) 
    {
      // Calculate new handle position, keeping it within the track bounds
      float x = std::max (_track.getPosition().x, 
                std::min (mousePos.x, _track.getPosition().x +
                                      _track.getSize().x));

      _handle.setPosition (x, _handle.getPosition().y);
      
      // Update the current value based on handle position
      float ratio = (x - _track.getPosition ().x) / _track.getSize ().x;
      _val = _min + ratio * (_max - _min);

      return true;
    }
  
  return false;
}

float Slider::get_value() const
{
  return _val;
}

void Slider::draw (sf::RenderTarget& target) const
{
  target.draw (_track);
  target.draw (_handle);
}