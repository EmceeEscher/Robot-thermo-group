#ifndef COLLISION_WATCH_HPP
#define COLLISION_WATCH_HPP


namespace CollisionWatch{

  /*
   * main loop which continually checks to see whether or not the collision detectors have been triggered
   */
  void loop();

  /*
   * Returns true if the front collision sensors have been triggered, false otherwise
   */
  bool hasDetectedCollision();

}
#endif
